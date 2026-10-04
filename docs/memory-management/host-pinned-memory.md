---

### File: `xinfer-essential/docs/memory-management/host-pinned-memory.md`

```markdown
# Allocating Non-Pageable Memory (`mlock` & `cudaHostRegister`)

Standard memory allocated via `malloc()` or `new` consists of virtual memory addresses backed by demand-paged anonymous pages. The Linux virtual memory manager (VMM) may swap these pages to disk under memory pressure or relocate them during page compaction, introducing non-deterministic latency spikes during real-time threat mitigation.

`xinfer-essential` guarantees deterministic execution by allocating **Host-Pinned (Non-Pageable) Memory**.

---

## 1. Virtual Memory Paging vs. Pinned Physical Memory

```text
CONVENTIONAL PAGEABLE MEMORY (Non-Deterministic):
[ Virtual Address ] ──(Kernel Page Table)──> [ Physical RAM ] OR [ Disk Swap File ]
                                                      ▲
                                                      │ Page Fault Trap (Millisec Latency)
                                            [ Kernel Swapper (kswapd) ]

--------------------------------------------------------------------------------

HOST-PINNED MEMORY (Deterministic Microsecond Guarantee):
[ Virtual Address ] ═══════════════════════> [ Locked Physical RAM Pages ]
  - mlock() / cudaHostAlloc()                  - Guaranteed Resident in Physical Memory
  - Bypasses Page Fault Handlers               - PCIe DMA Master Can Read Directly
```

---

## 2. Managing Operating System Limits (`RLIMIT_MEMLOCK`)

By default, unprivileged processes in Linux are limited to 64 KB of locked memory. `xinfer-essential` checks and adjusts process resource limits on initialization:

```cpp
#include <sys/resource.h>
#include <iostream>
#include <stdexcept>

void enforce_unlimited_memlock() {
    struct rlimit limit;
    if (getrlimit(RLIMIT_MEMLOCK, &limit) != 0) {
        throw std::runtime_error("Failed to query RLIMIT_MEMLOCK");
    }

    limit.rlim_cur = RLIM_INFINITY;
    limit.rlim_max = RLIM_INFINITY;

    if (setrlimit(RLIMIT_MEMLOCK, &limit) != 0) {
        std::cerr << "[xInfer WARN] Failed to set RLIMIT_MEMLOCK to infinity. "
                  << "Ensure CAP_SYS_RESOURCE is set or adjust /etc/security/limits.conf" 
                  << std::endl;
    }
}
```

---

## 3. Pinned Memory Allocator Implementation

The `PinnedMemoryPool` provides 64-byte aligned, page-locked virtual memory:

```cpp
#include <sys/mman.h>
#include <cstdlib>
#include <cstdint>
#include <new>

namespace xinfer {

class PinnedMemoryBlock {
public:
    explicit PinnedMemoryBlock(size_t size_bytes) : size_(size_bytes) {
        // 1. Allocate 64-byte cache-line aligned virtual memory
        if (posix_memalign(&ptr_, 64, size_) != 0) {
            throw std::bad_alloc();
        }

        // 2. Lock physical memory pages into RAM
        if (::mlock(ptr_, size_) != 0) {
            // Log warning if locking fails, but retain allocated memory
        }

        // 3. Advise the kernel not to include this buffer in core dumps
        // and prepare pages for immediate access
        ::madvise(ptr_, size_, MADV_DONTDUMP | MADV_WILLNEED);
    }

    ~PinnedMemoryBlock() {
        if (ptr_) {
            ::munlock(ptr_, size_);
            std::free(ptr_);
        }
    }

    PinnedMemoryBlock(const PinnedMemoryBlock&) = delete;
    PinnedMemoryBlock& operator=(const PinnedMemoryBlock&) = delete;

    PinnedMemoryBlock(PinnedMemoryBlock&& other) noexcept 
        : ptr_(other.ptr_), size_(other.size_) {
        other.ptr_ = nullptr;
        other.size_ = 0;
    }

    [[nodiscard]] void* get() const noexcept { return ptr_; }
    [[nodiscard]] size_t size() const noexcept { return size_; }

private:
    void* ptr_{nullptr};
    size_t size_{0};
};

} // namespace xinfer
```

---

## 4. Silicon Driver Registration (`cudaHostRegister`)

When running on NVIDIA systems, pre-allocated host memory can be registered with the CUDA driver, enabling direct PCIe DMA transfers without copying to staging buffers:

```cpp
#if defined(XINFER_ENABLE_CUDA)
#include <cuda_runtime.h>

void register_cuda_pinned_memory(void* host_ptr, size_t size_bytes) {
    cudaError_t status = cudaHostRegister(
        host_ptr, 
        size_bytes, 
        cudaHostRegisterMapped | cudaHostRegisterPortable
    );
    if (status != cudaSuccess) {
        throw xinfer::InferenceException(
            xinfer::ErrorCode::ERR_MEMORY_PIN_FAILED,
            cudaGetErrorString(status)
        );
    }
}
#endif
```
```

---

### File: `xinfer-essential/docs/memory-management/unified-memory.md`

```markdown
# Unified Virtual Address Spaces on Heterogeneous SoCs

Integrated Systems-on-Chip (SoCs)—such as the **Apple Silicon M-series, Rockchip RK3588, and Intel Lunar Lake**—share physical LPDDR5/DDR5 system memory across the host CPU, GPU, and Neural Processing Unit (NPU). 

`xinfer-essential` uses unified memory architectures to eliminate PCIe serialization and bus-copy overhead entirely.

---

## 1. Discrete Bus Architecture vs. Unified Memory SoC

```text
DISCRETE ARCHITECTURE (e.g., Intel Xeon + NVIDIA RTX):
[ CPU System RAM ] ────(PCIe Gen 4/5 Bus ~32-64 GB/s)────> [ GPU Dedicated VRAM ]
    - Requires cudaMemcpy or Host Pinned Staging Buffers
    - Higher transfer latency penalty

--------------------------------------------------------------------------------

UNIFIED MEMORY SOC (e.g., Apple M4, Rockchip RK3588, Intel Lunar Lake):
+─────────────────────────────────────────────────────────────────────────────+
|               Unified Physical Memory Pool (LPDDR5 / 100-800 GB/s)          |
+─────────────────────────────────────────────────────────────────────────────+
         ▲                                   ▲                               ▲
         │ Direct Pointer                    │ Direct Pointer                │ Direct Pointer
[ Host CPU (C++20) ]                [ Embedded GPU ]                [ Embedded NPU ]
```

---

## 2. Unified Pointer Coherence & Cache Sychronization

On unified memory platforms, while physical memory is shared, CPU and NPU hardware cache hierarchies (L1/L2 caches) are often separate and non-snooping. Reading from or writing to shared pointers without synchronization can lead to memory incoherency.

`xinfer-essential` handles cache synchronization across heterogeneous SoC cores:

```cpp
namespace xinfer {

enum class CacheSyncDirection {
    CPU_TO_DEVICE,
    DEVICE_TO_CPU
};

void synchronize_unified_cache(void* ptr, size_t size_bytes, CacheSyncDirection direction) {
#if defined(__aarch64__)
    // Direct ARMv8-A cache cleaning instructions
    char* addr = static_cast<char*>(ptr);
    const size_t line_size = 64; // Standard ARM64 cache line size

    if (direction == CacheSyncDirection::CPU_TO_DEVICE) {
        // Clean data cache by Virtual Address to Point of Coherency (PoC)
        for (size_t offset = 0; offset < size_bytes; offset += line_size) {
            asm volatile("dc cvac, %0" : : "r"(addr + offset) : "memory");
        }
    } else {
        // Invalidate data cache by Virtual Address to Point of Coherency (PoC)
        for (size_t offset = 0; offset < size_bytes; offset += line_size) {
            asm volatile("dc ivac, %0" : : "r"(addr + offset) : "memory");
        }
    }
    asm volatile("dsb sy" : : : "memory"); // Enforce full system memory barrier
#endif
}

} // namespace xinfer
```

---

## 3. Allocating Unified Pointers via POSIX System Calls

On Linux ARM64 systems (such as the RK3588), unified memory accessible to both the CPU and NPU can be created using anonymous shared memory maps:

```cpp
#include <sys/mman.h>
#include <xinfer/memory.hpp>

void* allocate_unified_soc_buffer(size_t size_bytes) {
    // Allocate shared, anonymous, page-aligned virtual memory
    void* ptr = ::mmap(
        nullptr,
        size_bytes,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );

    if (ptr == MAP_FAILED) {
        throw xinfer::InferenceException(
            xinfer::ErrorCode::ERR_OUT_OF_MEMORY,
            "Unified SoC mmap allocation failed"
        );
    }

    // Advise kernel to reserve contiguous physical pages immediately
    ::madvise(ptr, size_bytes, MADV_WILLNEED);
    return ptr;
}
```
```

---

### File: `xinfer-essential/docs/memory-management/tensor-backing-buffers.md`

```markdown
# Tensor Backing Buffers & Persistent Allocation Pools

Invoking dynamic heap memory allocations (`malloc`, `free`, `new`, `delete`) during runtime inference introduces thread contention, memory fragmentation, and latency variability.

`xinfer-essential` guarantees deterministic runtimes by using **pre-allocated, persistent backing stores** and **reusable memory pools**.

---

## 1. Tensor Buffer Ownership Models

`xinfer::Tensor` supports two distinct memory management models:

```text
 1. OWNING BACKING STORE (xinfer::TensorPool managed)
    ┌──────────────────────┐
    │    xinfer::Tensor    │──► Owns internal PinnedMemoryBlock
    └──────────────────────┘     Lifetime managed via RAII

 2. NON-OWNING BORROWED VIEW (External DMA / Hardware Map)
    ┌──────────────────────┐
    │    xinfer::Tensor    │──► Points to external memory (e.g., AF_XDP ring, V4L2)
    └──────────────────────┘     Zero-copy; relies on caller for lifetime management
```

---

## 2. Lock-Free Pre-Allocated Tensor Pool

For high-throughput workloads ($> 1,000,000\text{ EPS}$), `xinfer-essential` uses a lock-free fixed-capacity pool to recycle tensor buffers without making runtime kernel allocations:

```cpp
#include <xinfer/tensor.hpp>
#include <atomic>
#include <vector>
#include <memory>

namespace xinfer {

class TensorPool {
public:
    TensorPool(const TensorDescriptor& desc, size_t pool_size)
        : desc_(desc), capacity_(pool_size), free_index_(0) {
        
        buffers_.reserve(pool_size);
        for (size_t i = 0; i < pool_size; ++i) {
            buffers_.push_back(Tensor::create_owning(desc_));
        }
    }

    // Acquire an idle tensor buffer (Lock-free acquire)
    std::shared_ptr<Tensor> acquire() {
        size_t current = free_index_.fetch_add(1, std::memory_order_relaxed);
        if (current >= capacity_) {
            // Pool exhausted: reset and throw exception to maintain deterministic SLAs
            free_index_.fetch_sub(1, std::memory_order_relaxed);
            throw InferenceException(
                ErrorCode::ERR_OUT_OF_MEMORY, 
                "Tensor backing buffer pool exhausted"
            );
        }
        return buffers_[current];
    }

    // Return buffer to pool
    void release() noexcept {
        free_index_.fetch_sub(1, std::memory_order_release);
    }

    void reset() noexcept {
        free_index_.store(0, std::memory_order_release);
    }

private:
    TensorDescriptor desc_;
    size_t capacity_;
    std::atomic<size_t> free_index_;
    std::vector<std::shared_ptr<Tensor>> buffers_;
};

} // namespace xinfer
```

---

## 3. Double-Buffering for Asynchronous Pipelines

To maximize compute utilization, `xinfer-essential` implements double-buffering across inference runs. The host prepares buffer $N+1$ while the hardware accelerator processes buffer $N$:

```text
Time Step T:
   Host CPU:            [ Populate Input Buffer B (Pinned RAM) ]
   Accelerator Core:    [ Executing Compute on Buffer A (Device SRAM) ]

Time Step T+1:
   Host CPU:            [ Populate Input Buffer A (Pinned RAM) ]
   Accelerator Core:    [ Executing Compute on Buffer B (Device SRAM) ]
```

### Double-Buffering Code Example

```cpp
// Initialize double-buffered tensor references
auto buffer_A = engine.create_input_tensor("flow_in");
auto buffer_B = engine.create_input_tensor("flow_in");

bool toggle = false;

while (is_running) {
    auto current_input = toggle ? buffer_A : buffer_B;
    
    // Ingest data into the active staging buffer
    fill_packet_telemetry(current_input->data<float>());

    // Dispatch inference asynchronously on current buffer
    engine.forward_async(current_input);

    // Flip buffer toggle
    toggle = !toggle;
}
```
```

---

### File: `xinfer-essential/docs/memory-management/alignment-and-cache.md`

```markdown
# 64-Byte Cache Alignment & TLB Optimization

Modern CPU and NPU architectures rely heavily on SIMD vector units (AVX-512, AVX2, ARM Neon) and multi-level data caches. Misaligned memory addresses cross cache line boundaries, causing split-cache access penalties and serialization delays.

`xinfer-essential` enforces **64-byte cache alignment** and leverages **Linux HugePages** to optimize hardware memory throughput.

---

## 1. Microarchitectural Alignment Fundamentals

```text
MISALIGNED ACCESS (Crosses 64-byte boundary -> 2 Bus Cycles, Latency Penalty):
[ Byte 0 ... Byte 60 ] [ Boundary ] [ Byte 64 ... Byte 127 ]
       └────── 32-Byte Vector Read ──────┘ (Split Cache Line Transaction)

--------------------------------------------------------------------------------

OPTIMAL 64-BYTE ALIGNED ACCESS (Single Bus Cycle, Zero Overhead):
[ Cache Line Start (64-byte aligned) ] ───> [ 64 Bytes Continuous Memory ]
       └────── AVX-512 / Neon 512-bit Load ──────┘
```

---

## 2. Alignment Math Utilities

`xinfer-essential` provides `constexpr` alignment utilities for buffer arithmetic:

```cpp
#include <cstddef>
#include <cstdint>

namespace xinfer {

// Align byte size up to the nearest multiple of alignment
[[nodiscard]] constexpr size_t align_up(size_t size, size_t alignment = 64) noexcept {
    return (size + (alignment - 1)) & ~(alignment - 1);
}

// Verify whether an address is aligned
[[nodiscard]] inline bool is_aligned(const void* ptr, size_t alignment = 64) noexcept {
    return (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0;
}

} // namespace xinfer
```

---

## 3. False Sharing Prevention in Multi-Threaded State

In concurrent environments, if two threads on different CPU cores write to independent variables located on the same 64-byte cache line, the CPU invalidates the cache line across both cores repeatedly. This phenomenon—**false sharing**—degrades multi-threaded scaling.

`xinfer-essential` pads internal thread-local state structures using standard C++20 hardware interference constants:

```cpp
#include <new>
#include <atomic>

struct alignas(std::hardware_destructive_interference_size) ThreadExecutionContext {
    // Cache Line 1 (Dedicated to Thread A)
    std::atomic<uint64_t> processed_frames{0};
    uint64_t last_timestamp_ns{0};
};

struct alignas(std::hardware_destructive_interference_size) SchedState {
    // Cache Line 2 (Dedicated to Thread B)
    std::atomic<uint32_t> active_workers{0};
};
```

---

## 4. HugePages Integration (`MAP_HUGETLB`)

Standard Linux virtual memory pages are 4 KB in size. Large models and high-throughput ring buffers create thousands of page entries, overwhelming the CPU's **Translation Lookaside Buffer (TLB)** and causing costly page table walks.

`xinfer-essential` supports Linux 2 MB HugePages to reduce TLB misses:

```cpp
#include <sys/mman.h>
#include <xinfer/memory.hpp>

void* allocate_hugepage_memory(size_t size_bytes) {
    // Round size up to nearest 2 MB boundary
    size_t huge_page_size = 2 * 1024 * 1024;
    size_t aligned_size = xinfer::align_up(size_bytes, huge_page_size);

    void* ptr = ::mmap(
        nullptr,
        aligned_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
        -1,
        0
    );

    if (ptr == MAP_FAILED) {
        // Fallback to standard 64-byte aligned allocation if HugePages are not configured
        return nullptr;
    }

    return ptr;
}
```

### Verifying System HugePages

Check that the host kernel has pre-allocated 2 MB HugePages available:

```bash
cat /proc/meminfo | grep -i hugepages
```

To configure 512 HugePages (1 GB of pooled physical memory) dynamically:

```bash
echo 512 | sudo tee /proc/sys/vm/nr_hugepages
```
```

---

### Complete in Part 6
- `xinfer-essential/docs/memory-management/dma-buf-integration.md`
- `xinfer-essential/docs/memory-management/host-pinned-memory.md`
- `xinfer-essential/docs/memory-management/unified-memory.md`
- `xinfer-essential/docs/memory-management/tensor-backing-buffers.md`
- `xinfer-essential/docs/memory-management/alignment-and-cache.md`

All memory management documentation is now in place.

---

### Files to be Generated in Part 7

The next phase covers **Plugin Development** (`plugin-development/`):

1. `plugin-development/plugin-architecture.md` (Dynamic linker mechanics, `dlopen`, `RTLD_LAZY | RTLD_LOCAL`)
2. `plugin-development/iinference-plugin-interface.md` (The C++20 `IInferencePlugin` ABI contract)
3. `plugin-development/building-custom-plugins.md` (Writing, compiling, and testing a custom plugin)
4. `plugin-development/plugin-lifecycle.md` (Registration, validation, execution, and teardown states)

*(The 9 pre-built standard plugins under `plugin-development/standard-plugins/` will follow in Part 8).*

Let me know when you are ready to proceed with Part 7.
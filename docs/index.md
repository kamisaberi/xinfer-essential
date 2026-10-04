# xInfer Essential (`libxinfer.so`)

**Universal Heterogeneous C++20 AI Inference Runtime**  
*Tier 1 Foundational Engine of the Aryorithm / Blackbox Sentinel Ecosystem*

---

## Executive Architectural Overview

`xinfer-essential` is a low-latency, zero-copy C++20 inference engine designed for edge appliances, industrial cyber-physical hardware, and real-time defense networks. It abstracts complex, vendor-specific neural acceleration APIs behind a unified, cache-aligned tensor interface.

```
+-----------------------------------------------------------------------------+
|                          Application / Host Process                         |
|                 (e.g., Blackbox-Sentinel, EDR, SCADA Monitor)               |
+-----------------------------------------------------------------------------+
                                       │
                                       ▼
+─────────────────────────────────────────────────────────────────────────────+
|                         xinfer::InferenceEngine                             |
|    - Memory Domain Arbiter           - Direct Zero-Copy DMA Exchange        |
|    - Dynamic Backend Selector        - Strict Symbol Isolation Boundary     |
+─────────────────────────────────────────────────────────────────────────────+
         │                       │                         │
         ▼                       ▼                         ▼
+─────────────────+    +───────────────────+    +─────────────────────+
|   libxinfer_    |    |    libxinfer_     |    |     libxinfer_      |
|   tensorrt.so   |    |   openvino.so     |    |      rknn.so        |
|  (NVIDIA GPUs)  |    | (Intel NPU/iGPU)  |    |  (Rockchip RK3588)  |
+─────────────────+    +───────────────────+    +─────────────────────+
         │                       │                         │
         ▼                       ▼                         ▼
+─────────────────+    +───────────────────+    +─────────────────────+
|   CUDA Streams  |    | ov::RemoteContext |    |  /dev/rknpu (DRM)   |
| Host-Pinned Mem |    | Shared DMA-BUF    |    | Zero-Copy Buffer IO |
+─────────────────+    +───────────────────+    +─────────────────────+
```

### Core Invariants

1. **Deterministic Execution:** No runtime memory allocation in the inference fast-path. Tensor buffers are pre-allocated and pinned during initialization.
2. **Zero-Copy Memory Transport:** Direct integration with Linux kernel `DMA-BUF` file descriptors, CUDA Host-Pinned memory (`cudaHostAlloc`), and Intel OpenVINO remote tensors.
3. **Strict ABI Isolation:** Compiled with `-fvisibility=hidden`. Only routines explicitly marked with `XINFER_API` are exported across shared library boundaries.
4. **Zero Managed Runtimes:** Implemented in pure ISO C++20 without Python, Java, or Node dependencies in the evaluation path.
5. **Silicon Agnostic:** Unified dynamic dispatch across 15 discrete accelerator architectures.

---

## Hardware Target Matrix

| Accelerator Architecture | Target Silicon Family | Driver / Subsystem Hook | Native Precision |
| :--- | :--- | :--- | :--- |
| **NVIDIA TensorRT** | Orin Nano, AGX Orin, RTX A4000+ | CUDA Streams / cuDNN | FP32, FP16, INT8 |
| **Intel OpenVINO** | Core Ultra NPUs, Meteor Lake, Xeon | OneVPL / Level Zero / i915 | FP32, FP16, BF16, INT8 |
| **Rockchip RKNN** | RK3588, RK3588S, RK3576 | RKNPU2 / DRM IOCTL | FP16, INT8, INT4 |
| **Qualcomm QNN** | Snapdragon X Elite, SA8295P | FastRPC / Hexagon DSP | FP16, INT8 |
| **AMD Vitis AI** | Versal AI Edge, Kria SOMs, Zynq | XRT (Xilinx Runtime) / DPU | INT8, BFP16 |
| **Apple CoreML** | Apple Silicon (M1/M2/M3/M4 Series) | Metal Performance Shaders / ANE | FP32, FP16 |
| **AMD Ryzen AI** | Ryzen 7040/8040, Strix Point | XDNA Driver / DirectML | FP32, FP16, INT8 |
| **MediaTek NeuroPilot** | Dimensity 9300, Genio 1200 | APU Direct Driver / ION Memory | FP16, INT8 |
| **Hailo HailoRT** | Hailo-8, Hailo-8L M.2 Modules | PCIe Driver (`/dev/hailo0`) | INT8, INT16 |
| **Ambarella CVFlow** | CV2x, CV5x, CV7x Systems-on-Chip | Cavalry Driver (`/dev/cavalry`) | INT8, FP16 |
| **Samsung ENN** | Exynos 990, 2200, 2400 | ENN Driver Framebuffer | FP16, INT8 |
| **Google Coral** | Edge TPU (PCIe, M.2, Dual Edge) | `libedgetpu1-std` / Gasket | INT8 |
| **Intel FPGA AI Suite** | Agilex 7, Arria 10, Cyclone V | OpenCL / PCIe AXI Master | Custom Fixed-Point |
| **Microchip VectorBlox** | PolarFire SoC FPGA | AXI Shared Memory Driver | INT8 |
| **Lattice sensAI** | iCE40 UltraPlus, CrossLink-NX | Direct SPI / Wishbone Bridge | INT1, INT8 |

---

## 30-Second Verification Example

```cpp
#include <xinfer/xinfer.hpp>
#include <iostream>
#include <vector>

int main() {
    // 1. Initialize configuration with explicit backend targeting
    xinfer::EngineConfig config;
    config.model_path = "/opt/models/network_threat_v2.onnx";
    config.backend = xinfer::BackendType::AUTO; // Autodetects fastest available accelerator
    config.precision = xinfer::Precision::FP16;
    config.enable_zero_copy = true;

    // 2. Instantiate and compile execution plan
    xinfer::InferenceEngine engine(config);
    engine.initialize();

    // 3. Acquire pre-allocated zero-copy input view
    auto input_tensor = engine.get_input_tensor("flow_features");
    
    // Fill input vector (32-dim normalized flow vector)
    std::vector<float> sample(32, 0.5f);
    input_tensor->copy_from_host(sample.data(), sample.size() * sizeof(float));

    // 4. Synchronous execution (sub-microsecond execution profile)
    engine.forward();

    // 5. Read output prediction
    auto output_tensor = engine.get_output_tensor("threat_score");
    float anomaly_score = *output_tensor->data<float>();

    std::cout << "[xInfer] Anomaly Score: " << anomaly_score 
              << " | Latency: " << engine.last_inference_microseconds() << " us" << std::endl;

    return 0;
}
```

---

## Key Performance Indicators

* **Input-to-Output Fastpath Latency:** $< 11.4\,\mu\text{s}$ on Intel Core Ultra 7 165H (OpenVINO NPU, 32-dim tabular NetFlow).
* **Direct DMA Throughput:** $12.8\,\text{GB/s}$ sustained zero-copy ingestion over Linux DMA-BUF.
* **Heap Churn:** $0$ allocations during continuous forward passes.
* **Linker Footprint:** $< 4.2\,\text{MB}$ release shared library binary footprint (`libxinfer.so`).
```

---

### File: `xinfer-essential/docs/getting-started/overview.md`

```markdown
# High-Level Runtime Capabilities & Design Principles

`xinfer-essential` delivers a consistent C++20 execution abstraction over diverse hardware acceleration drivers. It is designed for low-latency operational environments that cannot absorb the memory overhead, garbage collection pauses, or dynamic allocation behavior of general-purpose runtimes.

---

## 1. Zero Managed Runtime Architecture

Traditional ML inference frameworks frequently bundle heavy runtimes or assume a Python execution layer. This introduces non-deterministic execution times, heap fragmentation, and security vulnerabilities inside air-gapped embedded environments.

`xinfer-essential` operates as a native shared library (`libxinfer.so`):

* **Pure ISO C++20:** Built with modern standard idioms including `std::span`, concepts, memory barriers, and RAII resource pools.
* **Deterministic Allocation:** All buffers, device contexts, scratchpads, and DMA-BUF descriptors are mapped during model load. The forward pass invokes zero calls to `malloc()`, `calloc()`, or `new`.
* **Zero Egress:** No background telemetry, telemetry rings, or dynamic asset downloads occur unless explicitly configured through `xinfer::ModelHub`.

---

## 2. Decoupled Backend Model (`IInferencePlugin`)

The core library does not link against silicon vendor SDKs at compile time. Instead, it relies on dynamically loaded runtime plugins adhering to the `IInferencePlugin` ABI contract.

```
                    +-----------------------+
                    | xinfer::PluginManager |
                    +-----------------------+
                                │
        ┌───────────────────────┼───────────────────────┐
        ▼                       ▼                       ▼
  libxinfer_openvino.so   libxinfer_tensorrt.so   libxinfer_rknn.so
  (Intel Level-Zero)       (CUDA Runtime API)      (Librknpu2 / DRM)
```

### Architectural Benefits

* **Minimal Base Footprint:** A host deployment without NVIDIA drivers installed will not encounter broken shared library links (`libcuda.so.1 not found`).
* **Hot-Pluggable Upgrades:** Driver-specific plugin libraries can be upgraded independently without recompiling host control daemons.
* **Graceful Degradation:** If an edge NPU crashes or fails hardware initialization, the engine falls back to optimized CPU SIMD (AVX-512 / ARM Neon) within the same execution cycle.

---

## 3. Wire-Speed Zero-Copy Memory Pipelines

The primary performance bottleneck in edge AI is memory copying across bus boundaries. Passing a 1080p frame or high-frequency NetFlow buffer through user-space memory, kernel socket memory, and GPU driver buffers degrades throughput.

`xinfer-essential` eliminates memory intermediaries through three zero-copy mechanisms:

1. **Linux DMA-BUF Sharing:** Direct file descriptor binding from network drivers (XDP/eBPF) or camera interfaces (V4L2) directly into NPU hardware address spaces.
2. **Pinned Host Memory Pools:** Lock-free, pre-allocated host memory rings mapped via `mlock()` and vendor-specific pinned allocation primitives (`cudaHostRegister`).
3. **Unified Memory Sharing:** Single virtual address space mapping across integrated architectures such as the Rockchip RK3588, Apple Silicon Unified Memory, and Intel Lunar Lake memory configurations.

---

## 4. Built-in Cryptographic Model Governance

Edge models operating on untrusted nodes risk tampering, adversarial poisoning, and parameter interception. `xinfer-essential` enforces security checkpoints prior to execution:

* **In-Memory Decryption:** Native AES-256-GCM unbundling directly into RAM; decrypted weights are never written to physical swap or temporary filesystems.
* **Strict SHA-256 Hashing:** Automated verification against signed manifests before execution handles are bound.
* **TPM 2.0 PCR Sealing:** Key derivation secured against Platform Configuration Registers to guarantee weight decryption only occurs on validated hardware.
```

---

### File: `xinfer-essential/docs/getting-started/system-requirements.md`

```markdown
# System Requirements & Prerequisites

Review the toolchain, operating system, and silicon driver requirements before compiling or running `xinfer-essential`.

---

## 1. Host Operating System & Architecture

`xinfer-essential` requires a 64-bit POSIX-compliant operating system with modern kernel DMA-BUF and asynchronous I/O capabilities.

| Operating System | Minimum Version | Recommended Target | Supported Architectures |
| :--- | :--- | :--- | :--- |
| **Ubuntu Linux** | 22.04 LTS | 24.04 LTS / 26.04 Devel | `x86_64`, `aarch64` |
| **Debian** | 12 (Bookworm) | 12 (Bookworm) | `x86_64`, `aarch64` |
| **Red Hat Enterprise Linux** | 9.0 | 9.4 | `x86_64`, `aarch64` |
| **macOS** | 13.0 (Ventura) | 14.0+ (Sonoma) | `arm64` (Apple Silicon) |

!!! note "Linux Kernel Requirements"
    Linux kernel versions **>= 5.15** are required. Kernels **>= 6.5** are recommended to use the latest `dma-buf` zero-copy primitives and Intel Level Zero direct memory export routines.

---

## 2. Compiler & Toolchain Standards

`xinfer-essential` utilizes ISO C++20 features, including concepts, designated initializers, `std::span`, and formatting routines.

* **GCC:** Version **12.1.0** or newer.
* **Clang/LLVM:** Version **16.0.0** or newer.
* **CMake:** Version **3.24.0** or newer.
* **Ninja Build:** Version **1.10.0+** (recommended for parallel compilation).
* **GNU C Library (glibc):** Version **2.35+** (tested up to `glibc 2.43`).

---

## 3. Hardware Accelerator Toolkits & Drivers

To enable target-specific execution backends, ensure the corresponding user-space libraries and kernel drivers are installed on the build or deployment host:

### Intel Platforms (OpenVINO / NPU / iGPU)
* **Intel OpenVINO Toolkit:** Version `2024.1.0` or higher.
* **Intel Level Zero Driver:** Version `1.15.1+`.
* **Intel OneVPL:** `2023.3+` (for hardware-accelerated video decoding).

### NVIDIA Platforms (CUDA / TensorRT)
* **NVIDIA Driver:** Version `>= 535.104.05`.
* **CUDA Toolkit:** Version `12.0` or higher.
* **TensorRT:** Version `8.6` through `10.x`.

### Rockchip Embedded (RK3588, RK3576)
* **RKNPU2 Driver:** Version `>= 1.6.0`.
* **Rockchip DRM Kernel Driver:** Built with `CONFIG_DRM_ROCKCHIP=y`.

### Qualcomm Platforms
* **Qualcomm Neural Processing SDK:** Version `2.18+`.
* **Hexagon DSP Compute Driver:** FastRPC kernel subsystem enabled.

### Hailo Platforms
* **HailoRT:** Version `4.16.0+`.
* **PCIe Driver Module:** `hailo_pci` running and verified via `hailortcli fw-control identify`.

---

## 4. Hardware Sizing Guidelines

```
+--------------------------------------------------------------------------+
| Micro Edge Configuration (Rockchip RK3588 / Hailo-8 M.2 / Raspberry Pi 5)|
|  - RAM: 4 GB minimum (Unified Memory)                                    |
|  - Storage: 16 GB eMMC / NVMe SSD                                        |
|  - Throughput: Up to 15,000 Inference Ops/Second                        |
+--------------------------------------------------------------------------+

+--------------------------------------------------------------------------+
| Enterprise High-Throughput Edge Node (Intel Xeon / Dual RTX A4000)       |
|  - RAM: 32 GB DDR5 (ECC Supported)                                       |
|  - PCIe: Gen 4 x16 / Gen 5 x16                                           |
|  - Storage: 100 GB Enterprise NVMe SSD                                    |
|  - Throughput: Up to 1,250,000 Inference Ops/Second                      |
+--------------------------------------------------------------------------+
```
```


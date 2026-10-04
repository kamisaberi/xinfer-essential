### Part 10: Complete C++20 API Reference (`api-reference/*`)

This section contains the formal C++20 API specification for `xinfer-essential`: namespace design, execution classes, memory handles, plugin registries, configuration schemas, type systems, and exception hierarchies.

---

### File: `xinfer-essential/docs/api-reference/index.md`

```markdown
# API Reference Overview

The `xinfer-essential` C++20 API is designed for mission-critical, low-latency applications that require deterministic memory ownership and direct hardware dispatch.

---

## 1. Header Organization

To use `xinfer-essential`, include the primary umbrella header:

```cpp
#include <xinfer/xinfer.hpp>
```

Alternatively, include modular headers for granular compilation:

| Header File | Primary Declarations |
| :--- | :--- |
| `<xinfer/inference_engine.hpp>` | Class `xinfer::InferenceEngine`, `ExecutionContext` |
| `<xinfer/tensor.hpp>` | Class `xinfer::Tensor`, `TensorDescriptor` |
| `<xinfer/engine_config.hpp>` | Struct `xinfer::EngineConfig` |
| `<xinfer/data_types.hpp>` | Enums `BackendType`, `Precision`, `MemoryType`, `DataType` |
| `<xinfer/model_hub.hpp>` | Class `xinfer::ModelHub`, `ModelArtifact` |
| `<xinfer/plugin_manager.hpp>` | Class `xinfer::PluginManager`, `IInferencePlugin` |
| `<xinfer/exception.hpp>` | Class `xinfer::InferenceException`, enum `ErrorCode` |

---

## 2. API Design Principles

1. **Deterministic Memory Ownership:** Tensors wrap pre-allocated memory via non-owning `std::span` views or own cache-aligned buffers through RAII.
2. **Explicit Error Boundaries:** Fast-path forward calls avoid dynamic error allocation by using standard return flags or throwing typed `InferenceException` hierarchies when unrecoverable hardware faults occur.
3. **Thread Confinement:** Shared model parameters remain strictly immutable; dynamic state is isolated within thread-local execution contexts.
4. **Zero-Overhead Abstractions:** Member accessors, alignment checks, and stride conversions are marked `constexpr` and `noexcept`.
```

---

### File: `xinfer-essential/docs/api-reference/inference-engine.md`

```markdown
# Class `xinfer::InferenceEngine`

Defined in header `<xinfer/inference_engine.hpp>`  
Namespace: `xinfer`

`InferenceEngine` is the central orchestrator responsible for loading models, dispatching computation to hardware accelerator plugins, and managing input/output tensor views.

---

## 1. Class Synopsis

```cpp
namespace xinfer {

class XINFER_API InferenceEngine {
public:
    InferenceEngine();
    explicit InferenceEngine(const EngineConfig& config);
    ~InferenceEngine();

    // Non-copyable, movable
    InferenceEngine(const InferenceEngine&) = delete;
    InferenceEngine& operator=(const InferenceEngine&) = delete;
    InferenceEngine(InferenceEngine&&) noexcept;
    InferenceEngine& operator=(InferenceEngine&&) noexcept;

    // Initialization & Lifecycle
    void configure(const EngineConfig& config);
    void initialize();
    void shutdown() noexcept;

    // Fast-Path Compute Execution
    void forward();
    void forward_async(std::function<void(ErrorCode, double)> callback);
    void synchronize();

    // Tensor Management
    [[nodiscard]] std::shared_ptr<Tensor> get_input_tensor(std::string_view name);
    [[nodiscard]] std::shared_ptr<Tensor> get_input_tensor(size_t index);
    [[nodiscard]] std::shared_ptr<const Tensor> get_output_tensor(std::string_view name) const;
    [[nodiscard]] std::shared_ptr<const Tensor> get_output_tensor(size_t index) const;
    void bind_input(std::string_view name, std::shared_ptr<Tensor> tensor);

    // Introspection & Diagnostics
    [[nodiscard]] double last_inference_microseconds() const noexcept;
    [[nodiscard]] std::string_view get_active_backend_name() const noexcept;
    [[nodiscard]] size_t get_input_count() const noexcept;
    [[nodiscard]] size_t get_output_count() const noexcept;
    [[nodiscard]] TensorDescriptor get_input_descriptor(size_t index) const;
    [[nodiscard]] TensorDescriptor get_output_descriptor(size_t index) const;

    // Multi-Threaded Execution
    [[nodiscard]] std::unique_ptr<ExecutionContext> create_execution_context();

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace xinfer
```

---

## 2. Member Functions

### `configure`
```cpp
void configure(const EngineConfig& config);
```
Sets the engine operational parameters. Must be invoked before `initialize()`. Throws `InferenceException` if the configuration parameters are invalid.

---

### `initialize`
```cpp
void initialize();
```
Dispatches to `PluginManager` to load the appropriate hardware backend, validates the model SHA-256 hash via `ModelHub`, pins memory scratchpads, and compiles the hardware execution graph.

---

### `forward`
```cpp
void forward();
```
Executes a synchronous forward inference pass using the currently bound input tensors. This method blocks until hardware computation completes. Guarantees zero heap allocation in steady state.

---

### `get_input_tensor`
```cpp
std::shared_ptr<Tensor> get_input_tensor(std::string_view name);
std::shared_ptr<Tensor> get_input_tensor(size_t index);
```
Acquires a handle to an input tensor buffer by port name or ordinal index. The underlying memory is pre-allocated and pinned.

---

### `bind_input`
```cpp
void bind_input(std::string_view name, std::shared_ptr<Tensor> tensor);
```
Binds an externally created tensor (e.g., a Linux kernel DMA-BUF descriptor or host-pinned buffer) to the target input port. Enables direct zero-copy data ingestion.

---

### `create_execution_context`
```cpp
std::unique_ptr<ExecutionContext> create_execution_context();
```
Creates an independent, thread-confined execution context sharing the engine's immutable model weights. Used for multi-threaded inference scaling.
```

---

### File: `xinfer-essential/docs/api-reference/tensor.md`

```markdown
# Class `xinfer::Tensor`

Defined in header `<xinfer/tensor.hpp>`  
Namespace: `xinfer`

`Tensor` represents a multi-dimensional array of elements mapped to physical RAM, device memory, or a Linux kernel `DMA-BUF` file descriptor.

---

## 1. Class Synopsis

```cpp
namespace xinfer {

struct TensorDescriptor {
    std::string name{};
    std::vector<int64_t> dimensions{};
    Precision precision{Precision::FP32};
    MemoryType memory_type{MemoryType::HOST_PINNED};
    std::vector<size_t> strides{};
};

class XINFER_API Tensor {
public:
    // Factory Methods
    static std::shared_ptr<Tensor> create_owning(const TensorDescriptor& desc);
    static std::shared_ptr<Tensor> create_from_raw_host(
        const TensorDescriptor& desc, 
        void* ptr, 
        size_t size_bytes
    );
    static std::shared_ptr<Tensor> create_from_dmabuf(
        const TensorDescriptor& desc, 
        int dma_fd, 
        size_t size_bytes
    );
    static std::shared_ptr<Tensor> create_from_physical_address(
        const TensorDescriptor& desc, 
        void* virt_ptr, 
        uint64_t phys_addr, 
        size_t size_bytes
    );

    ~Tensor();

    // Data Accessors
    template <typename T>
    [[nodiscard]] T* data() noexcept;

    template <typename T>
    [[nodiscard]] const T* data() const noexcept;

    template <typename T>
    [[nodiscard]] std::span<T> as_span() noexcept;

    // Buffer Introspection
    [[nodiscard]] const TensorDescriptor& descriptor() const noexcept;
    [[nodiscard]] size_t size_bytes() const noexcept;
    [[nodiscard]] size_t element_count() const noexcept;
    [[nodiscard]] MemoryType get_memory_type() const noexcept;
    [[nodiscard]] int get_dma_fd() const noexcept;
    [[nodiscard]] void* get_raw_pointer() noexcept;

    // In-Place Operations
    void copy_from_host(const void* src, size_t bytes);
    void flush_cache();
    void invalidate_cache();

private:
    Tensor(const TensorDescriptor& desc, void* ptr, int dma_fd, bool owning);
};

} // namespace xinfer
```

---

## 2. Factory Methods

### `create_from_dmabuf`
```cpp
static std::shared_ptr<Tensor> create_from_dmabuf(
    const TensorDescriptor& desc, 
    int dma_fd, 
    size_t size_bytes
);
```
Constructs a non-owning tensor wrapping an open Linux `DMA-BUF` file descriptor. The tensor does not duplicate memory pages; it references the underlying physical frames directly.

---

### `create_owning`
```cpp
static std::shared_ptr<Tensor> create_owning(const TensorDescriptor& desc);
```
Allocates a 64-byte aligned, page-locked (`mlock`) memory buffer corresponding to the shape and precision declared in `desc`.
```

---

### File: `xinfer-essential/docs/api-reference/model-hub.md`

```markdown
# Class `xinfer::ModelHub`

Defined in header `<xinfer/model_hub.hpp>`  
Namespace: `xinfer`

`ModelHub` handles model artifact resolution, multi-tier caching (RAM $\to$ NVMe $\to$ HTTPS), cryptographic checksum verification, and air-gapped isolation.

---

## 1. Class Synopsis

```cpp
namespace xinfer {

struct ModelArtifact {
    std::filesystem::path local_path;
    std::string sha256_hash;
    std::string format;
    size_t size_bytes{0};
    std::shared_ptr<const std::vector<uint8_t>> in_memory_buffer{nullptr};
};

class XINFER_API ModelHub {
public:
    explicit ModelHub(
        std::filesystem::path cache_directory = "/var/cache/xinfer/models", 
        bool offline_mode = false
    );
    ~ModelHub();

    [[nodiscard]] ModelArtifact resolve(
        std::string_view model_identifier,
        std::string_view expected_sha256,
        std::string_view remote_uri = ""
    );

    void pin_to_memory(std::string_view model_identifier);
    void unpin_from_memory(std::string_view model_identifier) noexcept;
    void prune_disk_cache(size_t max_bytes_allowed);
    
    [[nodiscard]] bool is_model_cached(
        std::string_view model_identifier, 
        std::string_view expected_sha256
    ) const noexcept;
    
    void clear_all() noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace xinfer
```

---

## 2. Key Method Documentation

### `resolve`
```cpp
ModelArtifact resolve(
    std::string_view model_identifier,
    std::string_view expected_sha256,
    std::string_view remote_uri = ""
);
```
Searches the active RAM pool and persistent disk cache for a model matching `model_identifier` and `expected_sha256`. If missing and `offline_mode == false`, pulls the model via HTTPS. Throws `InferenceException(ErrorCode::ERR_INTEGRITY_CHECK_FAILED)` if the calculated hash does not match `expected_sha256`.
```

---

### File: `xinfer-essential/docs/api-reference/plugin-manager.md`

```markdown
# Class `xinfer::PluginManager`

Defined in header `<xinfer/plugin_manager.hpp>`  
Namespace: `xinfer`

`PluginManager` manages dynamic hardware backend plugins, handles runtime `dlopen` loading with symbol encapsulation, and verifies ABI contracts.

---

## 1. Class Synopsis

```cpp
namespace xinfer {

class XINFER_API PluginManager {
public:
    static PluginManager& instance() noexcept;

    void set_search_path(const std::filesystem::path& path);
    void register_plugin_directory(const std::filesystem::path& dir);

    [[nodiscard]] std::shared_ptr<IInferencePlugin> load_plugin(BackendType backend);
    [[nodiscard]] std::shared_ptr<IInferencePlugin> load_plugin_from_file(
        const std::filesystem::path& shared_lib_path
    );

    [[nodiscard]] std::vector<BackendType> get_available_backends() const;
    [[nodiscard]] bool is_backend_available(BackendType backend) const noexcept;
    
    void unload_all() noexcept;

private:
    PluginManager();
    ~PluginManager();
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace xinfer
```

---

## 2. Architectural Usage

`PluginManager` is implemented as a thread-safe singleton. When `InferenceEngine::initialize()` executes, it queries `PluginManager::instance().load_plugin(config.backend)` to bind hardware drivers.
```

---

### File: `xinfer-essential/docs/api-reference/engine-config.md`

```markdown
# Struct `xinfer::EngineConfig`

Defined in header `<xinfer/engine_config.hpp>`  
Namespace: `xinfer`

`EngineConfig` defines the initialization parameters passed to `InferenceEngine`.

---

## 1. Structure Definition

```cpp
namespace xinfer {

struct EngineConfig {
    // Model Target
    std::filesystem::path model_path{};
    std::string expected_sha256{};

    // Accelerator Target
    BackendType backend{BackendType::AUTO};
    Precision precision{Precision::FP16};
    int32_t device_id{0};

    // Low-Level Optimization Controls
    bool enable_zero_copy{true};
    bool enable_offline_mode{false};
    size_t num_worker_threads{1};

    // Paths & Custom Options
    std::filesystem::path plugin_search_path{"/usr/local/lib/xinfer-plugins"};
    std::string custom_options{};

    // Serialization
    [[nodiscard]] std::string to_json() const;
    static EngineConfig from_json(std::string_view json_str);
};

} // namespace xinfer
```

---

## 2. Field Specifications

| Field | Default Value | Description |
| :--- | :--- | :--- |
| `model_path` | `""` | Absolute or relative path to the local model artifact. |
| `expected_sha256` | `""` | Expected 64-character hex SHA-256 digest. If empty, hash checks are skipped. |
| `backend` | `BackendType::AUTO` | Target hardware architecture. `AUTO` selects the fastest available backend. |
| `precision` | `Precision::FP16` | Compute precision. Supported options: `FP32`, `FP16`, `BF16`, `INT8`. |
| `device_id` | `0` | Physical device ordinal (e.g., GPU 0 or NPU core 0). |
| `enable_zero_copy` | `true` | Bypasses intermediate copies using DMA-BUF and pinned allocations. |
| `enable_offline_mode`| `false`| Disables network resolution; operates exclusively on local files. |
| `num_worker_threads` | `1` | Pre-allocates execution contexts for parallel workers. |
```

---

### File: `xinfer-essential/docs/api-reference/data-types.md`

```markdown
# Data Types, Precision & Enums

Defined in header `<xinfer/data_types.hpp>`  
Namespace: `xinfer`

This header defines the core enumerations and string conversion routines used across `xinfer-essential`.

---

## 1. Enums

### `BackendType`
Specifies the hardware target backend:
```cpp
enum class BackendType : uint32_t {
    AUTO = 0,
    CPU_REFERENCE,
    TENSORRT,
    OPENVINO,
    RKNN,
    QUALCOMM_QNN,
    AMD_VITIS_AI,
    APPLE_COREML,
    AMD_RYZEN_AI,
    MEDIATEK_NEUROPILOT,
    HAILO_HAILORT,
    AMBARELLA_CVFLOW,
    SAMSUNG_ENN,
    GOOGLE_CORAL,
    INTEL_FPGA_AI_SUITE,
    MICROCHIP_VECTORBLOX,
    LATTICE_SENSAI,
    CUSTOM = 255
};
```

### `Precision`
Specifies the floating-point or integer compute precision:
```cpp
enum class Precision : uint8_t {
    FP32,
    FP16,
    BF16,
    INT8,
    INT4,
    BINARY
};
```

### `MemoryType`
Specifies the underlying memory domain:
```cpp
enum class MemoryType : uint8_t {
    HOST,
    HOST_PINNED,
    DEVICE,
    DMA_BUF,
    UNIFIED
};
```

### `DataType`
Specifies tensor element types:
```cpp
enum class DataType : uint8_t {
    FLOAT32,
    FLOAT16,
    BFLOAT16,
    INT8,
    UINT8,
    INT16,
    INT32,
    INT64,
    BOOL,
    CUSTOM
};
```

---

## 2. Utility Functions

```cpp
[[nodiscard]] constexpr std::string_view to_string(BackendType backend) noexcept;
[[nodiscard]] constexpr std::string_view to_string(Precision precision) noexcept;
[[nodiscard]] constexpr std::string_view to_string(MemoryType memory_type) noexcept;
[[nodiscard]] constexpr std::string_view to_string(DataType data_type) noexcept;
[[nodiscard]] constexpr size_t get_element_size(DataType data_type) noexcept;
```
```

---

### File: `xinfer-essential/docs/api-reference/error-handling.md`

```markdown
# Class `xinfer::InferenceException` & Error Handling

Defined in header `<xinfer/exception.hpp>`  
Namespace: `xinfer`

`xinfer-essential` handles errors through standard C++20 exceptions derived from `std::exception` alongside structured error codes.

---

## 1. Class Synopsis

```cpp
namespace xinfer {

enum class ErrorCode : int32_t {
    SUCCESS = 0,
    ERR_ENGINE_NOT_INITIALIZED = -1,
    ERR_ENGINE_BUSY = -2,
    ERR_DEVICE_NOT_FOUND = -3,
    ERR_OUT_OF_MEMORY = -4,
    ERR_MEMORY_PIN_FAILED = -5,
    ERR_FILE_NOT_FOUND = -6,
    ERR_INTEGRITY_CHECK_FAILED = -7,
    ERR_PLUGIN_LOAD_FAILED = -8,
    ERR_ABI_MISMATCH = -9,
    ERR_MODEL_DESERIALIZATION_FAILED = -10,
    ERR_INCOMPATIBLE_MEMORY_DOMAINS = -11,
    ERR_TIMEOUT = -12,
    ERR_REMOTE_SYNC_FAILED = -13,
    ERR_INTERNAL_EXCEPTION = -99
};

class XINFER_API InferenceException : public std::exception {
public:
    explicit InferenceException(ErrorCode code, std::string message, int system_code = 0) noexcept;
    ~InferenceException() override = default;

    [[nodiscard]] const char* what() const noexcept override;
    [[nodiscard]] ErrorCode error_code() const noexcept;
    [[nodiscard]] int system_code() const noexcept;

private:
    ErrorCode code_;
    std::string message_;
    int system_code_{0};
};

} // namespace xinfer
```

---

## 2. Idiomatic Error Handling Example

```cpp
#include <xinfer/xinfer.hpp>
#include <iostream>

int main() {
    try {
        xinfer::EngineConfig config;
        config.model_path = "/opt/models/network_threat_v2.onnx";
        config.expected_sha256 = "e9a2c31e...";

        xinfer::InferenceEngine engine(config);
        engine.initialize();
        engine.forward();

    } catch (const xinfer::InferenceException& ex) {
        std::cerr << "[xInfer Exception Caught]\n"
                  << "  Code       : " << static_cast<int>(ex.error_code()) << "\n"
                  << "  Description: " << ex.what() << "\n"
                  << "  System Code: " << ex.system_code() << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Standard Exception: " << ex.what() << std::endl;
        return 2;
    }

    return 0;
}
```
```

---

### Complete in Part 10
- `xinfer-essential/docs/api-reference/index.md`
- `xinfer-essential/docs/api-reference/inference-engine.md`
- `xinfer-essential/docs/api-reference/tensor.md`
- `xinfer-essential/docs/api-reference/model-hub.md`
- `xinfer-essential/docs/api-reference/plugin-manager.md`
- `xinfer-essential/docs/api-reference/engine-config.md`
- `xinfer-essential/docs/api-reference/data-types.md`
- `xinfer-essential/docs/api-reference/error-handling.md`

All 8 API reference files are now generated.

---

### Files to be Generated in Part 11

The next phase covers practical, end-to-end **Tutorials** (`tutorials/`):

1. `tutorials/netflow-threat-autoencoder.md` (1D tabular vector scoring in under 12 microseconds)
2. `tutorials/realtime-yolo-edge-vision.md` (30 FPS camera inference on Rockchip RK3588 & Jetson)
3. `tutorials/scada-modbus-anomaly-detection.md` (Inline industrial control frame inspection)
4. `tutorials/thermal-overheating-detector.md` (Real-time industrial physical equipment monitoring)
5. `tutorials/dual-model-shadow-execution.md` (Running production and candidate models in parallel)

Let me know when you are ready to proceed with Part 11.
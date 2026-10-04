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
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
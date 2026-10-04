---

### File: `xinfer-essential/docs/model-hub/supported-model-formats.md`

```markdown
# Supported Model Serialization Formats

`xinfer-essential` accepts a wide range of model formats, mapping each directly to its optimal silicon execution backend.

---

## 1. Supported Formats Matrix

| Model Format | Standard Extension | Originating Framework | Target Silicon Backend | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **ONNX** | `.onnx` | PyTorch, JAX, TensorFlow | OpenVINO, TensorRT, CPU Fallback | Supported Opset versions: **11 through 17**. |
| **TensorRT Engine** | `.engine` / `.plan` | NVIDIA TensorRT Builder | `nvidia_tensorrt` | Serialized hardware-specific compute plan. |
| **OpenVINO IR** | `.xml` & `.bin` | Intel OpenVINO Model Optimizer | `intel_openvino` | Two-file format: XML graph topology + binary weights. |
| **Rockchip RKNN** | `.rknn` | RKNN-Toolkit2 | `rockchip_rknn` | Pre-compiled binary for RK3588/RK3576 NPU cores. |
| **Qualcomm Context** | `.bin` | Qualcomm QNN SDK | `qualcomm_qnn` | Pre-compiled HTP/DSP execution context. |
| **Hailo Executable** | `.hef` | Hailo Dataflow Compiler | `hailo_hailort` | Compiled dataflow graph targeting Hailo-8. |
| **Apple CoreML** | `.mlmodelc` | CoreML Tools (`coremltools`) | `apple_coreml` | Compiled bundle targeting the Apple Neural Engine. |
| **AMD Vitis Model** | `.xmodel` | Vitis AI Quantizer | `amd_vitis_ai` | DPU instruction stream for Xilinx/AMD FPGAs. |
| **Ambarella Cavalry**| `.cavalry` | Ambarella Toolchain | `ambarella_cvflow` | Binary partition targeting CVFlow Vector Processors. |
| **Samsung ENN** | `.nnc` | Samsung ENN Studio | `samsung_enn` | NNC binary bundle targeting Exynos NPUs. |
| **Edge TPU TFLite** | `.tflite` | Google Coral Edge TPU Compiler | `google_coral` | 8-bit quantized TensorFlow Lite container. |
| **VectorBlox Blob** | `.blob` | Microchip CoreVectorBlox SDK | `microchip_vectorblox`| Binary targeting PolarFire SoC FPGA fabric. |

---

## 2. ONNX Support Details (Opset 11–17)

For ONNX graphs, `xinfer-essential` supports both dense floating-point models and quantized networks:

* **Supported Precisions:** `FP32`, `FP16`, `BF16`, `INT8` (QuantizeLinear / DequantizeLinear operations).
* **Dynamic Dimensions:** Input graphs can declare dynamic batch dimensions (`[-1, 32]`) or dynamic sequence lengths.
* **Metadata Extraction:** `InferenceEngine` reads input tensor names, target output shapes, and embedded author metadata directly from the ONNX graph header on initialization:

```cpp
#include <xinfer/xinfer.hpp>
#include <iostream>

void inspect_model_metadata(xinfer::InferenceEngine& engine) {
    for (size_t i = 0; i < engine.get_input_count(); ++i) {
        auto desc = engine.get_input_descriptor(i);
        std::cout << "Input [" << i << "]: " << desc.name 
                  << " | DataType: " << xinfer::to_string(desc.precision)
                  << " | Shape: [";
        for (auto dim : desc.dimensions) {
            std::cout << dim << " ";
        }
        std::cout << "]" << std::endl;
    }
}
```
```

---

### Complete in Part 9
- `xinfer-essential/docs/model-hub/modelhub-architecture.md`
- `xinfer-essential/docs/model-hub/https-caching-rules.md`
- `xinfer-essential/docs/model-hub/cryptographic-verification.md`
- `xinfer-essential/docs/model-hub/air-gapped-offline-mode.md`
- `xinfer-essential/docs/model-hub/supported-model-formats.md`

All 5 Model Hub documentation files are now generated.

---

### Files to be Generated in Part 10

The next phase covers the complete **C++20 API Reference** (`api-reference/`):

1. `api-reference/index.md` (Namespace overview & architectural invariants)
2. `api-reference/inference-engine.md` (Class `xinfer::InferenceEngine`)
3. `api-reference/tensor.md` (Class `xinfer::Tensor` & `TensorDescriptor`)
4. `api-reference/model-hub.md` (Class `xinfer::ModelHub`)
5. `api-reference/plugin-manager.md` (Class `xinfer::PluginManager`)
6. `api-reference/engine-config.md` (Struct `xinfer::EngineConfig`)
7. `api-reference/data-types.md` (Enums `DataType`, `Precision`, `BackendType`, `MemoryType`)
8. `api-reference/error-handling.md` (Class `xinfer::InferenceException` & `ErrorCode`)

Let me know when you are ready to proceed with Part 10.
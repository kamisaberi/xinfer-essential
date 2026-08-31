Here is a complete, detailed, and comprehensive `README.md` for **xInfer Essential**, written professionally without icons or emojis.

***

# xInfer Essential: High-Performance C++20 Universal AI Runtime

xInfer Essential is a lightweight, zero-copy C++20 deep learning inference runtime designed for heterogeneous edge accelerators, embedded SoCs, and data center GPUs. Built as a high-performance C++ shared library (`libxinfer.so`), it abstracts vendor-specific neural processing SDKs behind a unified, low-overhead C++ API. 

xInfer Essential features native support for 15 hardware platforms, an automatic HTTPS ONNX Model Hub, zero-copy DMA memory management, and a dynamic C++ runtime plugin architecture.

---

## Core Architectural Features

- 15-Platform Universal Hardware Abstraction: Write application logic once in C++20 and execute models across NVIDIA, Intel, Rockchip, Qualcomm, Apple, AMD, MediaTek, Hailo, Google, Ambarella, Samsung, and FPGA accelerators.
- Zero-Copy Memory Pipeline: Native support for Linux DMA-BUF, NVIDIA NVMM, Host Pinned Memory, and CUDA/NPU Unified Memory to eliminate CPU-to-Accelerator copy bottlenecks.
- Automatic HTTPS Model Hub (`xinfer::ModelHub`): Automatically fetches and caches ONNX models from Hugging Face Hub, the ONNX Model Zoo, GitHub Releases, or custom HTTPS URLs when requested.
- Dynamic C++ Runtime Plugin Architecture (`xinfer::plugin::PluginManager`): Supports dynamic loading of compiled `.so` plugins at runtime via Linux `dlopen()` / `dlsym()` for custom model parsers, AES-256 weight decryption, hardware video decoding, and post-processing kernels.
- Integrated SIMD & CUDA Acceleration: Built-in CUDA and SIMD kernels for low-latency image letterboxing, RGB/BGR normalization, and YOLO Non-Maximum Suppression (NMS) bounding box decoding.
- Minimal Footprint: Compact binary footprint under 20MB with zero heavy external dependencies (no Python or heavy RPC frameworks required).

---

## Supported Hardware Matrix (15 Platforms)

| Target Enum | Hardware Platform / Vendor | Vendor SDK | Native Engine Format | Application Domain |
| :--- | :--- | :--- | :--- | :--- |
| `Target::TensorRT` | NVIDIA (GeForce RTX, Jetson Orin / Xavier) | TensorRT 10.x / CUDA | `.engine` / `.plan` | High-Perf Vision, LLMs |
| `Target::OpenVINO` | Intel (Core Ultra NPU, Xeon, Arc GPU, x86 CPU) | OpenVINO Runtime | `.xml` / `.bin` | Edge Servers, Laptops |
| `Target::RKNN` | Rockchip (RK3588, RK3568, RV1126 ARM SoCs) | RKNPU2 SDK | `.rknn` | Smart Cameras, NVRs |
| `Target::VitisAI` | AMD / Xilinx (Kria SOM, Zynq MPSoC, Versal) | Vitis AI (DPU) | `.xmodel` | Robotics, Defense |
| `Target::QNN` | Qualcomm (Snapdragon 8 Gen 2/3, RB5 HTP) | Qualcomm QNN | `.bin` / `.so` | Mobile, Drones, AR/VR |
| `Target::CoreML` | Apple (M-Series, A-Series Chips) | CoreML / Metal | `.mlmodelc` | macOS / iOS Apps |
| `Target::RyzenAI` | AMD (Ryzen 7040/8040 Series IPU) | Ryzen AI NPU SDK | `.onnx` | AI PCs |
| `Target::NeuroPilot` | MediaTek (Genio 1200, Dimensity) | NeuroPilot DLA | `.dla` / `.pte` | IoT / Smart Home |
| `Target::Hailo` | Hailo (Hailo-8 / Hailo-8L / Hailo-10) | HailoRT | `.hef` | Raspberry Pi 5, Edge Boxes |
| `Target::CVFlow` | Ambarella (CV2, CV3, CV5 SoCs) | CVFlow Cavalry | `.cavalry` | Automotive, Security |
| `Target::ENN` | Samsung (Exynos 2200/2400 Eden NPU) | Samsung ENN | `.nnc` | Mobile, Automotive Cockpits |
| `Target::EdgeTPU` | Google Coral (USB, M.2, Dev Board) | Edge TPU Runtime | `.tflite` | Low-Power IoT |
| `Target::FPGA_AI` | Intel FPGA (Agilex, Stratix 10) | FPGA AI Suite | `.aocx` | Radiation-Hardened / Defense |
| `Target::VectorBlox` | Microchip (PolarFire FPGA) | VectorBlox SDK | `.blob` | Low-Power Edge FPGAs |
| `Target::SensAI` | Lattice (CrossLink-NX) | sensAI Engine | `.bin` | TinyML, Wearables |

---

## 30-Plugin Runtime Catalog

xInfer Essential includes a dynamic C++ plugin architecture (`IInferencePlugin`) with 30 pre-built plugins:

| Rank | Plugin Name | Category | Primary Function |
| :---: | :--- | :--- | :--- |
| **1** | `NVIDIA_NVDEC_Video_Decoder` | Preprocessor | Decodes RTSP H.264/H.265 video directly into GPU VRAM (zero-copy CPU bypass). |
| **2** | `AES256_Model_Decryptor` | Security & IP | Decrypts encrypted ONNX/TensorRT model weights directly in RAM before GPU/NPU loading. |
| **3** | `YOLO_NMS_Postprocessor` | Postprocessor | Runs Non-Maximum Suppression (NMS) and bounding box decoding on GPU / ARM NEON. |
| **4** | `Rockchip_RGA_2D_Resizer` | Preprocessor | Scales, letterboxes, and normalizes images using Rockchip RGA 2D hardware. |
| **5** | `Linux_DMA_BUF_Allocator` | Memory Allocator | Enables zero-copy Direct Memory Access between camera V4L2 drivers, GPUs, and NPUs. |
| **6** | `PyTorch_Native_Parser` | Model Parser | Parses PyTorch `.pt` and Hugging Face `.safetensors` files directly. |
| **7** | `GGUF_Quantized_LLM_Parser` | Model Parser | Parses GGUF quantized model files for local LLM text generation. |
| **8** | `Audio_MelSpectrogram_Preprocessor` | Preprocessor | Converts raw WAV/PCM audio streams into Mel-Spectrogram tensors on GPU. |
| **9** | `PagedAttention_KVCache_Manager` | LLM Memory | Manages KV-cache memory blocks efficiently for local LLM context windows. |
| **10** | `Dynamic_Quantizer_Engine` | Quantization | Quantizes model weights dynamically on load (Float32 to FP16/INT8). |
| **11** | `TPM2_Hardware_Key_Exchange` | Security & IP | Exchanges model decryption keys directly with physical motherboard TPM 2.0 chips. |
| **12** | `NVIDIA_NVMM_ZeroCopy_Allocator` | Memory Allocator | Wraps NVIDIA Jetson `NvBuffer` memory pointers directly into `xinfer::Tensor`. |
| **13** | `TFLite_EdgeTPU_Parser` | Model Parser | Parses and executes Google Coral Edge TPU `.tflite` model files natively. |
| **14** | `ARM_NEON_SIMD_Preprocessor` | Preprocessor | Uses ARM NEON 128-bit vector instructions for fast image normalization on ARM. |
| **15** | `Microsecond_Telemetry_Profiler` | Telemetry | Measures kernel execution microsecond latencies and board wattage via NVML / Level Zero. |
| **16** | `Continuous_Batching_Queue` | LLM Memory | Dynamically batches concurrent text generation requests for local LLM servers. |
| **17** | `Custom_CUDA_Operator_Layer` | Custom Operator | Allows writing custom CUDA C++ kernels for layers missing in standard ONNX. |
| **18** | `Optical_Flow_Motion_Accelerator` | Preprocessor | Calculates dense optical flow motion vectors on GPU before passing frames to tracking AI. |
| **19** | `AMP_Fallback_Engine` | Quantization | Automatically falls back from FP16 to FP32 if numerical overflow (`NaN`/`Inf`) occurs. |
| **20** | `Intel_OneVPL_Video_Decoder` | Preprocessor | Uses Intel QuickSync / VA-API hardware video decoders on Intel CPUs/GPUs. |
| **21** | `CoreML_Apple_Parser` | Model Parser | Executes Apple CoreML model packages directly on Apple Silicon Neural Engines. |
| **22** | `Softmax_TopK_Sampling_Decoder` | Postprocessor | Runs Softmax and Top-K / Top-P temperature sampling on LLM probability tensors. |
| **23** | `POSIX_Shared_Memory_Allocator` | Memory Allocator | Shares tensor buffers across distinct Linux processes via POSIX shared memory (`/dev/shm`). |
| **24** | `Thermal_Frequency_Governor` | Telemetry | Monitors SoC temperature; dynamically throttles FPS/batch size to prevent overheating. |
| **25** | `PointCloud_LiDAR_Voxelizer` | Preprocessor | Converts raw 3D LiDAR point clouds into 3D voxel grid tensors for 3D AI perception. |
| **26** | `Segmentation_Mask_Polygon_Extractor` | Postprocessor | Converts raw 2D segmentation probability masks into vector contour polygons on GPU. |
| **27** | `Speculative_Decoding_Verifier` | LLM Memory | Verifies draft model tokens in parallel on primary LLM for 2x faster text generation. |
| **28** | `Watermark_Model_Verifier` | Security & IP | Verifies embedded digital steganography watermarks in model weights before execution. |
| **29** | `Multi_NPU_Model_Sharder` | Multi-Device | Splits large neural networks across multiple NPU cores (e.g., RK3588 NPU cores) in parallel. |
| **30** | `Vulkan_Memory_Allocator_Plugin` | Memory Allocator | Universal GPU buffer allocator working across AMD, NVIDIA, Intel, and ARM Mali GPUs. |

---

## Repository File Structure

```text
xinfer-essential/
├── CMakeLists.txt                    # Primary CMake build configuration (libxinfer.so)
├── LICENSE                           # MIT License
├── README.md                         # Detailed project documentation
├── install.sh                        # Multi-platform dependency installer script
│
├── include/
│   └── xinfer/                       # Public C++20 API Headers
│       ├── xinfer.hpp                # Master single-line include header
│       ├── backend.hpp               # Abstract C++ hardware driver interface
│       ├── engine.hpp                # High-level model loader & execution manager
│       ├── export.hpp                # Symbol visibility export macros (XINFER_API)
│       ├── hub.hpp                   # Automatic HTTPS ONNX Model Hub header
│       ├── memory.hpp                # Zero-copy DMA / Pinned memory allocators
│       ├── plugin.hpp                # Dynamic C++ plugin interface (IInferencePlugin)
│       ├── plugin_manager.hpp        # Dynamic .so plugin loader (dlopen / dlsym)
│       ├── target.hpp                # Hardware Target enums (15 targets) & DataType math
│       ├── tensor.hpp                # Hardware-agnostic C++20 Tensor class
│       │
│       └── utils/
│           ├── postproc.hpp          # NMS & Softmax decoding header
│           └── preproc.hpp           # Image scaling, normalization, & letterbox header
│
├── src/
│   ├── core/                         # Core Runtime Implementations
│   │   ├── engine.cpp                # Model loading, execution, & backend dispatcher
│   │   ├── hub.cpp                   # ModelHub HTTPS downloader & cache manager
│   │   ├── memory.cpp                # DMA buffer & CUDA/Host memory allocator
│   │   ├── plugin_manager.cpp        # Dynamic C++ plugin loader implementation
│   │   └── tensor.cpp                # Tensor allocation, reshape, & stride math
│   │
│   ├── backends/                     # Hardware Driver Implementations (15 Targets)
│   │   ├── tensorrt/                 # NVIDIA TensorRT backend driver (.engine)
│   │   ├── openvino/                 # Intel OpenVINO backend driver (.xml/.bin)
│   │   ├── rknn/                     # Rockchip RKNN backend driver (.rknn)
│   │   ├── vitis_ai/                 # AMD / Xilinx Vitis AI backend driver (.xmodel)
│   │   ├── qnn/                      # Qualcomm QNN backend driver (.bin/.so)
│   │   ├── coreml/                   # Apple CoreML / Metal backend driver (.mlmodelc)
│   │   ├── ryzen_ai/                 # AMD Ryzen AI backend driver (.onnx)
│   │   ├── neuropilot/               # MediaTek NeuroPilot backend driver (.dla/.pte)
│   │   ├── hailo/                    # Hailo HailoRT backend driver (.hef)
│   │   ├── cvflow/                   # Ambarella CVFlow backend driver (.cavalry)
│   │   ├── enn/                      # Samsung ENN backend driver (.nnc)
│   │   ├── edge_tpu/                 # Google Coral Edge TPU backend driver (.tflite)
│   │   ├── fpga_ai/                  # Intel FPGA AI Suite backend driver (.aocx)
│   │   ├── vectorblox/               # Microchip VectorBlox backend driver (.blob)
│   │   └── sensai/                   # Lattice sensAI backend driver (.bin)
│   │
│   ├── plugins/                      # 30 Dynamic Plugin C++ Source Implementations
│   │   ├── model_decryptor_plugin.cpp    # AES-256 model weight decryptor plugin
│   │   ├── yolo_nms_plugin.cpp           # CUDA/SIMD YOLO NMS postprocessor plugin
│   │   ├── audio_melspectrogram_plugin.cpp # Audio Mel-Spectrogram preprocessor plugin
│   │   ├── dynamic_quantizer_plugin.cpp   # Dynamic INT8/FP16 quantizer plugin
│   │   ├── telemetry_profiler_plugin.cpp  # Microsecond latency & telemetry profiler
│   │   ├── softmax_topk_plugin.cpp        # Softmax & Top-K LLM decoder plugin
│   │   ├── rga_resizer_plugin.cpp         # Rockchip RGA 2D resizer plugin
│   │   ├── dmabuf_allocator_plugin.cpp    # Linux DMA-BUF zero-copy allocator
│   │   ├── tpm2_key_exchange_plugin.cpp   # TPM 2.0 hardware key exchange plugin
│   │   ├── optical_flow_plugin.cpp        # Optical flow motion vector accelerator
│   │   ├── paged_attention_plugin.cpp     # PagedAttention KV-Cache manager
│   │   ├── eta_packet_plugin.cpp          # Encrypted traffic analysis (ETA) plugin
│   │   ├── amp_fallback_plugin.cpp        # Automatic mixed-precision fallback
│   │   ├── lidar_voxelizer_plugin.cpp     # PointCloud 3D LiDAR voxelizer
│   │   └── ...                           # (Remaining 16 C++ plugin source files)
│   │
│   └── utils/                        # Pre/Post Processing Implementations
│       ├── postproc.cpp              # CPU/SIMD fallback NMS & box decoder
│       ├── preproc.cpp               # CPU/SIMD fallback image scaling kernel
│       └── cuda_preproc.cu           # CUDA GPU image resize & normalization kernel
│
├── tests/                            # Unit Testing & Benchmarking Suite
│   ├── CMakeLists.txt                # Tests build script
│   ├── test_tensor.cpp               # Tensor memory & stride math unit test
│   ├── test_memory.cpp               # Zero-copy DMA buffer test
│   ├── test_backends.cpp             # Backend loader & execution unit test
│   └── benchmark.cpp                 # Microsecond latency & FPS benchmarker
│
└── examples/                         # Application Production Examples
    ├── 01_tensorrt_yolo.cpp          # Real-time zero-copy YOLO detection on NVIDIA GPU
    ├── 02_openvino_cpu.cpp           # Low-latency inference on Intel x86 CPU/NPU
    ├── 03_rknn_arm.cpp               # Vision inference on Rockchip RK3588 ARM NPU
    ├── 05_huggingface_face_detection.cpp # Auto-download ONNX model from Hugging Face
    └── 06_plugin_demo.cpp            # Dynamic C++ plugin manager demonstration
```

---

## System Requirements & Prerequisites

### Compiler Requirements
- C++20 compliant compiler: GCC 10+, Clang 12+, or MSVC 2019+
- CMake 3.20 or higher
- Linux utilities: `curl` or `wget` (For automatic HTTPS model downloads)

### Hardware SDK Dependencies (Enable based on required backends)
- NVIDIA TensorRT: CUDA Toolkit 12.x and TensorRT 10.x
- Intel OpenVINO: OpenVINO Runtime 2024.0 or higher
- Rockchip RKNN: RKNPU2 Runtime headers and `librknnrt.so`
- Google Coral: `libedgetpu-dev` / `libedgetpu1-max`
- Hailo: `hailort` library

---

## Installation & Build Guide

### 1. Automated Dependency Setup Script

Run the multi-platform installer script on Ubuntu/Debian/ARM to set up dependencies:

```bash
chmod +x install.sh
sudo ./install.sh --all
```

### 2. CMake Build Configuration Options

xInfer Essential uses feature flags in CMake to enable specific hardware backends:

| CMake Option | Description | Default |
| :--- | :--- | :--- |
| `XINFER_ENABLE_TENSORRT` | Compiles NVIDIA TensorRT backend and CUDA kernels | `OFF` |
| `XINFER_ENABLE_OPENVINO` | Compiles Intel OpenVINO backend | `ON` |
| `XINFER_ENABLE_RKNN` | Compiles Rockchip RKNPU2 backend | `OFF` |
| `XINFER_ENABLE_VITIS_AI` | Compiles AMD / Xilinx Vitis AI backend | `OFF` |
| `XINFER_ENABLE_QNN` | Compiles Qualcomm QNN HTP backend | `OFF` |
| `XINFER_ENABLE_HAILO` | Compiles Hailo HailoRT backend | `OFF` |
| `XINFER_ENABLE_EDGE_TPU` | Compiles Google Coral Edge TPU backend | `OFF` |
| `XINFER_BUILD_EXAMPLES` | Compiles standalone application examples | `ON` |
| `XINFER_BUILD_TESTS` | Compiles unit tests and benchmarks | `ON` |

### 3. Build & Install Shared Library (`libxinfer.so`)

```bash
# 1. Create build directory
mkdir -p build && cd build

# 2. Configure CMake (Example: OpenVINO + TensorRT enabled)
cmake .. -DXINFER_ENABLE_OPENVINO=ON -DXINFER_ENABLE_TENSORRT=ON

# 3. Compile Shared Library and 30 Plugins
make -j$(nproc)

# 4. Install Shared Library & Headers Globally
sudo make install
sudo ldconfig
```

---

## C++ Usage Examples

### Example 1: Loading ONNX directly from Hugging Face Hub

```cpp
#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>

int main() {
    try {
        // 1. Initialize Engine for OpenVINO (CPU / Intel NPU)
        xinfer::Engine engine(xinfer::Target::OpenVINO);

        // 2. Load ONNX model directly from Hugging Face Hub URL
        // xinfer::ModelHub automatically downloads the binary file over HTTPS if missing locally
        std::string model_url = "https://github.com/onnx/models/raw/main/validated/vision/body_analysis/ultraface/models/version-RFB-320.onnx";
        
        std::cout << "Loading ONNX model from URL..." << std::endl;
        engine.load_model(model_url);

        // 3. Prepare Input Tensor
        xinfer::Tensor& input = engine.get_input_tensor("input");
        std::vector<float> pixel_data(input.element_count(), 0.5f);
        input.copy_from_host(pixel_data.data(), input.get_size_in_bytes());

        // 4. Run Inference
        engine.infer();

        // 5. Retrieve Output
        xinfer::Tensor& output_scores = engine.get_output_tensor("scores");
        std::cout << "Inference completed! Top confidence score: " << output_scores.data<float>()[1] << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "xInfer Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
```

### Example 2: Loading C++ Dynamic Plugins at Runtime

```cpp
#include <iostream>
#include <xinfer/xinfer.hpp>

int main() {
    xinfer::plugin::PluginManager plugin_mgr;

    // Load AES-256 Model Weight Decryptor Plugin
    if (plugin_mgr.load_plugin("/usr/local/lib/libplugin_model_decryptor.so")) {
        std::cout << "AES-256 Decryptor Plugin active!" << std::endl;
    }

    // Load YOLO NMS Postprocessor Plugin
    if (plugin_mgr.load_plugin("/usr/local/lib/libplugin_yolo_nms.so")) {
        std::cout << "YOLO NMS Postprocessor Plugin active!" << std::endl;
    }

    return 0;
}
```

---

## Performance Benchmarking

A benchmark utility is provided in `tests/benchmark.cpp` to measure microsecond latency and throughput (FPS) across installed hardware backends:

```bash
./build/tests/benchmark --target openvino --model models/version-RFB-320.onnx --iterations 1000
```

### Benchmark Sample Output

```text
==================================================
xInfer Essential Engine Performance Benchmark
Target Hardware : Intel OpenVINO (CPU/NPU)
Model File      : models/version-RFB-320.onnx
Iterations      : 1000 warmup / 1000 test runs
==================================================
Average Latency : 1.12 ms
Minimum Latency : 0.98 ms
Maximum Latency : 1.45 ms
Throughput      : 892.85 FPS
Memory Mode     : Zero-Copy Host Pinned
==================================================
```

---

## License

xInfer Essential is licensed under the MIT License. See `LICENSE` for full license terms.
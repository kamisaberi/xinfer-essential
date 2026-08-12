Here is a complete, detailed, and comprehensive `README.md` for **xInfer Essential**, written professionally without icons or emojis.

***

# xInfer Essential: High-Performance C++20 Edge AI Runtime

xInfer Essential is a lightweight, zero-copy C++20 deep learning inference runtime designed for heterogeneous edge hardware. It abstracts vendor-specific neural processing SDKs behind a unified, high-performance C++ API, enabling developers to write deployment pipelines once and execute them across NVIDIA, Intel, and Rockchip edge platforms with maximum hardware utilization.

---

## Core Features

- Unified C++20 API: Single programming interface across distinct neural network accelerators.
- Zero-Copy Memory Pipelines: Native support for DMA Buffers, Pinned Host Memory, and CUDA/NPU Unified Memory to eliminate CPU-to-Accelerator copy overhead.
- Multi-Backend Architecture: Out-of-the-box support for NVIDIA TensorRT, Intel OpenVINO, and Rockchip RKNN.
- Hardware-Accelerated Preprocessing: Integrated CUDA and SIMD kernels for low-latency image resizing, letterboxing, and normalization directly on accelerator memory.
- Minimal Footprint: Compact binary footprint under 20MB with zero heavy external runtime dependencies (no Python, Docker, or heavy RPC frameworks required).
- Embedded First: Designed specifically for resource-constrained edge devices, air-gapped security appliances, robotics, and industrial IoT systems.

---

## Supported Hardware and Backends

| Target Backend | SDK Version | Target Hardware | Engine File Format |
| :--- | :--- | :--- | :--- |
| NVIDIA TensorRT | TensorRT 10.x / CUDA 12.x | GeForce RTX GPUs, Jetson Orin / Xavier | `.engine` / `.plan` |
| Intel OpenVINO | OpenVINO 2024.x | Intel Core Ultra NPU, Xeon, Arc GPU, x86 CPU | `.xml` / `.bin` |
| Rockchip RKNN | RKNPU2 SDK 2.x | RK3588, RK3568, RV1126 ARM SoCs | `.rknn` |

---

## Repository File Structure

```text
xinfer-essential/
├── CMakeLists.txt                    # Primary CMake build configuration
├── LICENSE                           # MIT License
├── README.md                         # Project documentation
│
├── include/
│   └── xinfer/
│       ├── xinfer.hpp                # Master include header
│       ├── backend.hpp               # Abstract C++ hardware driver interface
│       ├── engine.hpp                # High-level model loader & inference manager
│       ├── memory.hpp                # Zero-copy DMA & Pinned memory allocators
│       ├── target.hpp                # Target enums and runtime configuration
│       ├── tensor.hpp                # Hardware-agnostic C++20 Tensor class
│       │
│       └── utils/
│           ├── postproc.hpp          # NMS & Softmax decoding header
│           └── preproc.hpp           # Image scaling, normalization, & letterbox header
│
├── src/
│   ├── core/                         # Core Runtime Implementations
│   │   ├── engine.cpp                # Model loading, execution, & stream management
│   │   ├── memory.cpp                # Memory allocation & DMA buffer logic
│   │   └── tensor.cpp                # Tensor shape, stride, and buffer math
│   │
│   ├── backends/                     # Core Hardware Drivers
│   │   ├── tensorrt/
│   │   │   ├── tensorrt_backend.hpp  # TensorRT driver wrapper header
│   │   │   ├── tensorrt_backend.cpp  # TensorRT engine loading & execution
│   │   │   └── cuda_kernels.cu       # CUDA memory copy & stream kernels
│   │   ├── openvino/
│   │   │   ├── openvino_backend.hpp  # OpenVINO driver header
│   │   │   └── openvino_backend.cpp  # OpenVINO model compilation & execution
│   │   └── rknn/
│   │       ├── rknn_backend.hpp      # RKNN driver header
│   │       └── rknn_backend.cpp      # RKNN context & zero-copy NPU execution
│   │
│   └── utils/                        # Data Pre/Post Processing Implementations
│       ├── postproc.cpp              # CPU fallback NMS & box decoding
│       ├── preproc.cpp               # CPU/SIMD fallback image scaling kernel
│       └── cuda_preproc.cu           # CUDA GPU image resize & normalization kernel
│
├── tests/                            # Unit Testing & Benchmarking
│   ├── test_tensor.cpp               # Tensor memory & math unit tests
│   ├── test_memory.cpp               # Zero-copy DMA buffer tests
│   ├── test_backends.cpp             # Engine execution unit tests
│   └── benchmark.cpp                 # Microsecond latency and FPS benchmarking
│
└── examples/                         # Standalone Application Examples
    ├── 01_tensorrt_yolo.cpp          # Real-time zero-copy YOLO on NVIDIA GPUs
    ├── 02_openvino_cpu.cpp           # Low-latency inference on Intel CPUs/NPUs
    ├── 03_rknn_arm.cpp               # Vision inference on Rockchip RK3588 NPU
    └── 04_siem_threat_detection.cpp  # Real-time streaming anomaly detection
```

---

## System Requirements and Prerequisites

### Compiler Requirements
- C++20 compliant compiler: GCC 10+, Clang 12+, or MSVC 2019+
- CMake version 3.20 or higher

### Hardware SDK Dependencies (Enable based on required backends)
- NVIDIA TensorRT: CUDA Toolkit 12.x and TensorRT 10.x
- Intel OpenVINO: OpenVINO Runtime 2024.0 or higher
- Rockchip RKNN: RKNPU2 Runtime headers and `librknnrt.so`

---

## Build Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/kamisaberi/xinfer.git
cd xinfer
```

### 2. CMake Build Configuration Options

xInfer Essential uses feature toggles in CMake to compile only the drivers supported by your target hardware:

| CMake Option | Description | Default Value |
| :--- | :--- | :--- |
| `XINFER_ENABLE_TENSORRT` | Compiles NVIDIA TensorRT backend and CUDA kernels | `ON` |
| `XINFER_ENABLE_OPENVINO` | Compiles Intel OpenVINO backend | `ON` |
| `XINFER_ENABLE_RKNN` | Compiles Rockchip RKNPU2 backend | `OFF` |
| `XINFER_BUILD_EXAMPLES` | Compiles standalone application examples | `ON` |
| `XINFER_BUILD_TESTS` | Compiles unit tests and benchmark binaries | `ON` |

### 3. Build Example: NVIDIA TensorRT Only (Host or Jetson)

```bash
mkdir build && cd build
cmake .. -DXINFER_ENABLE_TENSORRT=ON \
         -DXINFER_ENABLE_OPENVINO=OFF \
         -DXINFER_ENABLE_RKNN=OFF
make -j$(nproc)
```

### 4. Build Example: Intel OpenVINO Only (x86 CPU/NPU)

```bash
mkdir build && cd build
cmake .. -DXINFER_ENABLE_TENSORRT=OFF \
         -DXINFER_ENABLE_OPENVINO=ON \
         -DXINFER_ENABLE_RKNN=OFF
make -j$(nproc)
```

### 5. Build Example: Rockchip RKNN (Cross-Compilation for ARM64)

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/aarch64-linux-gnu.cmake \
         -DXINFER_ENABLE_TENSORRT=OFF \
         -DXINFER_ENABLE_OPENVINO=OFF \
         -DXINFER_ENABLE_RKNN=ON
make -j$(nproc)
```

---

## Quickstart API Example

Below is a complete C++20 example demonstrating model loading, zero-copy memory allocation, inference execution, and output parsing using xInfer Essential.

```cpp
#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>

int main() {
    try {
        // 1. Initialize the inference engine for NVIDIA TensorRT
        xinfer::Engine engine(xinfer::Target::TensorRT);

        // 2. Load pre-compiled engine file
        std::cout << "Loading engine model..." << std::endl;
        engine.load_model("yolov8n.engine");

        // 3. Acquire references to input and output tensors
        xinfer::Tensor input_tensor  = engine.get_input_tensor("images");
        xinfer::Tensor output_tensor = engine.get_output_tensor("output0");

        // 4. Allocate zero-copy host-pinned memory buffer for input data
        size_t input_size_bytes = input_tensor.get_size_in_bytes();
        void* host_input_ptr = xinfer::Memory::allocate_pinned(input_size_bytes);

        // Populate host_input_ptr with image pixel data...
        // std::memcpy(host_input_ptr, raw_image_data, input_size_bytes);

        // 5. Copy data to input tensor and execute asynchronous inference
        input_tensor.copy_from_host(host_input_ptr, input_size_bytes);
        
        std::cout << "Executing inference..." << std::endl;
        engine.infer();

        // 6. Access output results directly from output buffer
        float* output_data = output_tensor.data<float>();
        std::cout << "Inference completed successfully. Output tensor shape: " 
                  << output_tensor.shape_string() << std::endl;

        // 7. Cleanup pinned memory
        xinfer::Memory::free_pinned(host_input_ptr);

    } catch (const std::exception& e) {
        std::cerr << "xInfer Runtime Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
```

---

## Memory Management Paradigms

xInfer Essential supports three distinct memory allocation strategies to match target hardware architectures:

1. Standard Memory Allocation: Data is held in standard CPU heap memory and transferred to accelerator device memory prior to inference.
2. Pinned Memory Allocation: Host memory is locked into physical RAM, enabling Direct Memory Access (DMA) hardware transfers without CPU intervention.
3. Zero-Copy / Unified Memory: Used on integrated SoCs (e.g., NVIDIA Jetson, Rockchip RK3588, Intel NPUs) where CPU and Accelerator share physical RAM. The accelerator reads input buffers directly from host memory without memory copy operations.

---

## Performance Benchmarking

A benchmark utility is provided in `tests/benchmark.cpp` to measure inference latency (in microseconds) and throughput (in FPS) for loaded models.

To run the benchmark utility after building:

```bash
./build/tests/benchmark --target tensorrt --model yolov8n.engine --iterations 1000
```

### Example Output

```text
==================================================
xInfer Essential Engine Benchmark
Target Hardware : NVIDIA TensorRT
Model File      : yolov8n.engine
Iterations      : 1000 warmup / 1000 test runs
==================================================
Average Latency : 1.42 ms
Minimum Latency : 1.28 ms
Maximum Latency : 1.89 ms
Throughput      : 704.22 FPS
Memory Mode     : Zero-Copy Host Pinned
==================================================
```

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for full license terms and conditions.
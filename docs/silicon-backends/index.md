### Part 3: Silicon Backends — Primary Edge Accelerators (`silicon-backends/*`)

This section contains the master silicon compatibility matrix and implementation guides for the first four production acceleration backends: **NVIDIA TensorRT**, **Intel OpenVINO**, **Rockchip RKNN**, and **Qualcomm QNN**.

---

### File: `xinfer-essential/docs/silicon-backends/index.md`

```markdown
# Silicon Compatibility Matrix & Hardware Overview

`xinfer-essential` abstracts heterogeneous hardware backends behind the unified `IInferencePlugin` interface. This allows edge security and industrial daemons to run across micro-edge NPUs, embedded SoCs, discrete GPUs, and FPGA coprocessors without code modifications.

---

## 1. Master Compatibility Matrix

| Backend Identifier | Silicon Family | Driver / Subsystem | Supported Model Formats | Zero-Copy Primitives | Typical Inference SLA |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `nvidia_tensorrt` | Jetson Orin Nano/AGX, RTX A4000+, Ada/Blackwell | CUDA 12+, TensorRT 8.6–10.x | `.engine`, `.plan`, `.onnx` | `cudaHostAlloc`, Managed Memory | $< 4.2\,\mu\text{s}$ |
| `intel_openvino` | Core Ultra (Meteor/Lunar Lake), Xeon, Arc | Intel Level Zero, NPU Driver, i915 | OpenVINO IR (`.xml`/`.bin`), `.onnx` | `ov::RemoteTensor`, Host-Pinned | $< 11.4\,\mu\text{s}$ |
| `rockchip_rknn` | RK3588, RK3588S, RK3576 | RKNPU2, Linux DRM / IOCTL | `.rknn` | Linux Kernel `DMA-BUF` | $< 8.9\,\mu\text{s}$ |
| `qualcomm_qnn` | Snapdragon X Elite, SA8295P, 8 Gen 2/3 | Hexagon FastRPC, QNN 2.18+ | QNN Context Binary (`.bin`) | FastRPC ION / DMA-BUF | $< 6.8\,\mu\text{s}$ |
| `amd_vitis_ai` | Versal AI Edge, Kria K26, Zynq UltraScale+ | Xilinx Runtime (XRT), DPU | `.xmodel` | XRT Contiguous Memory (BO) | $< 9.5\,\mu\text{s}$ |
| `apple_coreml` | Apple Silicon (M1, M2, M3, M4 Series) | Metal Performance Shaders, ANE | `.mlmodelc` | Metal Shared Unified Buffers | $< 5.1\,\mu\text{s}$ |
| `amd_ryzen_ai` | Ryzen 7040/8040, Strix Point | AMD XDNA Driver, DirectML/ONNX | `.onnx` | DirectML Shared Committed Memory | $< 12.0\,\mu\text{s}$ |
| `mediatek_neuropilot` | Dimensity 9300, Genio 1200 | APU Direct Driver, NeuroPilot SDK | `.dla`, `.pte` | ION Linux Memory Broker | $< 10.2\,\mu\text{s}$ |
| `hailo_hailort` | Hailo-8, Hailo-8L M.2 Coprocessors | PCIe Driver (`/dev/hailo0`), HailoRT | `.hef` | HailoRT Zero-Copy Host Buffers | $< 7.5\,\mu\text{s}$ |
| `ambarella_cvflow` | CV2x, CV5x, CV7x Edge AI SoCs | Cavalry Driver (`/dev/cavalry`) | `.cavalry` | Cavalry Physical Contiguous RAM | $< 8.2\,\mu\text{s}$ |
| `samsung_enn` | Exynos 990, 2200, 2400 | Exynos Neural Network (ENN) Driver| `.nnc` | ENN Unified Shared Memory | $< 13.5\,\mu\text{s}$ |
| `google_coral` | Edge TPU (PCIe, M.2, USB Accelerator) | `libedgetpu1-std`, Gasket Driver | Edge TPU Compiled `.tflite` | Paged Physical Direct RAM | $< 28.0\,\mu\text{s}$ |
| `intel_fpga_ai` | Agilex 7, Arria 10, Cyclone V | Intel FPGA AI Suite, OpenCL BSP | Compiled Bitstream (`.aocx`) | FPGA PCIe DMA Ring Buffers | $< 6.1\,\mu\text{s}$ |
| `microchip_vectorblox`| PolarFire SoC FPGA | CoreVectorBlox AXI IP Driver | VectorBlox Model (`.blob`) | AXI4 Non-Cached Scratchpad | $< 34.0\,\mu\text{s}$ |
| `lattice_sensai` | iCE40 UltraPlus, CrossLink-NX | Direct SPI / Wishbone Driver | sensAI Binary (`.bin`) | SPI FIFO Double-Buffering | $< 115.0\,\mu\text{s}$ |

---

## 2. Compilation Flags Reference

Enable or disable backend plugins during CMake configuration using explicit compiler flags:

```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DXINFER_ENABLE_TENSORRT=ON \
    -DXINFER_ENABLE_OPENVINO=ON \
    -DXINFER_ENABLE_RKNN=ON \
    -DXINFER_ENABLE_QNN=ON
```

---

## 3. Dynamic Hardware Dispatch & Fallback Order

When `config.backend = xinfer::BackendType::AUTO` is set, `xinfer-essential` discovers physical devices at runtime and selects the fastest backend using this priority order:

```text
 1. Dedicated Hardware Discrete Coprocessor (PCIe / Embedded NPU)
    [TensorRT] ──> [HailoRT] ──> [RKNN] ──> [Qualcomm QNN] ──> [Vitis AI]
                          │
                          ▼ (If coprocessor is unavailable)
 2. Integrated SoC Acceleration Engine
    [Intel OpenVINO NPU] ──> [Apple ANE] ──> [AMD Ryzen AI XDNA]
                          │
                          ▼ (If SoC NPU is uninitialized)
 3. Integrated or Desktop GPU Pipeline
    [Intel OpenVINO iGPU / Level Zero] ──> [CUDA GPU] ──> [Apple Metal]
                          │
                          ▼ (Ultimate deterministic fallback)
 4. Native Reference CPU SIMD Engine
    [AVX-512] ──> [AVX2 / FMA] ──> [ARM Neon] ──> [Standard POSIX C++]
```

If an edge NPU experiences a hardware bus reset or thermal throttling event during initialization, the engine drops to the next available tier without interrupting host daemon execution.
```

---

### File: `xinfer-essential/docs/silicon-backends/nvidia-tensorrt.md`

```markdown
# NVIDIA TensorRT Backend (`libxinfer_tensorrt.so`)

The NVIDIA TensorRT backend provides hardware-accelerated inference across NVIDIA Jetson systems (Orin Nano, Orin NX, AGX Orin) and discrete enterprise GPUs (RTX A4000, L40S, H100). It executes models directly within CUDA asynchronous streams, bypassing host CPU synchronization bottlenecks.

---

## 1. Prerequisites & Host Driver Verification

Ensure the host environment satisfies the driver and toolkit dependencies:

```bash
# Verify NVIDIA kernel driver
nvidia-smi

# Check CUDA Toolkit compiler version (CUDA >= 12.0 required)
nvcc --version
```

Verify that the following shared libraries are discoverable in `LD_LIBRARY_PATH`:
* `libnvinfer.so` (TensorRT Core Engine)
* `libnvinfer_plugin.so` (Custom Plugin Layers)
* `libcudart.so` (CUDA Runtime Library)

---

## 2. Compilation & Plugin Configuration

To compile `libxinfer_tensorrt.so`:

```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DXINFER_ENABLE_TENSORRT=ON \
    -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda \
    -DTENSORRT_ROOT_DIR=/usr/lib/x86_64-linux-gnu
ninja -C build xinfer_tensorrt
```

---

## 3. Runtime Architecture & Execution Pipeline

```text
+─────────────────────────────────────────────────────────────────────────────+
|                          xinfer::InferenceEngine                            |
+─────────────────────────────────────────────────────────────────────────────+
                                       │
                                       ▼
+─────────────────────────────────────────────────────────────────────────────+
|                         TensorRTPlugin Execution                            |
|  - cudaStream_t stream_             - Host-Pinned Input Buffers             |
|  - nvinfer1::IExecutionContext*     - Direct Device VRAM Output             |
+─────────────────────────────────────────────────────────────────────────────+
         │                                                     │
         ▼ (Async Enqueue: enqueueV3)                          ▼ (Zero-Copy)
+─────────────────────────────────────────────────────────────────────────────+
|                              NVIDIA CUDA GPU                                |
|  - Tensor Cores (FP16 / INT8)       - Asynchronous Completion Event Trap   |
+─────────────────────────────────────────────────────────────────────────────+
```

### In-Memory Model Deserialization

`xinfer-essential` loads pre-compiled `.engine` or `.plan` binaries directly from encrypted memory rings without staging temporary unencrypted files to disk:

```cpp
#include <xinfer/xinfer.hpp>
#include <xinfer/backends/tensorrt_config.hpp>

xinfer::EngineConfig config;
config.model_path = "/opt/models/network_threat_v2.engine";
config.backend = xinfer::BackendType::TENSORRT;
config.precision = xinfer::Precision::FP16;
config.device_id = 0; // GPU 0

// Backend-specific configuration options
xinfer::TensorRTOptions trt_opts;
trt_opts.enable_cuda_graphs = true; // Captures CUDA graph for sub-microsecond enqueue
trt_opts.dla_core = -1;              // Run on GPU Tensor Cores (Use 0 or 1 for Jetson DLA)
config.custom_options = trt_opts.serialize();

xinfer::InferenceEngine engine(config);
engine.initialize();
```

---

## 4. Zero-Copy CUDA Memory Mechanisms

To eliminate host-to-device memory copies (`cudaMemcpy`), use pinned host allocations:

```cpp
// Acquire the pre-allocated host-pinned tensor buffer
auto input_tensor = engine.get_input_tensor("network_features");

// input_tensor->data<float>() points to memory allocated via cudaHostAlloc()
float* host_pinned_ptr = input_tensor->data<float>();

// Ingest network packet vector directly into pinned RAM
std::memcpy(host_pinned_ptr, raw_packet_features, 32 * sizeof(float));

// Enqueue inference asynchronously; GPU reads host RAM directly over PCIe bus
engine.forward_async();

// Wait for stream completion only when reading results
engine.synchronize();
```

---

## 5. Performance Tuning & Best Practices

1. **Enable CUDA Graphs:** For fixed-size input tensors (e.g., 32-dim tabular flows), setting `enable_cuda_graphs = true` reduces CPU enqueue overhead from $\sim 8.2\,\mu\text{s}$ down to $< 1.1\,\mu\text{s}$.
2. **Jetson Unified Memory:** On Jetson AGX Orin, avoid discrete VRAM allocations. Use `cudaMallocManaged()` with `cudaMemAdviseSetPreferredLocation` targeting `cudaCpuDeviceId` to allow the GPU Tensor Cores and CPU to share the same physical LPDDR5 bus.
```

---

### File: `xinfer-essential/docs/silicon-backends/intel-openvino.md`

```markdown
# Intel OpenVINO Backend (`libxinfer_openvino.so`)

The Intel OpenVINO backend enables native execution across Intel Core Ultra NPUs (Meteor Lake, Lunar Lake, Arrow Lake), 11th–14th Gen Intel Core processors, Xeon Scalable server CPUs, and Intel Arc discrete GPUs. It interfaces with hardware using the Intel Level Zero compute runtime.

---

## 1. Prerequisites & Driver Verification

Ensure the Intel oneAPI Level Zero driver and OpenVINO runtime are installed:

```bash
# Check Intel NPU driver status
ls -l /dev/accel/accel*

# Verify Level Zero loader discovery
zestat
```

### System Dependencies (Ubuntu 24.04 / 22.04 LTS)

```bash
sudo apt-get install -y \
    intel-openvino-runtime-ubuntu24-2024.1.0 \
    intel-level-zero-gpu \
    level-zero
```

---

## 2. Compilation

Compile the OpenVINO backend plugin:

```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DXINFER_ENABLE_OPENVINO=ON \
    -DOpenVINO_DIR=/opt/intel/openvino_2024/runtime/cmake
ninja -C build xinfer_openvino
```

---

## 3. Direct NPU Initialization & Code Example

```cpp
#include <xinfer/xinfer.hpp>
#include <xinfer/backends/openvino_config.hpp>

xinfer::EngineConfig config;
// Supports both OpenVINO IR (.xml) and direct ONNX files
config.model_path = "/opt/models/network_threat_v2.xml";
config.backend = xinfer::BackendType::OPENVINO;
config.precision = xinfer::Precision::FP16;

// Specify target device: "NPU", "GPU", or "CPU"
xinfer::OpenVINOOptions ov_opts;
ov_opts.target_device = "NPU";
ov_opts.performance_hint = xinfer::OpenVINOPerformanceHint::LATENCY;
ov_opts.num_streams = 1; // Lowest deterministic jitter
config.custom_options = ov_opts.serialize();

xinfer::InferenceEngine engine;
engine.configure(config);
engine.initialize();

std::cout << "[xInfer] Bound to Hardware: " << engine.get_active_backend_name() << std::endl;
```

---

## 4. Zero-Copy Integration via `ov::RemoteTensor`

To achieve sub-microsecond data delivery without copying into `ov::Tensor` objects, `xinfer-essential` maps host-pinned RAM into Level Zero shared virtual allocations:

```text
[Host Pinned RAM (zeDriverAllocHostMem)]
                     │
                     ▼ Direct Pointer Aliasing (Zero-Copy)
[ov::Tensor (Wrapped with external memory pointer)]
                     │
                     ▼
[Intel Core Ultra NPU Compute Engine (via Level Zero direct DMA)]
```

### Implementation

```cpp
// Allocate aligned buffer accessible to the Level Zero runtime
void* aligned_buffer = nullptr;
posix_memalign(&aligned_buffer, 64, tensor_size_bytes);

// Wrap external memory into an xinfer::Tensor view without copying
auto zero_copy_tensor = xinfer::Tensor::create_from_raw_host(
    tensor_desc,
    aligned_buffer,
    tensor_size_bytes
);

engine.bind_input("flow_features", zero_copy_tensor);
engine.forward();
```

---

## 5. Performance Optimization Guidelines

* **NPU Power Profile:** Ensure the system NPU driver is not placed into aggressive power-saving states by adding the kernel argument `intel_vpu.power_profile=1` to `/etc/default/grub`.
* **Precision Selection:** Intel NPUs achieve optimal throughput and lowest latency using `Precision::FP16` or `Precision::INT8`. If `Precision::FP32` is supplied, `libxinfer_openvino.so` automatically inserts transparent precision conversion layers.
```

---

### File: `xinfer-essential/docs/silicon-backends/rockchip-rknn.md`

```markdown
# Rockchip RKNN Backend (`libxinfer_rknn.so`)

The Rockchip RKNN backend provides hardware acceleration for Rockchip SoCs, including the **RK3588, RK3588S, RK3576, and RV1126**. It communicates directly with the multi-core NPU via the Linux DRM (Direct Rendering Manager) driver subsystem and `librknnrt.so`.

---

## 1. Prerequisites & Platform Requirements

Ensure the Rockchip NPU kernel driver is loaded and initialized:

```bash
# Verify NPU device node availability
ls -l /dev/rknpu*

# Verify NPU frequency governor status
cat /sys/class/devfreq/fdab0000.npu/cur_freq
```

Set the NPU governor to maximum performance for deterministic latency:

```bash
echo performance | sudo tee /sys/class/devfreq/fdab0000.npu/governor
```

---

## 2. Compilation

Cross-compile or natively compile `libxinfer_rknn.so` on an `aarch64` Linux target:

```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DXINFER_ENABLE_RKNN=ON \
    -DRKNN_RT_INCLUDE_DIR=/usr/include/rknn \
    -DRKNN_RT_LIB=/usr/lib/librknnrt.so
ninja -C build xinfer_rknn
```

---

## 3. Multi-Core NPU Core Allocation (RK3588)

The RK3588 features three independent NPU cores capable of a collective 6 TOPS of compute. `xinfer-essential` provides explicit core-mask scheduling to support deterministic, multi-threaded pipelines:

```cpp
#include <xinfer/xinfer.hpp>
#include <xinfer/backends/rknn_config.hpp>

xinfer::EngineConfig config;
config.model_path = "/opt/models/network_threat_v2.rknn";
config.backend = xinfer::BackendType::RKNN;

xinfer::RKNNOptions rknn_opts;
// Bind model instance exclusively to Core 0 (Options: CORE_0, CORE_1, CORE_2, CORE_ALL)
rknn_opts.core_mask = xinfer::RKNNCoreMask::CORE_0;
config.custom_options = rknn_opts.serialize();

xinfer::InferenceEngine engine(config);
engine.initialize();
```

---

## 4. Zero-Copy Memory via Linux Kernel `DMA-BUF`

The RKNN backend supports direct hardware execution from Linux `DMA-BUF` file descriptors. This avoids copying data when reading from high-speed network interfaces (AF_XDP) or V4L2 video streams:

```text
[Linux Kernel Network / Camera Driver]
                  │
                  ▼ Emits DMA-BUF File Descriptor (int fd)
+─────────────────────────────────────────────────────────────+
| rknn_create_mem_from_fd()                                   |
|   - Bypasses user-space virtual memory                      |
|   - Maps physical page addresses directly to RK3588 NPU MMU |
+─────────────────────────────────────────────────────────────+
                  │
                  ▼
[Rockchip Tri-Core NPU Compute Engine]
```

### DMA-BUF Zero-Copy Binding Implementation

```cpp
int dma_fd = acquire_kernel_dma_buffer_fd();
size_t buffer_bytes = 512 * 512 * 3;

// Map external DMA-BUF directly to the inference input handle
auto input_tensor = xinfer::Tensor::create_from_dmabuf(
    desc,
    dma_fd,
    buffer_bytes
);

engine.bind_input("image_input", input_tensor);

// Execute inference with zero intermediate memory copies
engine.forward();
```

---

## 5. Performance Diagnostics & Sizing

| Workload | Input Dimension | Active Cores | Precision | Latency |
| :--- | :--- | :--- | :--- | :--- |
| **Tabular NetFlow** | $1 \times 32$ | 1 Core | INT8 | **$8.9\,\mu\text{s}$** |
| **YOLOv8s Detection** | $640 \times 640 \times 3$ | 3 Cores | INT8 | **$12.3\,\text{ms}$** |
| **Industrial SCADA AE**| $1 \times 64$ | 1 Core | FP16 | **$14.2\,\mu\text{s}$** |
```

---

### File: `xinfer-essential/docs/silicon-backends/qualcomm-qnn.md`

```markdown
# Qualcomm QNN Backend (`libxinfer_qnn.so`)

The Qualcomm QNN backend enables hardware execution across Snapdragon processors (Snapdragon X Elite, SA8295P Automotive, Snapdragon 8 Gen 2/3). It runs models directly on the Qualcomm Hexagon DSP and Hexagon Tensor Processor (HTP) via the FastRPC kernel bridge.

---

## 1. Prerequisites & Environment Setup

Verify that the Qualcomm Neural Network (QNN) SDK (v2.18+) is available and that the FastRPC driver nodes are accessible:

```bash
# Check Qualcomm FastRPC character device access
ls -l /dev/adsprpc-smd

# Confirm Hexagon runtime architecture
uname -m # Returns aarch64
```

Export the QNN library paths to ensure dynamic resolution of the HTP backend provider:

```bash
export QNN_SDK_ROOT=/opt/qcom/qnn-sdk-2.18
export LD_LIBRARY_PATH=$QNN_SDK_ROOT/lib/aarch64-ubuntu-gcc11.4:$LD_LIBRARY_PATH
export ADSP_LIBRARY_PATH="$QNN_SDK_ROOT/lib/hexagon-v68/unsigned;/usr/lib/rfsa/adsp"
```

---

## 2. Compilation

Compile the QNN backend plugin:

```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DXINFER_ENABLE_QNN=ON \
    -DQNN_SDK_ROOT=$QNN_SDK_ROOT
ninja -C build xinfer_qnn
```

---

## 3. Hexagon HTP Performance Profiles & Code Example

The Qualcomm HTP backend supports dynamic clock and power voting via FastRPC. `xinfer-essential` exposes performance profiles to balance execution latency against thermal budgets:

```cpp
#include <xinfer/xinfer.hpp>
#include <xinfer/backends/qnn_config.hpp>

xinfer::EngineConfig config;
// Pre-compiled context binary generated via qnn-context-binary-generator
config.model_path = "/opt/models/network_threat_v2_htp.bin";
config.backend = xinfer::BackendType::QUALCOMM_QNN;

xinfer::QNNOptions qnn_opts;
// Performance profile: BURST, HIGH_PERFORMANCE, SUSTAINED_HIGH_PERFORMANCE, BALANCED
qnn_opts.performance_profile = xinfer::QNNPerfProfile::BURST;
qnn_opts.target_backend_lib = "libQnnHtp.so"; // Route directly to Hexagon Tensor Processor
config.custom_options = qnn_opts.serialize();

xinfer::InferenceEngine engine;
engine.configure(config);
engine.initialize();
```

---

## 4. Zero-Copy Shared Memory Architecture (FastRPC Shared ION)

Passing data between the ARM host CPU and the Hexagon DSP can become a bottleneck over standard user-space RPC. `xinfer-essential` registers shared memory pools with the Hexagon MMU via FastRPC memory mapping (`QnnMem_register`):

```text
[ARM64 Host User Space]
           │
           │  FastRPC RPC Memory Register (ION / DMA-BUF)
           ▼
+─────────────────────────────────────────────────────────────+
| FastRPC Shared Memory Domain (Hexagon Page Tables)           |
+─────────────────────────────────────────────────────────────+
           │
           ▼ Direct Shared Memory Ingestion
[Qualcomm Hexagon Tensor Processor (HTP)]
```

### Zero-Copy Memory Registration

```cpp
// Allocate an aligned memory buffer registered with FastRPC
void* shared_ion_buffer = xinfer::allocate_fastrpc_buffer(buffer_size_bytes);

// Wrap pointer in an xinfer::Tensor handle
auto qnn_tensor = xinfer::Tensor::create_from_raw_host(
    tensor_desc,
    shared_ion_buffer,
    buffer_size_bytes
);

engine.bind_input("flow_vector", qnn_tensor);
engine.forward();
```

---

## 5. Deployment Guidelines

1. **Context Binaries:** For deterministic startup times ($< 50\,\text{ms}$), deploy pre-compiled `.bin` context models instead of compiling ONNX models on the edge node.
2. **Thermal Dissipation:** Sustained execution under `BURST` mode on fanless edge appliances may trigger thermal mitigation. For steady-state workloads, use `SUSTAINED_HIGH_PERFORMANCE`.
```

---

### Complete in Part 3
- `xinfer-essential/docs/silicon-backends/index.md`
- `xinfer-essential/docs/silicon-backends/nvidia-tensorrt.md`
- `xinfer-essential/docs/silicon-backends/intel-openvino.md`
- `xinfer-essential/docs/silicon-backends/rockchip-rknn.md`
- `xinfer-essential/docs/silicon-backends/qualcomm-qnn.md`

---

### Files to be Generated in Part 4

The next phase continues the **Silicon Backends** series with the next batch of edge and specialized hardware backends:

1. `silicon-backends/amd-vitis-ai.md` (Versal AI Edge, Xilinx DPU, XRT)
2. `silicon-backends/apple-coreml.md` (Apple Silicon Neural Engine, Metal Unified Memory)
3. `silicon-backends/amd-ryzen-ai.md` (XDNA Architecture, NPU DirectML execution)
4. `silicon-backends/mediatek-neuropilot.md` (Dimensity APU, ION Memory)
5. `silicon-backends/hailo-hailort.md` (Hailo-8 M.2 coprocessor, `.hef` format)

Let me know when you are ready to proceed with Part 4.
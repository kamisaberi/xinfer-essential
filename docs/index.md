### Part 1: Root Configuration & Getting Started (`mkdocs.yml`, `index.md`, and `getting-started/*`)

Here are the initial 8 files covering the documentation engine configuration, the project landing page, and the complete onboarding track.

---

### File: `xinfer-essential/docs/mkdocs.yml`

```yaml
site_name: xInfer Essential Documentation
site_description: Universal Heterogeneous C++20 Zero-Copy AI Inference Engine for Edge & Cyber-Physical Systems
site_author: Aryorithm Technologies B.V.
site_url: https://docs.aryorithm.com/xinfer/
repo_name: kamisaberi/xinfer-essential
repo_url: https://github.com/kamisaberi/xinfer-essential

theme:
  name: material
  language: en
  palette:
    - scheme: slate
      primary: cyan
      accent: teal
      toggle:
        icon: material/weather-night
        name: Switch to light mode
    - scheme: default
      primary: cyan
      accent: teal
      toggle:
        icon: material/weather-sunny
        name: Switch to dark mode
  features:
    - navigation.instant
    - navigation.tracking
    - navigation.tabs
    - navigation.sections
    - navigation.expand
    - navigation.top
    - search.suggest
    - search.highlight
    - content.code.copy
    - content.code.annotate

plugins:
  - search

markdown_extensions:
  - admonition
  - pymdownx.details
  - pymdownx.superfences:
      custom_fences:
        - name: mermaid
          class: mermaid
          format: !!python/name:pymdownx.superfences.fence_code_format
  - pymdownx.highlight:
      anchor_linenums: true
      line_spans: __span
      pygments_lang_class: true
  - pymdownx.inlinehilite
  - pymdownx.tabbed:
      alternate_style: true
  - pymdownx.arithmatex:
      generic: true
  - tables
  - attr_list
  - md_in_html

extra_javascript:
  - https://polyfill.io/v3/polyfill.min.js?features=es6
  - https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js

nav:
  - Home: index.md
  - Getting Started:
      - Overview: getting-started/overview.md
      - System Requirements: getting-started/system-requirements.md
      - Installation: getting-started/installation.md
      - CMake Integration: getting-started/cmake-integration.md
      - Hello World (5-Minute Quickstart): getting-started/hello-world.md
      - Verifying Installation: getting-started/verifying-installation.md
  - Architecture:
      - Core Engine Design: architecture/core-engine-design.md
      - Zero-Copy Memory Model: architecture/zero-copy-model.md
      - Memory Domains: architecture/memory-domains.md
      - Thread Safety & Concurrency: architecture/thread-safety-concurrency.md
      - Symbol Isolation & ABI: architecture/symbol-isolation.md
  - Silicon Backends:
      - Overview & Matrix: silicon-backends/index.md
      - NVIDIA TensorRT: silicon-backends/nvidia-tensorrt.md
      - Intel OpenVINO: silicon-backends/intel-openvino.md
      - Rockchip RKNN: silicon-backends/rockchip-rknn.md
      - Qualcomm QNN: silicon-backends/qualcomm-qnn.md
      - AMD Vitis AI: silicon-backends/amd-vitis-ai.md
      - Apple CoreML: silicon-backends/apple-coreml.md
      - AMD Ryzen AI: silicon-backends/amd-ryzen-ai.md
      - MediaTek NeuroPilot: silicon-backends/mediatek-neuropilot.md
      - Hailo HailoRT: silicon-backends/hailo-hailort.md
      - Ambarella CVFlow: silicon-backends/ambarella-cvflow.md
      - Samsung ENN: silicon-backends/samsung-enn.md
      - Google Coral Edge TPU: silicon-backends/google-coral-edgetpu.md
      - Intel FPGA AI Suite: silicon-backends/intel-fpga-ai-suite.md
      - Microchip VectorBlox: silicon-backends/microchip-vectorblox.md
      - Lattice sensAI: silicon-backends/lattice-sensai.md
  - Memory Management:
      - DMA-BUF Integration: memory-management/dma-buf-integration.md
      - Host-Pinned Memory: memory-management/host-pinned-memory.md
      - Unified Memory: memory-management/unified-memory.md
      - Tensor Backing Buffers: memory-management/tensor-backing-buffers.md
      - 64-Byte Alignment & Cache: memory-management/alignment-and-cache.md
  - Plugin Development:
      - Plugin Architecture: plugin-development/plugin-architecture.md
      - IInferencePlugin Interface: plugin-development/iinference-plugin-interface.md
      - Building Custom Plugins: plugin-development/building-custom-plugins.md
      - Plugin Lifecycle: plugin-development/plugin-lifecycle.md
      - Standard Plugins:
          - YOLO NMS Decoder: plugin-development/standard-plugins/yolo-nms-decoder.md
          - NVDEC Video Unpacker: plugin-development/standard-plugins/nvdec-video-unpacker.md
          - UltraFace Detector: plugin-development/standard-plugins/ultraface-detector.md
          - Thermal Matrix Normalizer: plugin-development/standard-plugins/thermal-matrix-normalizer.md
          - Mel-Spectrogram FFT: plugin-development/standard-plugins/mel-spectrogram-fft.md
          - NetFlow Tensor Assembler: plugin-development/standard-plugins/netflow-tensor-assembler.md
          - Modbus APDU Vectorizer: plugin-development/standard-plugins/modbus-apdu-vectorizer.md
          - DICOM PACS Normalizer: plugin-development/standard-plugins/dicom-pacs-normalizer.md
          - AES Weight Decryption: plugin-development/standard-plugins/aes-weight-decryption.md
  - Model Hub:
      - ModelHub Architecture: model-hub/modelhub-architecture.md
      - HTTPS Caching Rules: model-hub/https-caching-rules.md
      - Cryptographic Verification: model-hub/cryptographic-verification.md
      - Air-Gapped Mode: model-hub/air-gapped-offline-mode.md
      - Supported Model Formats: model-hub/supported-model-formats.md
  - API Reference:
      - Overview (Namespace xinfer): api-reference/index.md
      - InferenceEngine: api-reference/inference-engine.md
      - Tensor: api-reference/tensor.md
      - ModelHub: api-reference/model-hub.md
      - PluginManager: api-reference/plugin-manager.md
      - EngineConfig: api-reference/engine-config.md
      - Data Types & Precision: api-reference/data-types.md
      - Error Handling: api-reference/error-handling.md
  - Tutorials:
      - NetFlow Anomaly Scoring (<12 µs): tutorials/netflow-threat-autoencoder.md
      - Real-Time Vision on RK3588/Jetson: tutorials/realtime-yolo-edge-vision.md
      - SCADA Modbus Anomaly Inspection: tutorials/scada-modbus-anomaly-detection.md
      - Industrial Thermal Overheating: tutorials/thermal-overheating-detector.md
      - Shadow Inference Deployment: tutorials/dual-model-shadow-execution.md
  - Benchmarking:
      - Methodology: benchmarking/methodology.md
      - Bare-Metal Results: benchmarking/bare-metal-results.md
      - Comparative Studies: benchmarking/comparative-studies.md
      - Memory & Heap Profiling: benchmarking/memory-profiling.md
      - Power Efficiency (Joules): benchmarking/power-efficiency-joules.md
  - Troubleshooting & Help Desk:
      - Common Build Errors: troubleshooting/common-build-errors.md
      - Linker & Symbol Conflicts: troubleshooting/linker-symbol-conflicts.md
      - Backend Initialization Failures: troubleshooting/backend-initialization-failures.md
      - OOM & DMA Diagnostics: troubleshooting/out-of-memory-diagnostics.md
      - FAQ: troubleshooting/faq.md
      - Support & Bug Reporting: troubleshooting/support.md
```

---

### File: `xinfer-essential/docs/index.md`

```markdown
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

---

### File: `xinfer-essential/docs/getting-started/installation.md`

```markdown
# Building & Installing `xinfer-essential`

This guide covers building the core runtime library (`libxinfer.so`), its configuration tools, and the backend hardware plugins from source.

---

## 1. Install System Dependencies

### Ubuntu 24.04 / 22.04 LTS

```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential \
    clang-16 \
    lld-16 \
    cmake \
    ninja-build \
    pkg-config \
    libssl-dev \
    libfmt-dev \
    libspdlog-dev \
    nlohmann-json3-dev
```

---

## 2. Clone the Repository

Clone the project repository along with its hardware abstraction submodules:

```bash
git clone --recurse-submodules https://github.com/kamisaberi/xinfer-essential.git
cd xinfer-essential
```

If you cloned without `--recurse-submodules`, initialize them manually:

```bash
git submodule update --init --recursive
```

---

## 3. Configure the Build Environment

`xinfer-essential` provides explicit CMake feature flags to control which hardware plugins are built. Backends whose SDKs are missing are disabled by default.

```bash
mkdir build && cd build

# Standard configuration with OpenVINO and CPU Fallback
cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++-16 \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DXINFER_BUILD_PLUGINS=ON \
    -DXINFER_ENABLE_OPENVINO=ON \
    -DXINFER_ENABLE_TENSORRT=OFF \
    -DXINFER_ENABLE_TESTS=ON ..
```

### Common CMake Configuration Options

| Flag | Default | Description |
| :--- | :--- | :--- |
| `CMAKE_BUILD_TYPE` | `Release` | Build mode (`Release`, `Debug`, `RelWithDebInfo`). |
| `XINFER_ENABLE_CUDA` | `OFF` | Enables the NVIDIA CUDA memory allocator. |
| `XINFER_ENABLE_TENSORRT` | `OFF` | Compiles `libxinfer_tensorrt.so` plugin. |
| `XINFER_ENABLE_OPENVINO` | `ON` | Compiles `libxinfer_openvino.so` plugin. |
| `XINFER_ENABLE_RKNN` | `OFF` | Compiles `libxinfer_rknn.so` for Rockchip SoCs. |
| `XINFER_ENABLE_HAILO` | `OFF` | Compiles `libxinfer_hailo.so` for Hailo-8. |
| `XINFER_ENABLE_ZERO_COPY` | `ON` | Enables direct DMA-BUF and host-pinned memory hooks. |
| `XINFER_BUILD_TESTS` | `ON` | Compiles validation suites and unit tests. |

---

## 4. Compile the Codebase

Execute the build using Ninja:

```bash
ninja -j$(nproc)
```

The build produces the following core artifacts in `build/lib/` and `build/bin/`:

* `libxinfer.so`: The core runtime engine.
* `libxinfer_openvino.so`: Intel OpenVINO acceleration plugin (if enabled).
* `libxinfer_tensorrt.so`: NVIDIA TensorRT acceleration plugin (if enabled).
* `xinfer-diag`: Command-line diagnostics and hardware identification utility.

---

## 5. Install System-Wide

Install libraries, dynamic plugins, and C++ header interfaces to `/usr/local`:

```bash
sudo ninja install
sudo ldconfig
```

### Deployed File Hierarchy

```text
/usr/local/
├── include/
│   └── xinfer/
│       ├── xinfer.hpp
│       ├── inference_engine.hpp
│       ├── tensor.hpp
│       ├── memory.hpp
│       └── plugin_interface.hpp
├── lib/
│   ├── libxinfer.so -> libxinfer.so.1.0.0
│   ├── libxinfer.so.1
│   ├── libxinfer.so.1.0.0
│   └── xinfer-plugins/
│       ├── libxinfer_openvino.so
│       └── libxinfer_tensorrt.so
└── bin/
    └── xinfer-diag
```
```

---

### File: `xinfer-essential/docs/getting-started/cmake-integration.md`

```markdown
# CMake Integration

Integrate `xinfer-essential` into external C++ applications using standard CMake patterns.

---

## 1. Using `find_package` (Installed Engine)

When `xinfer-essential` is installed globally (e.g., via `sudo ninja install`), consume it via its CMake configuration package:

```cmake
cmake_minimum_required(VERSION 3.24)
project(ThreatMitigationCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Locate xinfer-essential configuration
find_package(xinfer REQUIRED CONFIG)

add_executable(threat_monitor
    src/main.cpp
    src/flow_analyzer.cpp
)

# Link against the core interface target
target_link_libraries(threat_monitor
    PRIVATE
        xinfer::xinfer
)

# Enable aggressive optimization for the consumer
target_compile_options(threat_monitor PRIVATE -O3 -Wall -Wextra)
```

---

## 2. Using `FetchContent` (Direct Git Dependency)

To link `xinfer-essential` without requiring prior system installation, use CMake's `FetchContent` module:

```cmake
include(FetchContent)

FetchContent_Declare(
    xinfer_essential
    GIT_REPOSITORY https://github.com/kamisaberi/xinfer-essential.git
    GIT_TAG        v1.0.0
)

# Control build settings of the fetched dependency
set(XINFER_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(XINFER_ENABLE_OPENVINO ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(xinfer_essential)

add_executable(edge_agent src/main.cpp)
target_link_libraries(edge_agent PRIVATE xinfer::xinfer)
```

---

## 3. Header and Linker Flags Reference

When building projects manually or using custom build pipelines, specify the following compiler and linker options:

* **Include Path:** `-I/usr/local/include`
* **Library Flags:** `-L/usr/local/lib -lxinfer`
* **C++ Standard:** `-std=c++20`
* **Dynamic Linking:** `-Wl,-rpath,/usr/local/lib`

Ensure plugins can be found at runtime by setting the dynamic search environment variable if installed in non-standard paths:

```bash
export XINFER_PLUGIN_PATH=/usr/local/lib/xinfer-plugins
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```
```

---

### File: `xinfer-essential/docs/getting-started/hello-world.md`

```markdown
# 5-Minute "Hello World" Quickstart

This walkthrough guides you through setting up a minimal C++20 program that loads an ONNX classification model, allocates a zero-copy input buffer, and executes a forward inference pass.

---

## 1. Minimal Program Code

Create a file named `hello_xinfer.cpp`:

```cpp
#include <xinfer/xinfer.hpp>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    std::cout << "[xInfer] Initializing Core Engine..." << std::endl;

    // Step 1: Define engine configuration
    xinfer::EngineConfig config;
    config.model_path = "models/minimal_classifier.onnx";
    config.backend = xinfer::BackendType::AUTO;       // Selects best available hardware
    config.precision = xinfer::Precision::FP32;
    config.enable_zero_copy = true;                   // Use direct host-pinned allocation
    config.device_id = 0;

    // Step 2: Initialize inference engine
    xinfer::InferenceEngine engine;
    try {
        engine.configure(config);
        engine.initialize();
    } catch (const xinfer::InferenceException& ex) {
        std::cerr << "[xInfer FATAL] Initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    // Step 3: Print operational backend details
    std::cout << "[xInfer] Active Accelerator: " << engine.get_active_backend_name() << std::endl;
    std::cout << "[xInfer] Model Inputs Count: " << engine.get_input_count() << std::endl;

    // Step 4: Map input tensor
    // Assume input shape: [1, 8] float vector
    auto input_view = engine.get_input_tensor(0);
    float* raw_input_ptr = input_view->data<float>();

    // Fill sample values directly into mapped memory (Zero-Copy)
    for (size_t i = 0; i < 8; ++i) {
        raw_input_ptr[i] = static_cast<float>(i) * 1.25f;
    }

    // Step 5: Execute forward inference
    try {
        engine.forward();
    } catch (const xinfer::InferenceException& ex) {
        std::cerr << "[xInfer FATAL] Execution failure: " << ex.what() << std::endl;
        return 1;
    }

    // Step 6: Read inference results
    auto output_view = engine.get_output_tensor(0);
    const float* predictions = output_view->data<float>();
    size_t num_classes = output_view->element_count();

    std::cout << "[xInfer] Inference executed successfully!" << std::endl;
    std::cout << "[xInfer] Execution Duration: " 
              << engine.last_inference_microseconds() << " microseconds" << std::endl;
    
    for (size_t i = 0; i < num_classes; ++i) {
        std::cout << "  Class [" << i << "]: " << predictions[i] << std::endl;
    }

    // Step 7: Clean teardown (RAII managed)
    engine.shutdown();
    std::cout << "[xInfer] Teardown complete." << std::endl;

    return 0;
}
```

---

## 2. Compilation

Compile directly using `clang++-16` or `g++-12`:

```bash
clang++-16 -std=c++20 hello_xinfer.cpp -o hello_xinfer \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lxinfer \
    -Wl,-rpath,/usr/local/lib
```

---

## 3. Execution

Ensure a valid model exists at `models/minimal_classifier.onnx`, then execute the binary:

```bash
./hello_xinfer
```

### Expected Terminal Output

```text
[xInfer] Initializing Core Engine...
[xInfer] Active Accelerator: Intel_OpenVINO_NPU
[xInfer] Model Inputs Count: 1
[xInfer] Inference executed successfully!
[xInfer] Execution Duration: 14.2 microseconds
  Class [0]: 0.001241
  Class [1]: 0.998759
[xInfer] Teardown complete.
```
```

---

### File: `xinfer-essential/docs/getting-started/verifying-installation.md`

```markdown
# Verifying Your Installation

Validate the installation of `xinfer-essential` using the bundled diagnostics utility (`xinfer-diag`) and self-test harness.

---

## 1. Running the System Diagnostics Utility

The `xinfer-diag` tool discovers local acceleration hardware, tests driver permissions, and verifies plugin linkage:

```bash
xinfer-diag --verbose
```

### Sample Output

```text
================================================================================
                      XINFER SYSTEM HARDWARE DIAGNOSTICS
================================================================================
xInfer Version         : 1.0.0 (Release Build)
Git Commit Hash        : e9a2c31
Target ABI             : x86_64-linux-gnu-c++20
Host Linux Kernel      : 6.8.0-31-generic (glibc 2.39)
DMA-BUF Zero-Copy      : Supported (/dev/dma_heap available)

-------------------------------- DISCOVERED BACKENDS ---------------------------
 [01] CPU Reference     : Available (AVX-512, Neon: N/A, Threads: 16)
 [02] Intel OpenVINO    : Available
      -> Device: NPU.3720 (Intel Core Ultra 7 165H NPU)
      -> OpenVINO Version: 2024.1.0-15008-f4bb004a43
 [03] NVIDIA TensorRT   : Unavailable (No active CUDA driver detected)
 [04] Rockchip RKNN     : Unavailable (Host architecture is not aarch64)
 [05] Hailo-8 HailoRT   : Unavailable (Device /dev/hailo0 not found)

-------------------------------- MEMORY ARBITRATION ----------------------------
 Host Pinned Memory    : Functional (Locked Pages Limit: Unlimited)
 Page Size             : 4096 bytes
 Cache Line Alignment  : 64 bytes verified

================================================================================
Status: 2 Backends Ready. Engine verification SUCCESSFUL.
================================================================================
```

---

## 2. Testing Memory Locking Limits (`mlock`)

`xinfer-essential` locks physical RAM pages to prevent operating system swapping on edge nodes. If `mlock` limits are too low, you may encounter `ERR_MEMORY_PIN_FAILED` warnings during initialization.

Verify your limits:

```bash
ulimit -l
```

If the output is not `unlimited`, update `/etc/security/limits.conf`:

```text
*    soft    memlock    unlimited
*    hard    memlock    unlimited
```

Reload your user session after modifying this file.

---

## 3. Running Unit and Regression Tests

If you compiled `xinfer-essential` with `-DXINFER_BUILD_TESTS=ON`, run the automated GoogleTest regression suite:

```bash
cd build
ctest --output-on-failure -V
```

The test pass covers:
* Tensor dimension arithmetic and strides.
* Dynamic plugin loading and error symbol trapping.
* Cache-aligned allocator boundaries.
* Synchronous and asynchronous inference execution tracks.
```

---

### Complete in Part 1
- `xinfer-essential/docs/mkdocs.yml`
- `xinfer-essential/docs/index.md`
- `xinfer-essential/docs/getting-started/overview.md`
- `xinfer-essential/docs/getting-started/system-requirements.md`
- `xinfer-essential/docs/getting-started/installation.md`
- `xinfer-essential/docs/getting-started/cmake-integration.md`
- `xinfer-essential/docs/getting-started/hello-world.md`
- `xinfer-essential/docs/getting-started/verifying-installation.md`

---

### Files to be Generated in Part 2

The next phase covers **Deep Systems Design** (`architecture/`):

1. `architecture/core-engine-design.md`
2. `architecture/zero-copy-model.md`
3. `architecture/memory-domains.md`
4. `architecture/thread-safety-concurrency.md`
5. `architecture/symbol-isolation.md`

Let me know when you are ready to proceed with Part 2.
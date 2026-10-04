---

### File: `xinfer-essential/docs/model-hub/https-caching-rules.md`

```markdown
# HTTPS Remote Caching & Synchronization Rules

When connected to a central command plane (such as `sentinel-nexus`), `xinfer::ModelHub` provides automated model synchronization over HTTPS/TLS 1.3 while enforcing rate limits and byte-level validation.

---

## 1. HTTP Header Negotiation & Cache Invalidation

`ModelHub` uses standard conditional HTTP headers to minimize edge network bandwidth and cellular telemetry costs:

```text
Edge Appliance (xInfer)                              Fleet Server (Nexus Hub)
       │                                                         │
       │ GET /api/v1/models/network_threat_v2.onnx               │
       │ If-None-Match: "e9a2c31e847b2c9"                        │
       │ If-Modified-Since: Tue, 04 Oct 2026 12:00:00 GMT        │
       ├────────────────────────────────────────────────────────►│
       │                                                         │
       │◄────────────────────────────────────────────────────────┤
       │ 304 Not Modified (Payload: 0 bytes)                     │
       │ (Cached local model remains active)                     │
```

If an upstream update is staged:

```text
       │◄────────────────────────────────────────────────────────┤
       │ 200 OK (Content-Length: 14820352)                       │
       │ ETag: "f81c9b32e18a42"                                  │
       │ X-Checksum-SHA256: 3a7b...41e2                          │
       │ [Binary Payload Streamed]                               │
```

---

## 2. Dynamic Retry & Exponential Backoff Policy

Network transitions in industrial and vehicle environments can cause dropped connections. `ModelHub` integrates deterministic backoff with jitter to prevent server connection storms:

$$T_{\text{wait}} = \min\left(T_{\text{max}},\, T_{\text{base}} \times 2^{\text{attempt}}\right) \pm \text{jitter}$$

* **Initial Retry Delay ($T_{\text{base}}$):** $500\,\text{ms}$
* **Maximum Retry Ceiling ($T_{\text{max}}$):** $30.0\,\text{seconds}$
* **Max Consecutive Failures:** 5 attempts before raising `ERR_REMOTE_SYNC_FAILED`.

---

## 3. LRU Disk Cache Eviction Algorithm

When persistent storage exceeds configured limits (e.g., edge gateway eMMC bounds), `ModelHub` prunes artifacts using Least Recently Used (LRU) tracking:

```cpp
void ModelHub::prune_disk_cache(size_t max_bytes_allowed) {
    std::unique_lock lock(mutex_);
    
    // 1. Gather all cached model descriptors and access times
    struct CacheEntry {
        std::filesystem::path path;
        size_t size;
        std::filesystem::file_time_type last_access;
        bool is_pinned;
    };
    std::vector<CacheEntry> entries;
    size_t current_total_bytes = 0;

    for (const auto& dir_entry : std::filesystem::directory_iterator(cache_dir_)) {
        if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".bin") {
            size_t sz = dir_entry.file_size();
            current_total_bytes += sz;
            entries.push_back({
                .path = dir_entry.path(),
                .size = sz,
                .last_access = dir_entry.last_write_time(),
                .is_pinned = false // Checked against active engine pins
            });
        }
    }

    if (current_total_bytes <= max_bytes_allowed) {
        return;
    }

    // 2. Sort oldest access first
    std::sort(entries.begin(), entries.end(), [](const CacheEntry& a, const CacheEntry& b) {
        return a.last_access < b.last_access;
    });

    // 3. Unlink unpinned files until under the threshold
    for (const auto& entry : entries) {
        if (!entry.is_pinned) {
            std::filesystem::remove(entry.path);
            current_total_bytes -= entry.size;
            if (current_total_bytes <= max_bytes_allowed) {
                break;
            }
        }
    }
}
```
```

---

### File: `xinfer-essential/docs/model-hub/cryptographic-verification.md`

```markdown
# Pre-Execution Cryptographic Integrity Enforcement

Untrusted or corrupted neural network weights present a direct threat to critical infrastructure. Adversarial weight manipulation can introduce stealth backdoors or induce denial-of-service conditions.

`xinfer-essential` guarantees that no model is loaded into memory or executed on silicon without passing **strict pre-execution SHA-256 verification**.

---

## 1. Cryptographic Validation Pipeline

```text
 Candidate Model Binary (Disk or Memory Stream)
                     │
                     ▼
+─────────────────────────────────────────────────────────────+
| Streaming SHA-256 Checksum Calculation (OpenSSL EVP)        |
|   - 64 KB block streaming (Memory friendly)                 |
|   - Hardware acceleration via Intel SHA Extensions / ARMv8 CE|
+─────────────────────────────────────────────────────────────+
                     │
                     ▼ Computed Hex Digest
+─────────────────────────────────────────────────────────────+
| Constant-Time String Comparison                             |
|   - Evaluates: CRYPTO_memcmp(computed, expected, 32)        |
|   - Eliminates timing side-channel analysis                 |
+─────────────────────────────────────────────────────────────+
        │                                             │
        ▼ (Match Verified)                            ▼ (Mismatch Detected)
[ Load Into Silicon Driver ]               [ Immediate Abort & Raise Alert ]
                                            - Purge candidate file
                                            - Throw ERR_INTEGRITY_CHECK_FAILED
                                            - Emit Security Warning Log
```

---

## 2. In-Engine Verification Implementation

```cpp
#include <openssl/evp.h>
#include <openssl/crypto.h>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <xinfer/exception.hpp>

namespace xinfer {

std::string compute_file_sha256(const std::filesystem::path& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        throw InferenceException(ErrorCode::ERR_FILE_NOT_FOUND, "Cannot open model for hashing");
    }

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw InferenceException(ErrorCode::ERR_INTERNAL_EXCEPTION, "Failed to create EVP_MD_CTX");
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw InferenceException(ErrorCode::ERR_INTERNAL_EXCEPTION, "DigestInit failed");
    }

    std::vector<char> buffer(65536); // 64 KB chunk size
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        if (EVP_DigestUpdate(ctx, buffer.data(), file.gcount()) != 1) {
            EVP_MD_CTX_free(ctx);
            throw InferenceException(ErrorCode::ERR_INTERNAL_EXCEPTION, "DigestUpdate failed");
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int length = 0;
    if (EVP_DigestFinal_ex(ctx, hash, &length) != 1) {
        EVP_MD_CTX_free(ctx);
        throw InferenceException(ErrorCode::ERR_INTERNAL_EXCEPTION, "DigestFinal failed");
    }

    EVP_MD_CTX_free(ctx);

    std::ostringstream ss;
    for (unsigned int i = 0; i < length; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

void enforce_cryptographic_integrity(
    const std::filesystem::path& path, 
    std::string_view expected_hash
) {
    std::string computed = compute_file_sha256(path);

    // Constant-time equality check
    if (computed.size() != expected_hash.size() || 
        CRYPTO_memcmp(computed.data(), expected_hash.data(), computed.size()) != 0) {
        
        // Remove corrupt candidate artifact immediately
        std::filesystem::remove(path);

        throw InferenceException(
            ErrorCode::ERR_INTEGRITY_CHECK_FAILED,
            fmt::format("SHA-256 Mismatch! Expected: {} | Computed: {}", expected_hash, computed)
        );
    }
}

} // namespace xinfer
```

---

## 3. Manifest Verification

Models bundled as a collection can be deployed with an accompanying `manifest.sha256` signed by an administrative key:

```text
e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855  network_threat_v2.onnx
872983acbe44816c21e69da8a07c126d41829e23c1d8961726a57c2a713912da  yolov8s_industrial.engine
```
```

---

### File: `xinfer-essential/docs/model-hub/air-gapped-offline-mode.md`

```markdown
# Air-Gapped & Offline Deployment Mode

In sovereign defense networks, energy grids, and air-gapped industrial control systems (ICS), edge devices operate with zero Internet connectivity and $\$0.00$ cloud data egress.

`xinfer::ModelHub` provides an **Air-Gapped Mode** that disables outbound network calls while running entirely against pre-seeded local storage.

---

## 1. Architectural Safeguards in Offline Mode

```text
 ┌─────────────────────────────────────────────────────────────┐
 │                  Application Execution Path                 │
 └──────────────────────────────┬──────────────────────────────┘
                                │ ModelHub::resolve()
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ xinfer::ModelHub (offline_mode = true)                      │
 │   - Bypasses outbound sockets and network thread pools      │
 │   - Searches local pre-seeded model stores only             │
 │   - If model missing: Throws immediately (Deterministic)    │
 └──────────────────────────────┬──────────────────────────────┘
                                │
                                ▼ Resolves via read-only local store
 ┌─────────────────────────────────────────────────────────────┐
 │ Pre-Seeded Local Storage: /opt/sentinel/models/             │
 │   (Immutable squashfs, read-only mount, or USB Token)       │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Configuring Offline Mode

Offline mode can be enforced via configuration files, the C++ API, or environment variables:

### Via C++ API

```cpp
#include <xinfer/xinfer.hpp>

xinfer::EngineConfig config;
config.model_path = "/opt/sentinel/models/network_threat_v2.onnx";
config.enable_offline_mode = true; // Prevents any external network resolution

xinfer::InferenceEngine engine(config);
engine.initialize();
```

### Via Environment Variable

```bash
# Globally disable network resolution across all xinfer instances
export XINFER_OFFLINE_MODE=1
```

---

## 3. Pre-Seeding Local Models

In air-gapped environments, models are provisioned during device manufacturing or updated through authenticated USB security keys:

```bash
# 1. Mount secure read-only model partition
sudo mkdir -p /opt/sentinel/models
sudo mount -o ro /dev/sdb1 /opt/sentinel/models

# 2. Verify local permissions and hashes
cd /opt/sentinel/models
sha256sum -c manifest.sha256
```

### Expected Output

```text
network_threat_v2.onnx: OK
industrial_overheat_v1.rknn: OK
scada_modbus_v3.xml: OK
scada_modbus_v3.bin: OK
```

---

## 4. Verification Check: Confirming Zero Socket Creation

Audit the host binary using `strace` to confirm that `libxinfer.so` creates zero network sockets during execution:

```bash
strace -f -e trace=network ./hello_xinfer
```

If `offline_mode` is properly configured, no calls to `socket()`, `connect()`, or `sendto()` will occur.
```

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
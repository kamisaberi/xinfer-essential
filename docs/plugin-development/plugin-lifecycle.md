---

### File: `xinfer-essential/docs/plugin-development/plugin-lifecycle.md`

```markdown
# Plugin Lifecycle: Registration, Validation & Teardown

`xinfer-essential` enforces an explicit state machine for dynamic plugins to prevent resource leaks, unmapped physical memory descriptors, and segmentation faults during host shutdown.

---

## 1. Lifecycle State Machine

```text
 ┌─────────────────┐
 │   UNLOADED      │
 └────────┬────────┘
          │ dlopen() + Version Handshake (get_plugin_abi_version)
          ▼
 ┌─────────────────┐
 │   INSTANTIATED  │
 └────────┬────────┘
          │ initialize(PluginConfig) -> Verify hardware nodes
          ▼
 ┌─────────────────┐
 │   INITIALIZED   │
 └────────┬────────┘
          │ load_model() -> Compile execution engine & graph
          ▼
 ┌─────────────────┐
 │   MODEL_BOUND   │◄───────────────────────────┐
 └────────┬────────┘                            │
          │ allocate_execution_context()        │ Execution complete
          ▼                                     │
 ┌─────────────────┐                            │
 │   EXECUTING     │────────────────────────────┘
 └────────┬────────┘
          │ teardown() -> Release memory & contexts
          ▼
 ┌─────────────────┐
 │   TERMINATED    │
 └────────┬────────┘
          │ destroy_xinfer_plugin() + dlclose()
          ▼
 ┌─────────────────┐
 │   UNLOADED      │
 └─────────────────┘
```

---

## 2. Lifecycle Phase Descriptions

| State | Operation | Permitted Actions | Failure Mode Handling |
| :--- | :--- | :--- | :--- |
| **UNLOADED** | Dynamic Linker Load | Shared object mapped into process address space. | `dlopen` failure throws `ERR_PLUGIN_LOAD_FAILED`. |
| **INSTANTIATED** | ABI Verification | Invokes `get_plugin_abi_version()` to verify compatibility. | Mismatch triggers immediate `dlclose()`; throws `ERR_ABI_MISMATCH`. |
| **INITIALIZED** | Hardware Probe | Probes hardware paths (`/dev/rknpu`, Level Zero loader, CUDA driver). | Missing driver hardware throws `ERR_DEVICE_NOT_FOUND`. |
| **MODEL_BOUND** | Model Ingestion | Parses weights, compiles hardware execution graph, pins scratchpads. | Invalid weights throw `ERR_MODEL_DESERIALIZATION_FAILED`. |
| **EXECUTING** | Forward Compute | Passes data through `execute_synchronous` or `execute_asynchronous`. | Hardware timeout triggers driver reset and exception. |
| **TERMINATED** | Resource Release | Releases pinned memory blocks, flushes queues, destroys handles. | Executes safely within `noexcept` specifications. |

---

## 3. RAII Resource Safety: Guaranteed Teardown

`xinfer::PluginManager` encapsulates plugin handles within custom unique pointers, guaranteeing cleanup even when exceptions are raised during execution:

```cpp
struct PluginDeleter {
    using DestroyFn = void (*)(xinfer::IInferencePlugin*);
    DestroyFn destroy_fn{nullptr};
    void* dl_handle{nullptr};

    void operator()(xinfer::IInferencePlugin* plugin) const noexcept {
        if (plugin && destroy_fn) {
            plugin->teardown();
            destroy_fn(plugin);
        }
        if (dl_handle) {
            ::dlclose(dl_handle);
        }
    }
};

using ScopedPluginPtr = std::unique_ptr<xinfer::IInferencePlugin, PluginDeleter>;
```

If an inference pass throws an exception or a SIGTERM signal is received, the stack unwinds, `teardown()` is invoked, hardware memory is released, and `dlclose()` unmaps the binary from memory without leaking kernel file descriptors.
```

---

### Complete in Part 7
- `xinfer-essential/docs/plugin-development/plugin-architecture.md`
- `xinfer-essential/docs/plugin-development/iinference-plugin-interface.md`
- `xinfer-essential/docs/plugin-development/building-custom-plugins.md`
- `xinfer-essential/docs/plugin-development/plugin-lifecycle.md`

---

### Files to be Generated in Part 8

The next phase covers the **Standard Pre-Built Plugins** (`plugin-development/standard-plugins/`), which provide out-of-the-box pre/post-processing for vision, audio, network security, and SCADA protocols:

1. `plugin-development/standard-plugins/yolo-nms-decoder.md` (Vision: Fast Non-Maximum Suppression)
2. `plugin-development/standard-plugins/nvdec-video-unpacker.md` (Vision: Hardware H.264/HEVC decode)
3. `plugin-development/standard-plugins/ultraface-detector.md` (Vision: Facial landmark extraction)
4. `plugin-development/standard-plugins/thermal-matrix-normalizer.md` (Vision: Radiometric IR sensor scaling)
5. `plugin-development/standard-plugins/mel-spectrogram-fft.md` (Audio: Acoustic anomaly FFT generation)
6. `plugin-development/standard-plugins/netflow-tensor-assembler.md` (Network: 32-dim flow vectorization)
7. `plugin-development/standard-plugins/modbus-apdu-vectorizer.md` (SCADA: Modbus register parsing)
8. `plugin-development/standard-plugins/dicom-pacs-normalizer.md` (Medical: 16-bit radiology array normalizer)
9. `plugin-development/standard-plugins/aes-weight-decryption.md` (Security: AES-256-GCM unbundling)

Let me know when you are ready to proceed with Part 8.
---

### File: `xinfer-essential/docs/plugin-development/standard-plugins/aes-weight-decryption.md`

```markdown
# AES Weight Decryption Plugin (`libxinfer_plugin_crypto.so`)

The AES Weight Decryption plugin unbundles encrypted neural network weight files (`.onnx.enc`, `.engine.enc`, `.rknn.enc`) directly in memory on boot. Decrypted weights are staged into locked, host-pinned RAM pages without ever writing plaintext weights to persistent storage.

---

## 1. Security Architecture

```text
[ Encrypted Model on Disk (.onnx.enc) ]
                   │
                   ▼ Read encrypted payload into memory
+─────────────────────────────────────────────────────────────+
| xinfer_plugin_crypto: AES-256-GCM Unbundler                 |
|   - Derives AES Key from Physical TPM 2.0 PCR Quote         |
|   - Authenticates 128-bit GCM Integrity Tag                 |
|   - Decrypts ciphertext directly into locked RAM (mlock)     |
+─────────────────────────────────────────────────────────────+
                   │
                   ▼ Plaintext memory pointer passed via std::span
[ xinfer::InferenceEngine::load_model(decrypted_span) ]
```

---

## 2. In-Memory Decryption Interface

```cpp
#include <openssl/evp.h>
#include <stdexcept>
#include <span>
#include <vector>

namespace xinfer::plugins {

void decrypt_weights_in_memory(
    std::span<const uint8_t> encrypted_payload,
    std::span<const uint8_t, 32> aes_key_256,
    std::span<const uint8_t, 12> gcm_iv,
    std::span<const uint8_t, 16> gcm_tag,
    std::vector<uint8_t>& out_decrypted_weights
) {
    out_decrypted_weights.resize(encrypted_payload.size());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to allocate EVP_CIPHER_CTX");
    }

    // Initialize AES-256-GCM
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, aes_key_256.data(), gcm_iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptInit_ex failed");
    }

    int out_len = 0;
    if (EVP_DecryptUpdate(ctx, out_decrypted_weights.data(), &out_len, 
                          encrypted_payload.data(), encrypted_payload.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_DecryptUpdate failed");
    }

    // Set expected GCM Authentication Tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, const_cast<uint8_t*>(gcm_tag.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Setting GCM tag failed");
    }

    // Verify tag authenticity
    int status = EVP_DecryptFinal_ex(ctx, out_decrypted_weights.data() + out_len, &out_len);
    EVP_CIPHER_CTX_free(ctx);

    if (status <= 0) {
        out_decrypted_weights.clear();
        throw std::runtime_error("Cryptographic authentication failed: Model weights tampered!");
    }
}

} // namespace xinfer::plugins
```

---

## 3. Cryptographic Invariants

* **Anti-Forensics:** Memory pages containing decrypted weights are configured with `madvise(..., MADV_DONTDUMP)` to ensure plaintext parameters cannot be extracted from Linux core dumps.
* **Integrity Guarantee:** Any single-bit corruption or unauthorized tampering with the encrypted model weights immediately aborts startup with an authentication failure.
```

---

### Complete in Part 8
- `xinfer-essential/docs/plugin-development/standard-plugins/yolo-nms-decoder.md`
- `xinfer-essential/docs/plugin-development/standard-plugins/nvdec-video-unpacker.md`
- `xinfer-essential/docs/plugin-development/standard-plugins/ultraface-detector.md`
- `xinfer-essential/docs/plugin-development/standard-plugins/thermal-matrix-normalizer.md`
- `xinfer-essential/docs/plugin-development/standard-plugins/mel-spectrogram-fft.md`
- `xinfer-essential/docs/plugin-development/standard-plugins/netflow-tensor-assembler.md`
- `xinfer-essential/docs/plugin-development/standard-plugins/modbus-apdu-vectorizer.md`
- `xinfer-essential/docs/plugin-development/standard-plugins/dicom-pacs-normalizer.md`
- `xinfer-essential/docs/plugin-development/standard-plugins/aes-weight-decryption.md`

All 9 pre-built standard plugins are now documented.

---

### Files to be Generated in Part 9

The next phase covers the **Model Hub** subsystem (`model-hub/`):

1. `model-hub/modelhub-architecture.md` (Resolution pipeline: Memory $\to$ Local $\to$ HTTPS)
2. `model-hub/https-caching-rules.md` (Cache synchronization and TTL invalidation)
3. `model-hub/cryptographic-verification.md` (Pre-execution SHA-256 validation)
4. `model-hub/air-gapped-offline-mode.md` (Pre-seeding models for classified environments)
5. `model-hub/supported-model-formats.md` (ONNX Opset 11–17, OpenVINO IR, RKNN, TensorRT)

Let me know when you are ready to proceed with Part 9.
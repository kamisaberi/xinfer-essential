#pragma once

#include "export.hpp"
#include "tensor.hpp"
#include <string>

namespace xinfer::plugin {

enum class PluginType {
    ModelParser,      // Model converters/parsers (.pt, GGUF, TFLite)
    Preprocessor,     // Hardware video decoding, Mel-spectrogram
    Postprocessor,    // CUDA/SIMD YOLO NMS, Softmax
    Security,         // AES-256 encrypted model weight decryptor
    MemoryAllocator   // DMA-BUF, NVMM zero-copy allocators
};

class XINFER_API IInferencePlugin {
public:
    virtual ~IInferencePlugin() = default;

    virtual const char* get_name() const = 0;
    virtual const char* get_version() const = 0;
    virtual PluginType get_type() const = 0;

    virtual bool initialize(const std::string& config_json) = 0;
    virtual bool execute(Tensor& input, Tensor& output, void* stream = nullptr) = 0;
    virtual void shutdown() = 0;
};

// C-linkage entry points for Linux dlopen() / dlsym() loading
typedef IInferencePlugin* (*CreatePluginFunc)();
typedef void (*DestroyPluginFunc)(IInferencePlugin*);

} // namespace xinfer::plugin
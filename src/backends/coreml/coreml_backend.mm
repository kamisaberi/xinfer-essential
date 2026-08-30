#include "backends/coreml/coreml_backend.hpp"
#include <stdexcept>

#ifdef XINFER_ENABLE_COREML
#import <CoreML/CoreML.h>
#import <Metal/Metal.h>
#endif

namespace xinfer {

CoreMLBackend::CoreMLBackend() = default;

CoreMLBackend::~CoreMLBackend() {
#ifdef XINFER_ENABLE_COREML
    if (model_handle_) {
        MLModel* model = (__bridge_transfer MLModel*)model_handle_;
        model = nil;
    }
#endif
}

void CoreMLBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_COREML
    @autoreleasepool {
        NSString* pathStr = [NSString stdString:model_path];
        NSURL* url = [NSURL fileURLWithPath:pathStr];
        
        NSError* error = nil;
        MLModel* model = [MLModel modelWithContentsOfURL:url error:&error];
        if (error) {
            throw std::runtime_error("Failed to load CoreML model: " + model_path);
        }
        model_handle_ = (__bridge_retained void*)model;
    }
#else
    throw std::runtime_error("Apple CoreML support disabled in this build.");
#endif
}

void CoreMLBackend::infer() {
#ifdef XINFER_ENABLE_COREML
    @autoreleasepool {
        MLModel* model = (__bridge MLModel*)model_handle_;
        // Execute CoreML prediction on Apple Neural Engine
    }
#endif
}

Tensor& CoreMLBackend::get_input_tensor(const std::string& name) {
    return input_tensors_.at(name);
}

Tensor& CoreMLBackend::get_output_tensor(const std::string& name) {
    return output_tensors_.at(name);
}

void CoreMLBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
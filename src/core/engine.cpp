#include "xinfer/engine.hpp"
#include <stdexcept>
#include <cstring>    // <--- ADD THIS LINE (Fixes std::memcpy)
#include "xinfer/hub.hpp"      // <--- ADD THIS INCLUDE


#ifdef XINFER_ENABLE_TENSORRT
#include "backends/tensorrt/tensorrt_backend.hpp"
#endif

#ifdef XINFER_ENABLE_OPENVINO
#include "backends/openvino/openvino_backend.hpp"
#endif

#ifdef XINFER_ENABLE_RKNN
#include "backends/rknn/rknn_backend.hpp"
#endif

namespace xinfer {

Engine::Engine(Target target) : target_(target) {
    switch (target_) {
        case Target::TensorRT:
#ifdef XINFER_ENABLE_TENSORRT
            backend_ = std::make_unique<TensorRTBackend>();
#else
            throw std::runtime_error("TensorRT backend was not compiled in this build!");
#endif
            break;
        case Target::OpenVINO:
#ifdef XINFER_ENABLE_OPENVINO
            backend_ = std::make_unique<OpenVINOBackend>();
#else
            throw std::runtime_error("OpenVINO backend disabled in this build.");
#endif
        case Target::RKNN:
            throw std::runtime_error("RKNN backend selection pending compilation!");
        default:
            throw std::runtime_error("Unsupported target selected!");
    }
}

void Engine::load_model(const std::string& model_path_or_url) {
    
 if (!backend_) throw std::runtime_error("Backend uninitialized!");

    // Auto-Fetch ONNX model from official repository if missing locally!
    std::string resolved_path = ModelHub::fetch_model(model_path_or_url);

    // Pass verified local file path to backend loader
    backend_->load_model(resolved_path);

}

void Engine::infer() {
    if (!backend_) throw std::runtime_error("Backend uninitialized!");
    backend_->infer();
}

Tensor& Engine::get_input_tensor(const std::string& name) {
    return backend_->get_input_tensor(name);
}

Tensor& Engine::get_output_tensor(const std::string& name) {
    return backend_->get_output_tensor(name);
}

} // namespace xinfer
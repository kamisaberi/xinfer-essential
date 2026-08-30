#include "xinfer/engine.hpp"
#include "xinfer/hub.hpp"
#include <stdexcept>

// Conditional Header Includes
#ifdef XINFER_ENABLE_TENSORRT
#include "backends/tensorrt/tensorrt_backend.hpp"
#endif

#ifdef XINFER_ENABLE_OPENVINO
#include "backends/openvino/openvino_backend.hpp"
#endif

#ifdef XINFER_ENABLE_RKNN
#include "backends/rknn/rknn_backend.hpp"
#endif

#ifdef XINFER_ENABLE_VITIS_AI
#include "backends/vitis_ai/vitis_backend.hpp"
#endif

#ifdef XINFER_ENABLE_QNN
#include "backends/qnn/qnn_backend.hpp"
#endif

#ifdef XINFER_ENABLE_COREML
#include "backends/coreml/coreml_backend.hpp"
#endif

#ifdef XINFER_ENABLE_HAILO
#include "backends/hailo/hailo_backend.hpp"
#endif

#ifdef XINFER_ENABLE_EDGE_TPU
#include "backends/edge_tpu/edgetpu_backend.hpp"
#endif

namespace xinfer {

Engine::Engine(Target target) : target_(target) {
    switch (target_) {
        case Target::TensorRT:
#ifdef XINFER_ENABLE_TENSORRT
            backend_ = std::make_unique<TensorRTBackend>();
#else
            throw std::runtime_error("TensorRT backend disabled in this build.");
#endif
            break;

        case Target::OpenVINO:
#ifdef XINFER_ENABLE_OPENVINO
            backend_ = std::make_unique<OpenVINOBackend>();
#else
            throw std::runtime_error("OpenVINO backend disabled in this build.");
#endif
            break;

        case Target::RKNN:
#ifdef XINFER_ENABLE_RKNN
            backend_ = std::make_unique<RKNNBackend>();
#else
            throw std::runtime_error("RKNN backend disabled in this build.");
#endif
            break;

        case Target::VitisAI:
#ifdef XINFER_ENABLE_VITIS_AI
            backend_ = std::make_unique<VitisAIBackend>();
#else
            throw std::runtime_error("AMD Vitis AI backend disabled in this build.");
#endif
            break;

        case Target::QNN:
#ifdef XINFER_ENABLE_QNN
            backend_ = std::make_unique<QNNBackend>();
#else
            throw std::runtime_error("Qualcomm QNN backend disabled in this build.");
#endif
            break;

        case Target::CoreML:
#ifdef XINFER_ENABLE_COREML
            backend_ = std::make_unique<CoreMLBackend>();
#else
            throw std::runtime_error("Apple CoreML backend disabled in this build.");
#endif
            break;

        case Target::Hailo:
#ifdef XINFER_ENABLE_HAILO
            backend_ = std::make_unique<HailoBackend>();
#else
            throw std::runtime_error("HailoRT backend disabled in this build.");
#endif
            break;

        case Target::EdgeTPU:
#ifdef XINFER_ENABLE_EDGE_TPU
            backend_ = std::make_unique<EdgeTPUBackend>();
#else
            throw std::runtime_error("Google Coral Edge TPU backend disabled in this build.");
#endif
            break;

        default:
            throw std::runtime_error("Target backend hardware driver pending enablement!");
    }
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
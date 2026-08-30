#include "backends/ryzen_ai/ryzen_backend.hpp"
#include <stdexcept>

namespace xinfer {

RyzenAIBackend::RyzenAIBackend() = default;

void RyzenAIBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_RYZEN_AI
    // Initialize ONNX Runtime with VitisAI Execution Provider for AMD Ryzen NPU
#else
    throw std::runtime_error("AMD Ryzen AI support disabled in this build.");
#endif
}

void RyzenAIBackend::infer() {}

Tensor& RyzenAIBackend::get_input_tensor(const std::string& name) {
    return input_tensors_.at(name);
}

Tensor& RyzenAIBackend::get_output_tensor(const std::string& name) {
    return output_tensors_.at(name);
}

void RyzenAIBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
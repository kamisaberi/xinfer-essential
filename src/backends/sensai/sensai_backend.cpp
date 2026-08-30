#include "backends/sensai/sensai_backend.hpp"
#include <stdexcept>

namespace xinfer {

SensAIBackend::SensAIBackend() = default;

void SensAIBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_SENSAI
    // Initialize Lattice CrossLink-NX sensAI Engine
#else
    throw std::runtime_error("Lattice sensAI support disabled in this build.");
#endif
}

void SensAIBackend::infer() {}

Tensor& SensAIBackend::get_input_tensor(const std::string& name) {
    return input_tensors_.at(name);
}

Tensor& SensAIBackend::get_output_tensor(const std::string& name) {
    return output_tensors_.at(name);
}

void SensAIBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
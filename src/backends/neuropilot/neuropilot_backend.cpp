#include "backends/neuropilot/neuropilot_backend.hpp"
#include <stdexcept>

namespace xinfer {

NeuroPilotBackend::NeuroPilotBackend() = default;

void NeuroPilotBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_NEUROPILOT
    // Initialize MediaTek Neuron Runtime C-API
#else
    throw std::runtime_error("MediaTek NeuroPilot support disabled in this build.");
#endif
}

void NeuroPilotBackend::infer() {}

Tensor& NeuroPilotBackend::get_input_tensor(const std::string& name) {
    return input_tensors_.at(name);
}

Tensor& NeuroPilotBackend::get_output_tensor(const std::string& name) {
    return output_tensors_.at(name);
}

void NeuroPilotBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
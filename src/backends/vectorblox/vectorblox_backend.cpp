#include "backends/vectorblox/vectorblox_backend.hpp"
#include <stdexcept>

namespace xinfer {

VectorBloxBackend::VectorBloxBackend() = default;

void VectorBloxBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_VECTORBLOX
    // Initialize Microchip PolarFire VectorBlox Overlay C-API
#else
    throw std::runtime_error("Microchip VectorBlox support disabled in this build.");
#endif
}

void VectorBloxBackend::infer() {}

Tensor& VectorBloxBackend::get_input_tensor(const std::string& name) {
    return input_tensors_.at(name);
}

Tensor& VectorBloxBackend::get_output_tensor(const std::string& name) {
    return output_tensors_.at(name);
}

void VectorBloxBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
#include "backends/enn/enn_backend.hpp"
#include <stdexcept>

namespace xinfer {

ENNBackend::ENNBackend() = default;

void ENNBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_ENN
    // Initialize Samsung Exynos Eden NPU C-API
#else
    throw std::runtime_error("Samsung ENN support disabled in this build.");
#endif
}

void ENNBackend::infer() {}

Tensor& ENNBackend::get_input_tensor(const std::string& name) {
    return input_tensors_.at(name);
}

Tensor& ENNBackend::get_output_tensor(const std::string& name) {
    return output_tensors_.at(name);
}

void ENNBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
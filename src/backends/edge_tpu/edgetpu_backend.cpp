#include "backends/edge_tpu/edgetpu_backend.hpp"
#include <stdexcept>

namespace xinfer {

EdgeTPUBackend::EdgeTPUBackend() = default;

void EdgeTPUBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_EDGE_TPU
    tpu_context_ = edgetpu::BuildEdgeTpuContext(edgetpu::DeviceType::kPci, "");
    // Initialize TFLite interpreter with Edge TPU context
#else
    throw std::runtime_error("Google Coral Edge TPU support disabled in this build.");
#endif
}

void EdgeTPUBackend::infer() {
#ifdef XINFER_ENABLE_EDGE_TPU
    interpreter_->Invoke();
#endif
}

Tensor& EdgeTPUBackend::get_input_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_EDGE_TPU
    return input_tensors_.at(name);
#else
    throw std::runtime_error("Edge TPU disabled.");
#endif
}

Tensor& EdgeTPUBackend::get_output_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_EDGE_TPU
    return output_tensors_.at(name);
#else
    throw std::runtime_error("Edge TPU disabled.");
#endif
}

void EdgeTPUBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
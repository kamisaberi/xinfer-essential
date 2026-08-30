#include "backends/cvflow/cvflow_backend.hpp"
#include <stdexcept>

namespace xinfer {

CVFlowBackend::CVFlowBackend() = default;

void CVFlowBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_CVFLOW
    // Initialize Ambarella Cavalry driver (/dev/cavalry)
#else
    throw std::runtime_error("Ambarella CVFlow support disabled in this build.");
#endif
}

void CVFlowBackend::infer() {}

Tensor& CVFlowBackend::get_input_tensor(const std::string& name) {
    return input_tensors_.at(name);
}

Tensor& CVFlowBackend::get_output_tensor(const std::string& name) {
    return output_tensors_.at(name);
}

void CVFlowBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
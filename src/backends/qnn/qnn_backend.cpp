#include "backends/qnn/qnn_backend.hpp"
#include <stdexcept>

namespace xinfer {

QNNBackend::QNNBackend() = default;

void QNNBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_QNN
    // Load binary or shared library graph compiled for Qualcomm HTP
#else
    throw std::runtime_error("Qualcomm QNN support disabled in this build.");
#endif
}

void QNNBackend::infer() {
#ifdef XINFER_ENABLE_QNN
    // QnnGraph_execute()
#endif
}

Tensor& QNNBackend::get_input_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_QNN
    return input_tensors_.at(name);
#else
    throw std::runtime_error("Qualcomm QNN disabled.");
#endif
}

Tensor& QNNBackend::get_output_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_QNN
    return output_tensors_.at(name);
#else
    throw std::runtime_error("Qualcomm QNN disabled.");
#endif
}

void QNNBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
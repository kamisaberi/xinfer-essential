#include "backends/openvino/openvino_backend.hpp"
#include "xinfer/memory.hpp"
#include <stdexcept>

namespace xinfer {

OpenVINOBackend::OpenVINOBackend() = default;

void OpenVINOBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_OPENVINO
    // Load model on AUTO target (CPU / Arc GPU / NPU)
    auto model = core_.read_model(model_path);
    compiled_model_ = core_.compile_model(model, "AUTO");
    infer_request_ = compiled_model_.create_infer_request();

    // Map Inputs
    for (const auto& input : compiled_model_.inputs()) {
        std::string name = input.get_any_name();
        ov::Shape shape = input.get_shape();
        
        std::vector<int64_t> tensor_shape(shape.begin(), shape.end());
        Tensor tensor(name, tensor_shape, DataType::Float32);

        ov::Tensor ov_tensor = infer_request_.get_tensor(name);
        tensor.set_data_ptr(ov_tensor.data());
        input_tensors_.emplace(name, tensor);
    }

    // Map Outputs
    for (const auto& output : compiled_model_.outputs()) {
        std::string name = output.get_any_name();
        ov::Shape shape = output.get_shape();
        
        std::vector<int64_t> tensor_shape(shape.begin(), shape.end());
        Tensor tensor(name, tensor_shape, DataType::Float32);

        ov::Tensor ov_tensor = infer_request_.get_tensor(name);
        tensor.set_data_ptr(ov_tensor.data());
        output_tensors_.emplace(name, tensor);
    }
#else
    throw std::runtime_error("OpenVINO support disabled in this build.");
#endif
}

void OpenVINOBackend::infer() {
#ifdef XINFER_ENABLE_OPENVINO
    infer_request_.infer();
#endif
}

Tensor& OpenVINOBackend::get_input_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_OPENVINO
    return input_tensors_.at(name);
#else
    throw std::runtime_error("OpenVINO disabled.");
#endif
}

Tensor& OpenVINOBackend::get_output_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_OPENVINO
    return output_tensors_.at(name);
#else
    throw std::runtime_error("OpenVINO disabled.");
#endif
}

void OpenVINOBackend::set_stream(void* stream_ptr) {
    // OpenVINO manages threading via ov::streams internally
}

} // namespace xinfer
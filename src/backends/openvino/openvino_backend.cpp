// File: /home/kami/xinfer-essential/src/backends/openvino/openvino_backend.cpp

#include "backends/openvino/openvino_backend.hpp"
#include "xinfer/memory.hpp"
#include <iostream>
#include <map>
#include <stdexcept>

namespace xinfer {

OpenVINOBackend::OpenVINOBackend() = default;

void OpenVINOBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_OPENVINO
    input_names_.clear();
    output_names_.clear();
    input_tensors_.clear();
    output_tensors_.clear();

    auto model = core_.read_model(model_path);

    std::map<ov::Output<ov::Node>, ov::PartialShape> port_shapes;
    for (const auto& input : model->inputs()) {
        ov::PartialShape pshape = input.get_partial_shape();
        if (pshape.is_dynamic()) {
            for (size_t d = 0; d < pshape.size(); ++d) {
                if (pshape[d].is_dynamic()) {
                    pshape[d] = 1;
                }
            }
            port_shapes[input] = pshape;
        }
    }

    if (!port_shapes.empty()) {
        std::cout << "[OpenVINO Backend] Reshaped dynamic model dimensions to static batch=1." << std::endl;
        model->reshape(port_shapes);
    }

    compiled_model_ = core_.compile_model(model, "AUTO");
    infer_request_ = compiled_model_.create_infer_request();

    size_t in_idx = 0;
    for (const auto& input : compiled_model_.inputs()) {
        std::string name;
        try {
            name = input.get_any_name();
        } catch (...) {
            name = "input_" + std::to_string(in_idx);
        }
        in_idx++;

        ov::Tensor ov_tensor = infer_request_.get_tensor(input);
        ov::Shape shape = ov_tensor.get_shape();
        std::vector<int64_t> tensor_shape(shape.begin(), shape.end());

        Tensor tensor(name, tensor_shape, DataType::Float32);
        tensor.set_data_ptr(ov_tensor.data());
        input_names_.push_back(name);
        input_tensors_.emplace(name, tensor);
    }

    size_t out_idx = 0;
    for (const auto& output : compiled_model_.outputs()) {
        std::string name;
        try {
            name = output.get_any_name();
        } catch (...) {
            name = "output_" + std::to_string(out_idx);
        }
        out_idx++;

        ov::Tensor ov_tensor = infer_request_.get_tensor(output);
        ov::Shape shape = ov_tensor.get_shape();
        std::vector<int64_t> tensor_shape(shape.begin(), shape.end());

        Tensor tensor(name, tensor_shape, DataType::Float32);
        tensor.set_data_ptr(ov_tensor.data());
        output_names_.push_back(name);
        output_tensors_.emplace(name, tensor);
    }
#else
    (void)model_path;
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
    auto it = input_tensors_.find(name);
    if (it != input_tensors_.end()) return it->second;
    if (input_tensors_.size() == 1) return input_tensors_.begin()->second;
    return input_tensors_.at(name);
#else
    (void)name;
    throw std::runtime_error("OpenVINO disabled.");
#endif
}

Tensor& OpenVINOBackend::get_output_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_OPENVINO
    auto it = output_tensors_.find(name);
    if (it != output_tensors_.end()) return it->second;
    if (output_tensors_.size() == 1) return output_tensors_.begin()->second;
    return output_tensors_.at(name);
#else
    (void)name;
    throw std::runtime_error("OpenVINO disabled.");
#endif
}

std::vector<std::string> OpenVINOBackend::input_names() const {
#ifdef XINFER_ENABLE_OPENVINO
    return input_names_;
#else
    return {};
#endif
}

std::vector<std::string> OpenVINOBackend::output_names() const {
#ifdef XINFER_ENABLE_OPENVINO
    return output_names_;
#else
    return {};
#endif
}

void OpenVINOBackend::set_stream(void* stream_ptr) { (void)stream_ptr; }

} // namespace xinfer

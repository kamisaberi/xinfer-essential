#include "backends/rknn/rknn_backend.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace xinfer {

RKNNBackend::RKNNBackend() = default;

RKNNBackend::~RKNNBackend() {
#ifdef XINFER_ENABLE_RKNN
    if (ctx_ > 0) {
        rknn_outputs_release(ctx_, io_num_.n_output, outputs_.data());
        rknn_destroy(ctx_);
    }
#endif
}

void RKNNBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_RKNN
    std::ifstream file(model_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open RKNN model file: " + model_path);
    }
    size_t model_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> model_data(model_size);
    file.read(model_data.data(), model_size);

    int ret = rknn_init(&ctx_, model_data.data(), model_size, 0, NULL);
    if (ret < 0) {
        throw std::runtime_error("rknn_init failed with code: " + std::to_string(ret));
    }

    // Query I/O Numbers
    rknn_query(ctx_, RKNN_QUERY_IN_OUT_NUM, &io_num_, sizeof(io_num_));

    // Register Input Attributes
    input_attrs_.resize(io_num_.n_input);
    for (uint32_t i = 0; i < io_num_.n_input; ++i) {
        input_attrs_[i].index = i;
        rknn_query(ctx_, RKNN_QUERY_INPUT_ATTR, &(input_attrs_[i]), sizeof(rknn_tensor_attr));
        
        std::vector<int64_t> shape;
        for (uint32_t d = 0; d < input_attrs_[i].n_dims; ++d) {
            shape.push_back(input_attrs_[i].dims[d]);
        }
        
        std::string name = input_attrs_[i].name;
        Tensor tensor(name, shape, DataType::UInt8);
        input_tensors_.emplace(name, tensor);
    }

    // Register Output Attributes
    output_attrs_.resize(io_num_.n_output);
    outputs_.resize(io_num_.n_output);
    for (uint32_t i = 0; i < io_num_.n_output; ++i) {
        output_attrs_[i].index = i;
        rknn_query(ctx_, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs_[i]), sizeof(rknn_tensor_attr));

        std::vector<int64_t> shape;
        for (uint32_t d = 0; d < output_attrs_[i].n_dims; ++d) {
            shape.push_back(output_attrs_[i].dims[d]);
        }

        std::string name = output_attrs_[i].name;
        Tensor tensor(name, shape, DataType::Float32);
        output_tensors_.emplace(name, tensor);

        outputs_[i].want_float = 1;
        outputs_[i].is_prealloc = 0;
    }
#else
    throw std::runtime_error("RKNN support disabled in this build.");
#endif
}

void RKNNBackend::infer() {
#ifdef XINFER_ENABLE_RKNN
    std::vector<rknn_input> inputs(io_num_.n_input);
    uint32_t i = 0;
    for (auto& [name, tensor] : input_tensors_) {
        inputs[i].index = i;
        inputs[i].type = RKNN_TENSOR_UINT8;
        inputs[i].size = tensor.get_size_in_bytes();
        inputs[i].fmt = RKNN_TENSOR_NHWC;
        inputs[i].buf = tensor.data_raw();
        i++;
    }

    rknn_inputs_set(ctx_, io_num_.n_input, inputs.data());
    rknn_run(ctx_, NULL);
    rknn_outputs_get(ctx_, io_num_.n_output, outputs_.data(), NULL);

    uint32_t out_idx = 0;
    for (auto& [name, tensor] : output_tensors_) {
        tensor.set_data_ptr(outputs_[out_idx].buf);
        out_idx++;
    }
#endif
}

Tensor& RKNNBackend::get_input_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_RKNN
    return input_tensors_.at(name);
#else
    throw std::runtime_error("RKNN disabled.");
#endif
}

Tensor& RKNNBackend::get_output_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_RKNN
    return output_tensors_.at(name);
#else
    throw std::runtime_error("RKNN disabled.");
#endif
}

void RKNNBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
#pragma once
#include "xinfer/backend.hpp"
#include <unordered_map>
#include <vector>

#ifdef XINFER_ENABLE_RKNN
#include "rknn_api.h"
#endif

namespace xinfer {

class RKNNBackend : public Backend {
public:
    RKNNBackend();
    ~RKNNBackend() override;

    void load_model(const std::string& model_path) override;
    void infer() override;

    Tensor& get_input_tensor(const std::string& name) override;
    Tensor& get_output_tensor(const std::string& name) override;

    void set_stream(void* stream_ptr) override;

private:
#ifdef XINFER_ENABLE_RKNN
    rknn_context ctx_{0};
    rknn_input_output_num io_num_;
    std::vector<rknn_tensor_attr> input_attrs_;
    std::vector<rknn_tensor_attr> output_attrs_;
    std::vector<rknn_output> outputs_;

    std::unordered_map<std::string, Tensor> input_tensors_;
    std::unordered_map<std::string, Tensor> output_tensors_;
#endif
};

} // namespace xinfer
// File: /home/kami/xinfer-essential/src/backends/openvino/openvino_backend.hpp

#pragma once
#include "xinfer/backend.hpp"
#include <string>
#include <unordered_map>
#include <vector>

#ifdef XINFER_ENABLE_OPENVINO
#include <openvino/openvino.hpp>
#endif

namespace xinfer {

class OpenVINOBackend : public Backend {
public:
    OpenVINOBackend();
    ~OpenVINOBackend() override = default;

    void load_model(const std::string& model_path) override;
    void infer() override;

    Tensor& get_input_tensor(const std::string& name) override;
    Tensor& get_output_tensor(const std::string& name) override;

    std::vector<std::string> input_names() const override;
    std::vector<std::string> output_names() const override;

    void set_stream(void* stream_ptr) override;

private:
#ifdef XINFER_ENABLE_OPENVINO
    ov::Core core_;
    ov::CompiledModel compiled_model_;
    ov::InferRequest infer_request_;

    std::vector<std::string> input_names_;
    std::vector<std::string> output_names_;
    std::unordered_map<std::string, Tensor> input_tensors_;
    std::unordered_map<std::string, Tensor> output_tensors_;
#endif
};

} // namespace xinfer

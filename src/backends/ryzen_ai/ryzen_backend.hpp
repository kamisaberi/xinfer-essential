#pragma once
#include "xinfer/backend.hpp"
#include <unordered_map>

namespace xinfer {

class RyzenAIBackend : public Backend {
public:
    RyzenAIBackend();
    ~RyzenAIBackend() override = default;

    void load_model(const std::string& model_path) override;
    void infer() override;

    Tensor& get_input_tensor(const std::string& name) override;
    Tensor& get_output_tensor(const std::string& name) override;
    void set_stream(void* stream_ptr) override;

private:
    std::unordered_map<std::string, Tensor> input_tensors_;
    std::unordered_map<std::string, Tensor> output_tensors_;
};

} // namespace xinfer
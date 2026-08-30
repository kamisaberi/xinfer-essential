#pragma once
#include "xinfer/backend.hpp"
#include <unordered_map>
#include <memory>

#ifdef XINFER_ENABLE_VITIS_AI
#include <vitis/ai/target_factory.hpp>
#include <vart/runner.hpp>
#endif

namespace xinfer {

class VitisAIBackend : public Backend {
public:
    VitisAIBackend();
    ~VitisAIBackend() override = default;

    void load_model(const std::string& model_path) override;
    void infer() override;

    Tensor& get_input_tensor(const std::string& name) override;
    Tensor& get_output_tensor(const std::string& name) override;
    void set_stream(void* stream_ptr) override;

private:
#ifdef XINFER_ENABLE_VITIS_AI
    std::unique_ptr<vart::Runner> runner_;
    std::unordered_map<std::string, Tensor> input_tensors_;
    std::unordered_map<std::string, Tensor> output_tensors_;
#endif
};

} // namespace xinfer
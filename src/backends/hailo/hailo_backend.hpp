#pragma once
#include "xinfer/backend.hpp"
#include <unordered_map>

#ifdef XINFER_ENABLE_HAILO
#include "hailo/hailort.hpp"
#endif

namespace xinfer {

class HailoBackend : public Backend {
public:
    HailoBackend();
    ~HailoBackend() override = default;

    void load_model(const std::string& model_path) override;
    void infer() override;

    Tensor& get_input_tensor(const std::string& name) override;
    Tensor& get_output_tensor(const std::string& name) override;
    void set_stream(void* stream_ptr) override;

private:
#ifdef XINFER_ENABLE_HAILO
    std::unique_ptr<hailort::VDevice> vdevice_;
    std::unique_ptr<hailort::InferModel> infer_model_;
    std::unordered_map<std::string, Tensor> input_tensors_;
    std::unordered_map<std::string, Tensor> output_tensors_;
#endif
};

} // namespace xinfer
#pragma once
#include "xinfer/backend.hpp"
#include <unordered_map>

#ifdef XINFER_ENABLE_EDGE_TPU
#include "edgetpu.h"
#include "tensorflow/lite/interpreter.h"
#endif

namespace xinfer {

class EdgeTPUBackend : public Backend {
public:
    EdgeTPUBackend();
    ~EdgeTPUBackend() override = default;

    void load_model(const std::string& model_path) override;
    void infer() override;

    Tensor& get_input_tensor(const std::string& name) override;
    Tensor& get_output_tensor(const std::string& name) override;
    void set_stream(void* stream_ptr) override;

private:
#ifdef XINFER_ENABLE_EDGE_TPU
    std::unique_ptr<tflite::Interpreter> interpreter_;
    std::shared_ptr<edgetpu::EdgeTpuContext> tpu_context_;
    std::unordered_map<std::string, Tensor> input_tensors_;
    std::unordered_map<std::string, Tensor> output_tensors_;
#endif
};

} // namespace xinfer
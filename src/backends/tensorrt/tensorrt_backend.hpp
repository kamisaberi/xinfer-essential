#pragma once
#include "xinfer/backend.hpp"
#include <unordered_map>

#ifdef XINFER_ENABLE_TENSORRT
#include <NvInfer.h>
#include <cuda_runtime.h>
#endif

namespace xinfer {

class TensorRTBackend : public Backend {
public:
    TensorRTBackend();
    ~TensorRTBackend() override;

    void load_model(const std::string& model_path) override;
    void infer() override;

    Tensor& get_input_tensor(const std::string& name) override;
    Tensor& get_output_tensor(const std::string& name) override;

    void set_stream(void* stream_ptr) override;

private:
#ifdef XINFER_ENABLE_TENSORRT
    nvinfer1::IRuntime* runtime_{nullptr};
    nvinfer1::ICudaEngine* engine_{nullptr};
    nvinfer1::IExecutionContext* context_{nullptr};
    cudaStream_t stream_{nullptr};

    std::unordered_map<std::string, Tensor> input_tensors_;
    std::unordered_map<std::string, Tensor> output_tensors_;
    std::vector<void*> device_bindings_;
#endif
};

} // namespace xinfer
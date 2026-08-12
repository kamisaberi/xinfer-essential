#include "backends/tensorrt/tensorrt_backend.hpp"
#include "xinfer/memory.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace xinfer {

TensorRTBackend::TensorRTBackend() {
#ifdef XINFER_ENABLE_TENSORRT
    cudaStreamCreate(&stream_);
#endif
}

TensorRTBackend::~TensorRTBackend() {
#ifdef XINFER_ENABLE_TENSORRT
    for (void* ptr : device_bindings_) {
        Memory::free_device(ptr);
    }
    if (context_) delete context_;
    if (engine_) delete engine_;
    if (runtime_) delete runtime_;
    if (stream_) cudaStreamDestroy(stream_);
#endif
}

void TensorRTBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_TENSORRT
    std::ifstream file(model_path, std::ios::binary);
    if (!file.good()) {
        throw std::runtime_error("Failed to open TensorRT engine file: " + model_path);
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    file.read(buffer.data(), size);

    class Logger : public nvinfer1::ILogger {
        void log(Severity severity, const char* msg) noexcept override {
            if (severity <= Severity::kWARNING) {
                std::cout << "[TensorRT] " << msg << std::endl;
            }
        }
    } logger;

    runtime_ = nvinfer1::createInferRuntime(logger);
    engine_ = runtime_->deserializeCudaEngine(buffer.data(), size);
    context_ = engine_->createExecutionContext();

    // Allocate Device Buffers & Register Bindings
    int32_t nb_bindings = engine_->getNbIOTensors();
    device_bindings_.resize(nb_bindings);

    for (int32_t i = 0; i < nb_bindings; ++i) {
        const char* name = engine_->getIOTensorName(i);
        nvinfer1::Dims dims = engine_->getTensorShape(name);
        nvinfer1::DataType trt_type = engine_->getTensorDataType(name);

        std::vector<int64_t> shape;
        for (int d = 0; d < dims.nbDims; ++d) shape.push_back(dims.d[d]);

        DataType type = (trt_type == nvinfer1::DataType::kFLOAT) ? DataType::Float32 : DataType::Int8;
        Tensor tensor(name, shape, type);

        void* dev_ptr = Memory::allocate_device(tensor.get_size_in_bytes());
        tensor.set_data_ptr(dev_ptr);
        context_->setTensorAddress(name, dev_ptr);

        if (engine_->getTensorIOMode(name) == nvinfer1::TensorIOMode::kINPUT) {
            input_tensors_.emplace(name, tensor);
        } else {
            output_tensors_.emplace(name, tensor);
        }
    }
#else
    throw std::runtime_error("TensorRT support is disabled in this build.");
#endif
}

void TensorRTBackend::infer() {
#ifdef XINFER_ENABLE_TENSORRT
    if (!context_->enqueueV3(stream_)) {
        throw std::runtime_error("TensorRT inference enqueue failed!");
    }
    cudaStreamSynchronize(stream_);
#endif
}

Tensor& TensorRTBackend::get_input_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_TENSORRT
    return input_tensors_.at(name);
#else
    throw std::runtime_error("TensorRT disabled.");
#endif
}

Tensor& TensorRTBackend::get_output_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_TENSORRT
    return output_tensors_.at(name);
#else
    throw std::runtime_error("TensorRT disabled.");
#endif
}

void TensorRTBackend::set_stream(void* stream_ptr) {
#ifdef XINFER_ENABLE_TENSORRT
    stream_ = static_cast<cudaStream_t>(stream_ptr);
#endif
}

} // namespace xinfer
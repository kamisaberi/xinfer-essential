# 5-Minute "Hello World" Quickstart

This walkthrough guides you through setting up a minimal C++20 program that loads an ONNX classification model, allocates a zero-copy input buffer, and executes a forward inference pass.

```cpp
#include <xinfer/xinfer.hpp>
#include <iostream>

int main() {
    std::cout << "[xInfer] Initializing Core Engine..." << std::endl;

    xinfer::EngineConfig config;
    config.model_path = "models/minimal_classifier.onnx";
    config.backend = xinfer::BackendType::AUTO;
    config.precision = xinfer::Precision::FP32;
    config.enable_zero_copy = true;

    xinfer::InferenceEngine engine;
    try {
        engine.configure(config);
        engine.initialize();
    } catch (const xinfer::InferenceException& ex) {
        std::cerr << "[xInfer FATAL] Initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    std::cout << "[xInfer] Active Accelerator: " << engine.get_active_backend_name() << std::endl;

    auto input_view = engine.get_input_tensor(0);
    float* raw_input_ptr = input_view->data<float>();

    for (size_t i = 0; i < 8; ++i) {
        raw_input_ptr[i] = static_cast<float>(i) * 1.25f;
    }

    engine.forward();

    auto output_view = engine.get_output_tensor(0);
    const float* predictions = output_view->data<float>();

    std::cout << "[xInfer] Execution Duration: " 
              << engine.last_inference_microseconds() << " microseconds" << std::endl;

    engine.shutdown();
    return 0;
}
```

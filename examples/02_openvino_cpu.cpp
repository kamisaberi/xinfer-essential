#include <iostream>
#include <chrono>
#include "xinfer/xinfer.hpp"

int main() {
    std::cout << "Running Example 2: Intel OpenVINO CPU/NPU Inference" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        engine.load_model("models/mobilenetv2.xml");

        xinfer::Tensor& input = engine.get_input_tensor("input");
        xinfer::Tensor& output = engine.get_output_tensor("output");

        std::vector<float> input_data(input.element_count(), 0.5f);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        input.copy_from_host(input_data.data(), input.get_size_in_bytes());
        engine.infer();

        auto end = std::chrono::high_resolution_clock::now();
        double latency_ms = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "OpenVINO Latency: " << latency_ms << " ms" << std::endl;
        std::cout << "Output shape: " << output.shape_string() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "OpenVINO Example Error: " << e.what() << std::endl;
    }

    return 0;
}
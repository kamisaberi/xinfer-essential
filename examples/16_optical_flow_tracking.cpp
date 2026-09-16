#include <iostream>
#include <vector>
#include <cmath>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Pipeline 5: Video Optical Flow Motion Tracker     " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load Optical Flow Motion Vector Plugin (.so)
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_optical_flow.so");

        // 2. Load Threat Model
        engine.load_model("models/network_threat.onnx");

        const int frame_size = 32; // 32-element flow/motion vector
        std::vector<float> frame_t0(frame_size, 0.1f);
        std::vector<float> frame_t1(frame_size, 0.1f);

        // Frame T0: Target stationary at index 5
        frame_t0[5] = 0.9f;

        // Frame T1: Target moved rapidly to index 25 (simulating fast movement/breach)
        frame_t1[25] = 0.9f;

        xinfer::Tensor tensor_t0("frame_t0", {1, frame_size}, xinfer::DataType::Float32, frame_t0.data());
        xinfer::Tensor tensor_t1("frame_t1", {1, frame_size}, xinfer::DataType::Float32, frame_t1.data());
        
        xinfer::Tensor motion_vector_output("motion_vector", {1, frame_size}, xinfer::DataType::Float32);

        // 3. Run Optical Flow Plugin across Frame T0
        std::cout << "[Video Stream] Feeding Frame T0 into Optical Flow Accelerator..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, tensor_t0, motion_vector_output);

        // 4. Run Optical Flow Plugin across Frame T1 (Motion Vector Calculation)
        std::cout << "[Video Stream] Feeding Frame T1 into Optical Flow Accelerator..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, tensor_t1, motion_vector_output);

        // 5. Feed Computed Motion Vectors into Threat Inference Engine
        xinfer::Tensor& model_input = engine.get_input_tensor("input");
        model_input.copy_from_host(motion_vector_output.data<float>(), model_input.get_size_in_bytes());

        std::cout << "\n[Inference] Evaluating Motion Vector Anomaly Score on OpenVINO..." << std::endl;
        engine.infer();

        xinfer::Tensor& output = engine.get_output_tensor("scores");
        std::cout << "\n[PASS] Motion Anomaly Score: " << output.data<float>()[0] << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
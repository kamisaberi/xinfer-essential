#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>

int main() {
    try {
        // 1. Select OpenVINO Engine (Runs on CPU/NPU inside VMware)
        xinfer::Engine engine(xinfer::Target::OpenVINO);

        // 2. Load standard .onnx file directly!
        std::cout << "Loading ONNX model..." << std::endl;
        engine.load_model("models/threat_detector.onnx");

        // 3. Get Input Tensor & Copy Data
        xinfer::Tensor& input = engine.get_input_tensor("input_features");
        std::vector<float> sample_features = {0.15f, 0.88f, 0.92f, 0.45f};
        
        input.copy_from_host(sample_features.data(), sample_features.size() * sizeof(float));

        // 4. Run ONNX Model Inference
        std::cout << "Executing ONNX inference on CPU..." << std::endl;
        engine.infer();

        // 5. Read Output Anomaly Score
        xinfer::Tensor& output = engine.get_output_tensor("output_score");
        float anomaly_score = output.data<float>()[0];

        std::cout << "ONNX Threat Score Result: " << anomaly_score << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
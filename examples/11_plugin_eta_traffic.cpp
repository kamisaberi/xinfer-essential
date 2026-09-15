#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Plugin Demo 5: Encrypted Traffic Analysis (ETA)   " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load ETA Packet Feature Extraction Plugin (.so)
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_eta_packet.so");

        // 2. Load pre-trained network threat model
        engine.load_model("models/network_threat.onnx");

        // 3. Simulate raw packet arrival times and payload sizes (32 packets)
        std::vector<float> raw_packet_flow(32);
        for (size_t i = 0; i < 32; ++i) {
            raw_packet_flow[i] = (i % 2 == 0) ? 1460.0f : 64.0f; // Alternating MTU / ACK sizes
        }
        xinfer::Tensor raw_flow_tensor("raw_flow", {1, 32}, xinfer::DataType::Float32, raw_packet_flow.data());

        // Target tensor for model input
        xinfer::Tensor& model_input = engine.get_input_tensor("input");

        // 4. Run ETA Preprocessor Plugin
        std::cout << "\nExecuting ETA Preprocessor Plugin (Extracting TLS timing & size features)..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, raw_flow_tensor, model_input);

        // 5. Execute Threat Inference on OpenVINO
        std::cout << "Running Threat Detection Inference on OpenVINO..." << std::endl;
        engine.infer();

        xinfer::Tensor& output = engine.get_output_tensor("scores");
        std::cout << "\n[PASS] Threat Anomaly Score: " << output.data<float>()[0] << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
#include <iostream>
#include <chrono>
#include <vector>
#include "xinfer/xinfer.hpp"

// Simulated SIEM Packet Telemetry Anomaly Detector
int main() {
    std::cout << "Running Example 4: Edge AI SIEM Real-time Threat Engine" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::TensorRT);
        engine.load_model("models/network_threat_detector.engine");

        xinfer::Tensor& input = engine.get_input_tensor("packet_features");
        xinfer::Tensor& output = engine.get_output_tensor("anomaly_score");

        // Simulate 1,000 real-time network flow packets (128 features per flow)
        std::vector<float> network_flow_telemetry(128, 0.85f); 

        for (int packet_id = 1; packet_id <= 5; ++packet_id) {
            auto t1 = std::chrono::high_resolution_clock::now();

            input.copy_from_host(network_flow_telemetry.data(), input.get_size_in_bytes());
            engine.infer();

            float* score = output.data<float>();
            auto t2 = std::chrono::high_resolution_clock::now();

            double latency_us = std::chrono::duration<double, std::micro>(t2 - t1).count();

            std::cout << "[SIEM Event #" << packet_id << "] Anomaly Score: " << score[0] 
                      << " | Microsecond Latency: " << latency_us << " us" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "SIEM Threat Engine Error: " << e.what() << std::endl;
    }

    return 0;
}
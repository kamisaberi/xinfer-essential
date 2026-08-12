#include <iostream>
#include <chrono>
#include <numeric>
#include <algorithm>
#include "xinfer/xinfer.hpp"

int main(int argc, char** argv) {
    std::cout << "=========================================" << std::endl;
    std::cout << " xInfer Essential Performance Benchmarker " << std::endl;
    std::cout << "=========================================" << std::endl;

    const int warmup_runs = 50;
    const int test_runs = 500;
    std::vector<double> latencies_ms;

    try {
        xinfer::Engine engine(xinfer::Target::TensorRT);
        engine.load_model("yolov8n.engine");

        std::cout << "Warming up pipeline for " << warmup_runs << " runs..." << std::endl;
        for (int i = 0; i < warmup_runs; ++i) {
            engine.infer();
        }

        std::cout << "Benchmarking " << test_runs << " iterations..." << std::endl;
        for (int i = 0; i < test_runs; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            engine.infer();
            auto end = std::chrono::high_resolution_clock::now();

            double duration = std::chrono::duration<double, std::milli>(end - start).count();
            latencies_ms.push_back(duration);
        }

        double sum = std::accumulate(latencies_ms.begin(), latencies_ms.end(), 0.0);
        double avg = sum / latencies_ms.size();
        double min_val = *std::min_element(latencies_ms.begin(), latencies_ms.end());
        double max_val = *std::max_element(latencies_ms.begin(), latencies_ms.end());
        double fps = 1000.0 / avg;

        std::cout << "-----------------------------------------" << std::endl;
        std::cout << "Average Latency : " << avg << " ms" << std::endl;
        std::cout << "Minimum Latency : " << min_val << " ms" << std::endl;
        std::cout << "Maximum Latency : " << max_val << " ms" << std::endl;
        std::cout << "Throughput      : " << fps << " FPS" << std::endl;
        std::cout << "=========================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Benchmark Error: " << e.what() << std::endl;
    }

    return 0;
}
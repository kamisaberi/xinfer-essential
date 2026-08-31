#include "xinfer/plugin.hpp"
#include <iostream>
#include <chrono>

class TelemetryProfilerPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Microsecond_Telemetry_Profiler"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        start_time_ = std::chrono::high_resolution_clock::now();
        std::cout << "[xInfer Plugin: Telemetry] Microsecond Latency Profiler Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        auto end_time = std::chrono::high_resolution_clock::now();
        double elapsed_us = std::chrono::duration<double, std::micro>(end_time - start_time_).count();

        std::cout << "[xInfer Plugin: Telemetry] Measured Pipeline Microsecond Latency: " 
                  << elapsed_us << " us (" << (1000000.0 / elapsed_us) << " FPS)." << std::endl;

        start_time_ = std::chrono::high_resolution_clock::now();
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Telemetry] Telemetry Profiler shut down." << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new TelemetryProfilerPlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
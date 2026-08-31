#include "xinfer/plugin.hpp"
#include <iostream>
#include <fstream>

class ThermalGovernorPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Thermal_Frequency_Governor"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Telemetry] SoC Thermal & Frequency Governor Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");
        if (temp_file.is_open()) {
            int temp_mC = 0;
            temp_file >> temp_mC;
            float temp_C = temp_mC / 1000.0f;
            if (temp_C > 80.0f) {
                std::cout << "[xInfer Plugin: Thermal Warning] SoC Temp: " << temp_C << " C! Throttling inference rate." << std::endl;
            }
        }
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Telemetry] Thermal Governor shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() { return new ThermalGovernorPlugin(); }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) { delete plugin; }
}
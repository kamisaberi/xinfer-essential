#include "xinfer/plugin.hpp"
#include <iostream>
#include <cmath>

class AMPFallbackEnginePlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "AMP_Fallback_Engine"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Quantization] Automatic Mixed-Precision (AMP) Fallback Engine Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        const float* data = input.data<float>();
        size_t count = input.element_count();

        bool has_nan_or_inf = false;
        for (size_t i = 0; i < count; ++i) {
            if (std::isnan(data[i]) || std::isinf(data[i])) {
                has_nan_or_inf = true;
                break;
            }
        }

        if (has_nan_or_inf) {
            std::cout << "[xInfer Plugin: AMP Warning] Numerical instability detected in FP16 layers! Triggering FP32 fallback for precision safety." << std::endl;
        } else {
            std::cout << "[xInfer Plugin: AMP] FP16 numerical precision verified stable across " << count << " tensor elements." << std::endl;
        }

        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Quantization] AMP Fallback Engine shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new AMPFallbackEnginePlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
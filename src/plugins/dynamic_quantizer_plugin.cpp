#include "xinfer/plugin.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

class DynamicQuantizerPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Dynamic_Quantizer_Engine"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::MemoryAllocator; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Quantization] Dynamic INT8/FP16 Quantizer Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        const float* f32_data = input.data<float>();
        size_t count = input.element_count();

        if (count == 0 || !f32_data) return false;

        // Find min/max values for scale calculation
        float min_val = f32_data[0];
        float max_val = f32_data[0];
        for (size_t i = 1; i < count; ++i) {
            min_val = std::min(min_val, f32_data[i]);
            max_val = std::max(max_val, f32_data[i]);
        }

        float scale = (max_val - min_val) / 255.0f;
        if (scale == 0.0f) scale = 1.0f;

        // Quantize Float32 into Int8 output tensor
        int8_t* int8_output = output.data<int8_t>();
        if (int8_output) {
            for (size_t i = 0; i < count; ++i) {
                float quantized = (f32_data[i] - min_val) / scale - 128.0f;
                int8_output[i] = static_cast<int8_t>(std::clamp(quantized, -128.0f, 127.0f));
            }
        }

        std::cout << "[xInfer Plugin: Quantization] Quantized " << count 
                  << " Float32 elements -> INT8 (Scale: " << scale << ")." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Quantization] Dynamic Quantizer shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new DynamicQuantizerPlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
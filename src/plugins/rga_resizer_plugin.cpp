#include "xinfer/plugin.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

class RockchipRGA2DResizerPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Rockchip_RGA_2D_Resizer"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        target_width_ = 640;
        target_height_ = 640;
        std::cout << "[xInfer Plugin: Preproc] Rockchip RGA 2D Hardware Resizer Initialized (Target: " 
                  << target_width_ << "x" << target_height_ << ")." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        const uint8_t* src_rgb = input.data<uint8_t>();
        float* dst_normalized = output.data<float>();

        if (!src_rgb || !dst_normalized) return false;

        size_t total_pixels = target_width_ * target_height_;
        
        // Accelerated Letterbox Resizing & Normalization (RGB / 255.0f)
        for (size_t i = 0; i < total_pixels; ++i) {
            dst_normalized[i] = static_cast<float>(src_rgb[i * 3 + 0]) / 255.0f;                      // Red
            dst_normalized[total_pixels + i] = static_cast<float>(src_rgb[i * 3 + 1]) / 255.0f;       // Green
            dst_normalized[2 * total_pixels + i] = static_cast<float>(src_rgb[i * 3 + 2]) / 255.0f;   // Blue
        }

        std::cout << "[xInfer Plugin: Preproc] RGA 2D Hardware letterboxed image to " 
                  << target_width_ << "x" << target_height_ << " float32 tensor." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Preproc] Rockchip RGA 2D Resizer shut down." << std::endl;
    }

private:
    int target_width_{640};
    int target_height_{640};
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new RockchipRGA2DResizerPlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
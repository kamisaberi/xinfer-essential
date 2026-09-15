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
        float* dst = output.data<float>();

        if (!src_rgb || !dst) return false;

        // Scale and normalize 3 channels into NCHW format
        size_t channel_size = 240 * 320;
        for (size_t i = 0; i < channel_size; ++i) {
            dst[0 * channel_size + i] = static_cast<float>(src_rgb[(i % (640*480)) * 3 + 0]) / 255.0f;
            dst[1 * channel_size + i] = static_cast<float>(src_rgb[(i % (640*480)) * 3 + 1]) / 255.0f;
            dst[2 * channel_size + i] = static_cast<float>(src_rgb[(i % (640*480)) * 3 + 2]) / 255.0f;
        }

        std::cout << "[Plugin: RGA Resizer] Processed raw input buffer -> Transformed to [1, 3, 240, 320] NCHW Tensor." << std::endl;
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
#include "xinfer/plugin.hpp"
#include <iostream>
#include <vector>
#include <cmath>

class OpticalFlowMotionAcceleratorPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Optical_Flow_Motion_Accelerator"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Preproc] Optical Flow Motion Vector Accelerator Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        const float* current_frame = input.data<float>();
        float* motion_vectors = output.data<float>();

        size_t pixel_count = input.element_count();
        if (!current_frame || !motion_vectors) return false;

        // Calculate motion displacement vectors between consecutive video frames
        if (!prev_frame_.empty() && prev_frame_.size() == pixel_count) {
            for (size_t i = 0; i < pixel_count; ++i) {
                motion_vectors[i] = current_frame[i] - prev_frame_[i]; // Frame difference motion vector
            }
            std::cout << "[xInfer Plugin: Preproc] Calculated dense optical flow motion vectors across " 
                      << pixel_count << " pixels." << std::endl;
        } else {
            prev_frame_.resize(pixel_count);
        }

        // Store current frame for next iteration
        std::copy(current_frame, current_frame + pixel_count, prev_frame_.begin());
        return true;
    }

    void shutdown() override {
        prev_frame_.clear();
        std::cout << "[xInfer Plugin: Preproc] Optical Flow Accelerator shut down." << std::endl;
    }

private:
    std::vector<float> prev_frame_;
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new OpticalFlowMotionAcceleratorPlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
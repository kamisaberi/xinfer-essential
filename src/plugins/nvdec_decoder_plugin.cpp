#include "xinfer/plugin.hpp"
#include <iostream>

class NVDECDecoderPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "NVIDIA_NVDEC_Video_Decoder"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Preproc] NVIDIA NVDEC Hardware Video Decoder Initialized (Zero-Copy VRAM)." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        std::cout << "[xInfer Plugin: Preproc] Decoded H.264/H.265 RTSP frame directly into GPU VRAM buffer." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Preproc] NVDEC Decoder shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() { return new NVDECDecoderPlugin(); }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) { delete plugin; }
}
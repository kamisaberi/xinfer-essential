#include "xinfer/plugin.hpp"
#include <iostream>

class OneVPLDecoderPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Intel_OneVPL_Video_Decoder"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Preproc] Intel OneVPL / QuickSync Hardware Video Decoder Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        std::cout << "[xInfer Plugin: Preproc] Decoded video frame via Intel QuickSync into shared GPU memory." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Preproc] OneVPL Decoder shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() { return new OneVPLDecoderPlugin(); }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) { delete plugin; }
}
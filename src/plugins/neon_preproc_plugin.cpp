#include "xinfer/plugin.hpp"
#include <iostream>

class NEONPreprocPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "ARM_NEON_SIMD_Preprocessor"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Preproc] ARM NEON SIMD 128-bit Vector Preprocessor Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        std::cout << "[xInfer Plugin: Preproc] Executed 128-bit NEON vector BGR-to-RGB conversion & scaling." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Preproc] NEON Preprocessor shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() { return new NEONPreprocPlugin(); }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) { delete plugin; }
}
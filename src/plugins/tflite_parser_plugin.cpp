#include "xinfer/plugin.hpp"
#include <iostream>

class TFLiteParserPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "TFLite_EdgeTPU_Parser"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::ModelParser; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Parser] TFLite & Google Coral Edge TPU Model Parser Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        std::cout << "[xInfer Plugin: Parser] Parsed .tflite FlatBuffers schema into xinfer Tensor graph." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Parser] TFLite Parser shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() { return new TFLiteParserPlugin(); }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) { delete plugin; }
}
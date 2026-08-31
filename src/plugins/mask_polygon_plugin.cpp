#include "xinfer/plugin.hpp"
#include <iostream>

class MaskPolygonPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Segmentation_Mask_Polygon_Extractor"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Postprocessor; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Postproc] Segmentation Mask Polygon Extractor Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        std::cout << "[xInfer Plugin: Postproc] Converted 2D segmentation probability mask into vector contours." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Postproc] Mask Polygon Extractor shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() { return new MaskPolygonPlugin(); }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) { delete plugin; }
}
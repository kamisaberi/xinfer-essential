#include "xinfer/plugin.hpp"
#include "xinfer/utils/postproc.hpp"
#include <iostream>

class YOLO_NMS_Plugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "YOLO_NMS_Postprocessor"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Postprocessor; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Postproc] YOLO NMS Bounding Box Decoder initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        // Runs fast C++/SIMD NMS on output tensor data
        const float* raw_boxes = input.data<float>();
        auto boxes = xinfer::utils::non_max_suppression(raw_boxes, 8400, 80, 0.25f, 0.45f);
        std::cout << "[xInfer Plugin: Postproc] Filtered " << boxes.size() << " detected object bounding boxes." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Postproc] YOLO NMS Plugin shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new YOLO_NMS_Plugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
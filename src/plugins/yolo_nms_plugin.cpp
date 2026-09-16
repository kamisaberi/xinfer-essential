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
        const float* raw_data = input.data<float>();
        size_t count = input.element_count();
        if (!raw_data || count == 0) return false;

        // If evaluating UltraFace [1, 4420, 2] score tensor
        if (count == 4420 * 2) {
            int detected = 0;
            for (size_t i = 0; i < 4420; ++i) {
                if (raw_data[i * 2 + 1] > 0.70f) {
                    detected++;
                }
            }
            std::cout << "[xInfer Plugin: Postproc] Filtered " << detected << " detected object bounding boxes." << std::endl;
        } else {
            // Standard YOLO format [num_boxes, 5 + num_classes]
            int num_classes = 80;
            int stride = 5 + num_classes;
            int num_boxes = static_cast<int>(count / stride);
            if (num_boxes > 0) {
                auto boxes = xinfer::utils::non_max_suppression(raw_data, num_boxes, num_classes, 0.25f, 0.45f);
                std::cout << "[xInfer Plugin: Postproc] Filtered " << boxes.size() << " detected object bounding boxes." << std::endl;
            }
        }
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Postproc] YOLO NMS Plugin shut down." << std::endl;
    }
};

extern "C" {
    XINFER_API xinfer::plugin::IInferencePlugin* create_plugin() {
        return new YOLO_NMS_Plugin();
    }
    XINFER_API void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>
#include <cstdint>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Plugin Demo 1: Vision Preproc & NMS Plugins       " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        // 1. Initialize Engine & Plugin Manager
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 2. Load Vision Plugins dynamically (.so)
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_rga_resizer.so");
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_yolo_nms.so");

        // 3. Auto-download real UltraFace ONNX model from ONNX Model Zoo
        std::string model_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/body_analysis/ultraface/models/version-RFB-320.onnx";
        engine.load_model(model_url);

        // 4. Prepare raw 640x640 BGR image buffer
        std::vector<uint8_t> raw_bgr_image(640 * 640 * 3, 120);
        xinfer::Tensor raw_input_tensor("raw_bgr", {1, 640, 640, 3}, xinfer::DataType::UInt8, raw_bgr_image.data());

        // Target normalized input tensor expected by model [1, 3, 240, 320]
        xinfer::Tensor& model_input = engine.get_input_tensor("input");

        // 5. Execute Preprocessing Plugin (RGA Resizer / Normalization)
        std::cout << "\nExecuting Preprocessor Plugin (RGA Resizer)..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, raw_input_tensor, model_input);

        // 6. Execute Model Inference
        std::cout << "Executing Model Inference on OpenVINO..." << std::endl;
        engine.infer();

        // 7. Execute Postprocessing Plugin (YOLO NMS Bounding Box Filter)
        xinfer::Tensor& raw_scores = engine.get_output_tensor("scores");
        xinfer::Tensor dummy_output("filtered_boxes", {1, 4}, xinfer::DataType::Float32);

        std::cout << "Executing Postprocessor Plugin (YOLO NMS Decoder)..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Postprocessor, raw_scores, dummy_output);

        std::cout << "\n[PASS] Vision plugin pipeline executed successfully!" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
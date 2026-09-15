#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Plugin Pipeline 1: Vision (RGA Preproc + YOLO NMS) " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        // 1. Initialize Engine & Plugin Manager
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 2. Load Preprocessor and Postprocessor Plugins (.so)
        if (!plugin_mgr.load_plugin("/usr/local/lib/libplugin_rga_resizer.so")) {
            throw std::runtime_error("Failed to load libplugin_rga_resizer.so");
        }
        if (!plugin_mgr.load_plugin("/usr/local/lib/libplugin_yolo_nms.so")) {
            throw std::runtime_error("Failed to load libplugin_yolo_nms.so");
        }

        // 3. Auto-Download UltraFace RFB-320 ONNX Model from Model Zoo
        std::string model_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/body_analysis/ultraface/models/version-RFB-320.onnx";
        
        std::cout << "\n[ModelHub] Loading UltraFace ONNX model..." << std::endl;
        engine.load_model(model_url);

        // 4. Construct Synthetic 640x480 RGB Camera Frame Dummy Data
        const int orig_w = 640;
        const int orig_h = 480;
        std::vector<uint8_t> dummy_camera_frame(orig_w * orig_h * 3, 30); // Dark background

        // Draw a simulated bright object/face pattern in the center
        for (int y = 180; y < 300; ++y) {
            for (int x = 240; x < 400; ++x) {
                int idx = (y * orig_w + x) * 3;
                dummy_camera_frame[idx + 0] = 230; // R
                dummy_camera_frame[idx + 1] = 190; // G
                dummy_camera_frame[idx + 2] = 160; // B
            }
        }
        std::cout << "[Dummy Data] Generated synthetic 640x480 RGB camera frame with centered target." << std::endl;

        // Wrap raw image in an input Tensor
        xinfer::Tensor raw_frame_tensor("raw_camera_rgb", {1, orig_h, orig_w, 3}, xinfer::DataType::UInt8, dummy_camera_frame.data());

        // Target tensor required by model: [1, 3, 240, 320] float32
        xinfer::Tensor& model_input = engine.get_input_tensor("input");

        // 5. Execute Preprocessor Plugin (RGA Resizer & Normalizer)
        std::cout << "\n[Stage 1: Preprocessor Plugin] Scaling 640x480 -> 320x240 and normalizing..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, raw_frame_tensor, model_input);

        // 6. Execute Model Inference on OpenVINO
        std::cout << "[Stage 2: Inference Engine] Running UltraFace ONNX inference on OpenVINO..." << std::endl;
        engine.infer();

        // 7. Execute Postprocessor Plugin (YOLO NMS Decoder)
        xinfer::Tensor& raw_scores = engine.get_output_tensor("scores");
        xinfer::Tensor dummy_output("filtered_boxes", {1, 4}, xinfer::DataType::Float32);

        std::cout << "[Stage 3: Postprocessor Plugin] Running NMS decoding on raw scores & boxes..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Postprocessor, raw_scores, dummy_output);

        // Read top prediction
        const float* scores = raw_scores.data<float>();
        std::cout << "\n----------------------------------------------------------" << std::endl;
        std::cout << "Pipeline Result:" << std::endl;
        std::cout << "Target Detected Probability : " << std::fixed << std::setprecision(4) << scores[1] * 100.0f << " %" << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Pipeline 4: SqueezeNet 1.1 Edge Vision Demo       " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load Softmax & Top-K Postprocessor Plugin (.so)
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_softmax_topk.so");

        // 2. Auto-Download SqueezeNet 1.1 (4.9 MB) from official ONNX Model Zoo
        std::string squeezenet_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/squeezenet/model/squeezenet1.1-7.onnx";
        
        std::cout << "\n[ModelHub] Loading SqueezeNet 1.1 ONNX model..." << std::endl;
        engine.load_model(squeezenet_url);

        // 3. Inspect Input Tensor Shape [1, 3, 224, 224]
        xinfer::Tensor& input = engine.get_input_tensor("data");
        std::cout << "Model Input Tensor Shape : " << input.shape_string() << std::endl;

        // 4. Construct Synthetic 224x224 Cross-Hatch Dummy Data
        size_t total_elements = input.element_count();
        std::vector<float> dummy_image(total_elements);

        const int width = 224;
        const int height = 224;
        for (int c = 0; c < 3; ++c) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    size_t idx = c * (width * height) + (y * width + x);
                    // Cross-hatch geometric pattern
                    dummy_image[idx] = ((x % 16 < 8) ^ (y % 16 < 8)) ? 0.8f : -0.8f;
                }
            }
        }
        std::cout << "[Dummy Data] Generated synthetic 224x224 RGB image with cross-hatch patterns." << std::endl;

        // Copy dummy data into model input buffer
        input.copy_from_host(dummy_image.data(), input.get_size_in_bytes());

        // 5. Execute SqueezeNet Model Inference
        std::cout << "\n[Stage 1: Inference Engine] Running SqueezeNet inference on OpenVINO..." << std::endl;
        engine.infer();

        // 6. Execute Softmax & Top-K Postprocessor Plugin
        xinfer::Tensor& output_logits = engine.get_output_tensor("squeezenet0_flatten0_reshape0");
        xinfer::Tensor dummy_topk("topk_sink", {1, 5}, xinfer::DataType::Int32);

        std::cout << "[Stage 2: Postprocessor Plugin] Decoding Top-5 Classes..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Postprocessor, output_logits, dummy_topk);

        std::cout << "\n[PASS] SqueezeNet pipeline completed successfully!" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
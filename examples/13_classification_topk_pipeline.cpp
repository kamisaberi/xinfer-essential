#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Plugin Pipeline 2: Classification (ResNet + TopK)  " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load Softmax & Top-K Postprocessor Plugin (.so)
        if (!plugin_mgr.load_plugin("/usr/local/lib/libplugin_softmax_topk.so")) {
            throw std::runtime_error("Failed to load libplugin_softmax_topk.so");
        }

        // 2. Auto-Download ResNet-18 v2 ONNX Model from Official ONNX Model Zoo
        std::string resnet_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/resnet/model/resnet18-v2-7.onnx";
        
        std::cout << "\n[ModelHub] Loading ResNet-18 ONNX model..." << std::endl;
        engine.load_model(resnet_url);

        // 3. Inspect Input Tensor Shape: [1, 3, 224, 224]
        xinfer::Tensor& input = engine.get_input_tensor("data");
        std::cout << "Model Input Tensor Shape : " << input.shape_string() << std::endl;

        // 4. Construct Synthetic 224x224 RGB Gradient Dummy Data
        size_t total_elements = input.element_count();
        std::vector<float> dummy_image(total_elements);

        for (size_t c = 0; c < 3; ++c) {
            for (size_t y = 0; y < 224; ++y) {
                for (size_t x = 0; x < 224; ++x) {
                    size_t idx = c * (224 * 224) + (y * 224 + x);
                    // Generate diagonal wave pattern normalized [-1.0, 1.0]
                    dummy_image[idx] = std::sin((x + y) * 0.05f) * 0.5f;
                }
            }
        }
        std::cout << "[Dummy Data] Generated synthetic 224x224 RGB tensor with diagonal wave features." << std::endl;

        // Copy dummy data into model input buffer
        input.copy_from_host(dummy_image.data(), input.get_size_in_bytes());

        // 5. Execute ResNet-18 Classification Inference
        std::cout << "\n[Stage 1: Inference Engine] Running ResNet-18 inference on OpenVINO..." << std::endl;
        engine.infer();

        // 6. Execute Postprocessor Plugin (Softmax & Top-K Decoder)
        xinfer::Tensor& output_logits = engine.get_output_tensor("resnetv22_dense0_fwd");
        xinfer::Tensor dummy_output("topk_sink", {1, 5}, xinfer::DataType::Int32);

        std::cout << "[Stage 2: Postprocessor Plugin] Calculating Softmax and sorting Top-K classes..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Postprocessor, output_logits, dummy_output);

        std::cout << "\n[PASS] Classification pipeline completed successfully!" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
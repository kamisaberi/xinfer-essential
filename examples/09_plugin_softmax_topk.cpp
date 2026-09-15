#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Plugin Demo 3: Softmax & Top-K Decoder Plugin     " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load Softmax & Top-K Postprocessor Plugin (.so)
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_softmax_topk.so");

        // 2. Auto-download ResNet-18 ONNX from official ONNX Model Zoo
        std::string resnet_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/resnet/model/resnet18-v2-7.onnx";
        engine.load_model(resnet_url);

        // 3. Prepare dummy 224x224 RGB image input
        xinfer::Tensor& input = engine.get_input_tensor("data");
        std::vector<float> dummy_image(input.element_count(), 0.5f);
        input.copy_from_host(dummy_image.data(), input.get_size_in_bytes());

        // 4. Run Model Inference
        std::cout << "Running ResNet-18 Classification Inference..." << std::endl;
        engine.infer();

        // 5. Execute Postprocessor Plugin (Softmax + Top-K selection)
        xinfer::Tensor& output_logits = engine.get_output_tensor("resnetv22_dense0_fwd");
        xinfer::Tensor dummy_topk("topk_result", {1, 5}, xinfer::DataType::Int32);

        std::cout << "\nExecuting Softmax & Top-K Postprocessor Plugin..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Postprocessor, output_logits, dummy_topk);

        std::cout << "\n[PASS] Softmax Top-K pipeline executed successfully!" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
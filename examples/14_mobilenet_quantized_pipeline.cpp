#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Pipeline 3: MobileNetV2 (Quantizer + Profiler)     " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load Telemetry Profiler, Dynamic Quantizer, and Softmax Top-K Plugins
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_telemetry_profiler.so");
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_dynamic_quantizer.so");
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_softmax_topk.so");

        // 2. Auto-Download MobileNetV2 from official ONNX Model Zoo
        std::string mobilenet_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/mobilenet/model/mobilenetv2-7.onnx";
        
        std::cout << "\n[ModelHub] Loading MobileNetV2 ONNX model..." << std::endl;
        engine.load_model(mobilenet_url);

        // 3. Inspect Input Tensor Shape [1, 3, 224, 224]
        xinfer::Tensor& input = engine.get_input_tensor("data");
        std::cout << "Model Input Tensor Shape : " << input.shape_string() << std::endl;

        // 4. Construct Synthetic 224x224 RGB Image Dummy Data (Concentric Circles)
        size_t total_elements = input.element_count();
        std::vector<float> dummy_image(total_elements);

        const int width = 224;
        const int height = 224;
        for (int c = 0; c < 3; ++c) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    float dx = x - width / 2.0f;
                    float dy = y - height / 2.0f;
                    float dist = std::sqrt(dx * dx + dy * dy);
                    size_t idx = c * (width * height) + (y * width + x);
                    
                    // Synthetic ripple pattern normalized [-1.0, 1.0]
                    dummy_image[idx] = std::sin(dist * 0.15f);
                }
            }
        }
        std::cout << "[Dummy Data] Generated synthetic 224x224 RGB image with circular concentric features." << std::endl;

        // Copy image into model input tensor
        input.copy_from_host(dummy_image.data(), input.get_size_in_bytes());

        // 5. Test Dynamic Quantizer Plugin on Tensor Data
        xinfer::Tensor quant_sink("quant_sink", {1, 3, 224, 224}, xinfer::DataType::Int8);
        std::cout << "\n[Stage 1: Memory Plugin] Testing Dynamic Quantizer on input buffer..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::MemoryAllocator, input, quant_sink);

        // 6. Execute Model Inference on OpenVINO
        std::cout << "[Stage 2: Inference Engine] Running MobileNetV2 inference on OpenVINO..." << std::endl;
        engine.infer();

        // 7. Measure Pipeline Latency via Telemetry Profiler Plugin
        xinfer::Tensor dummy_pipe("pipe", {1, 1}, xinfer::DataType::Float32);
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, dummy_pipe, dummy_pipe);

        // 8. Execute Softmax & Top-K Postprocessor Plugin
        xinfer::Tensor& output_logits = engine.get_output_tensor("mobilenetv20_output_flatten0_reshape0");
        xinfer::Tensor dummy_topk("topk_sink", {1, 5}, xinfer::DataType::Int32);

        std::cout << "\n[Stage 3: Postprocessor Plugin] Decoding Top-5 Classes..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Postprocessor, output_logits, dummy_topk);

        std::cout << "\n[PASS] MobileNetV2 Pipeline executed successfully!" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <limits>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Pipeline 9: Automatic Mixed-Precision (AMP)       " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load AMP Fallback Plugin (.so)
        if (!plugin_mgr.load_plugin("/usr/local/lib/libplugin_amp_fallback.so")) {
            throw std::runtime_error("Failed to load libplugin_amp_fallback.so");
        }

        // 2. Load SqueezeNet 1.1 ONNX Model
        std::string model_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/squeezenet/model/squeezenet1.1-7.onnx";
        std::cout << "\n[ModelHub] Loading model via HTTPS..." << std::endl;
        engine.load_model(model_url);

        xinfer::Tensor& model_input = engine.get_input_tensor("data");
        std::cout << "Model Input Tensor Shape : " << model_input.shape_string() << std::endl;

        const size_t total_elements = model_input.element_count();

        // 3. Construct Synthetic Tensor with Numerical Boundary Values
        std::vector<float> input_data(total_elements, 0.5f);
        // Inject extreme float values to test numerical stability
        input_data[10] = 65504.0f; // FP16 max boundary
        input_data[20] = -65504.0f;
        input_data[30] = 1e-7f;   // FP16 subnormal threshold

        std::cout << "[Dummy Data] Generated tensor with FP16 boundary values (1e-7 to 65504.0)." << std::endl;

        // Backed Input Tensor
        xinfer::Tensor raw_tensor("raw_input", {1, 3, 224, 224}, xinfer::DataType::Float32, input_data.data());

        // Backed Output Tensor
        std::vector<float> sanitized_buffer(total_elements, 0.0f);
        xinfer::Tensor sanitized_tensor("sanitized", {1, 3, 224, 224}, xinfer::DataType::Float32, sanitized_buffer.data());

        // 4. Run AMP Fallback Plugin (Sanitizes & checks numerical stability)
        std::cout << "\n[Stage 1: AMP Plugin] Evaluating tensor numerical stability..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, raw_tensor, sanitized_tensor);

        // 5. Copy Verified Data to Model Input
        model_input.copy_from_host(raw_tensor.data<float>(), model_input.get_size_in_bytes());

        // 6. Run Model Inference
        std::cout << "[Stage 2: Inference Engine] Running inference on OpenVINO..." << std::endl;
        engine.infer();

        // 7. Inspect Outputs
        xinfer::Tensor& output = engine.get_output_tensor("squeezenet0_flatten0_reshape0");
        const float* logits = output.data<float>();

        bool output_valid = true;
        for (size_t i = 0; i < output.element_count(); ++i) {
            if (std::isnan(logits[i]) || std::isinf(logits[i])) {
                output_valid = false;
                break;
            }
        }

        std::cout << "\n----------------------------------------------------------" << std::endl;
        std::cout << "Pipeline Output:" << std::endl;
        std::cout << "Numerical Precision Maintained: " << (output_valid ? "STABLE (No NaN/Inf)" : "UNSTABLE") << std::endl;
        std::cout << "Top Prediction Logit Value    : " << std::fixed << std::setprecision(4) << logits[0] << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
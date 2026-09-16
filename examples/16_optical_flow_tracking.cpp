#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Pipeline 5: Video Optical Flow Motion Tracker     " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        // 1. Initialize Engine & Plugin Manager
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 2. Load Optical Flow Preprocessor Plugin (.so)
        if (!plugin_mgr.load_plugin("/usr/local/lib/libplugin_optical_flow.so")) {
            throw std::runtime_error("Failed to load libplugin_optical_flow.so");
        }

        // 3. Direct HTTPS URL from Official ONNX Model Zoo (Replaces local file path)
        std::string model_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/squeezenet/model/squeezenet1.1-7.onnx";
        
        std::cout << "\n[ModelHub] Loading model via HTTPS URL from ONNX Model Zoo..." << std::endl;
        // ModelHub auto-downloads squeezenet1.1-7.onnx into models/ if missing locally
        engine.load_model(model_url);

        // 4. Query Input Tensor Shape: [1, 3, 224, 224]
        xinfer::Tensor& model_input = engine.get_input_tensor("data");
        std::cout << "Model Input Tensor Shape : " << model_input.shape_string() << std::endl;

        const int width = 224;
        const int height = 224;
        const size_t total_elements = model_input.element_count(); // 1 * 3 * 224 * 224 = 150528

        // 5. Construct Synthetic Consecutive Video Frames (T0 and T1)
        std::vector<float> frame_t0(total_elements, 0.05f); // Static dark background
        std::vector<float> frame_t1(total_elements, 0.05f);

        // Frame T0: Target object stationary at coordinate (30, 30)
        for (int c = 0; c < 3; ++c) {
            for (int y = 30; y < 60; ++y) {
                for (int x = 30; x < 60; ++x) {
                    size_t idx = c * (width * height) + (y * width + x);
                    frame_t0[idx] = 0.95f; // Bright target object
                }
            }
        }

        // Frame T1: Target object rapidly displaced to coordinate (130, 130)
        for (int c = 0; c < 3; ++c) {
            for (int y = 130; y < 160; ++y) {
                for (int x = 130; x < 160; ++x) {
                    size_t idx = c * (width * height) + (y * width + x);
                    frame_t1[idx] = 0.95f; // Target in new position
                }
            }
        }
        std::cout << "[Dummy Data] Generated consecutive 224x224 video frames simulating target displacement." << std::endl;

        xinfer::Tensor tensor_t0("frame_t0", {1, 3, height, width}, xinfer::DataType::Float32, frame_t0.data());
        xinfer::Tensor tensor_t1("frame_t1", {1, 3, height, width}, xinfer::DataType::Float32, frame_t1.data());
        
        // Tensor to hold the computed optical flow motion displacement map
        xinfer::Tensor motion_map("motion_map", {1, 3, height, width}, xinfer::DataType::Float32);

        // 6. Execute Optical Flow Plugin on Frame T0 (Establishes baseline)
        std::cout << "\n[Stage 1: Optical Flow Plugin] Ingesting Video Frame T0 (Baseline)..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, tensor_t0, motion_map);

        // 7. Execute Optical Flow Plugin on Frame T1 (Computes dense pixel displacement)
        std::cout << "[Stage 1: Optical Flow Plugin] Ingesting Video Frame T1 (Computing Motion Displacement)..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, tensor_t1, motion_map);

        // 8. Copy the computed motion map into the model's input buffer
        model_input.copy_from_host(motion_map.data<float>(), model_input.get_size_in_bytes());

        // 9. Execute Model Inference on OpenVINO
        std::cout << "\n[Stage 2: Inference Engine] Evaluating Motion Map on OpenVINO..." << std::endl;
        engine.infer();

        // 10. Inspect Output Predictions
        xinfer::Tensor& output = engine.get_output_tensor("squeezenet0_flatten0_reshape0");
        const float* logits = output.data<float>();

        // Calculate peak motion activation response across output classes
        float max_activation = 0.0f;
        for (size_t i = 0; i < output.element_count(); ++i) {
            if (std::abs(logits[i]) > max_activation) {
                max_activation = std::abs(logits[i]);
            }
        }

        std::cout << "\n----------------------------------------------------------" << std::endl;
        std::cout << "Pipeline Output:" << std::endl;
        std::cout << "Target Displaced Across Frames : (30,30) -> (130,130)" << std::endl;
        std::cout << "Dense Motion Pixels Calculated : " << total_elements / 3 << " pixels" << std::endl;
        std::cout << "Peak Motion Activation Response: " << std::fixed << std::setprecision(4) << max_activation << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
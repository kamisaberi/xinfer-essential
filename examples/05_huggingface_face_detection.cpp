#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Essential: ONNX Model Zoo UltraFace Demo          " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        // 1. Initialize OpenVINO Engine (Runs on CPU/NPU inside VMware)
        xinfer::Engine engine(xinfer::Target::OpenVINO);

        // 2. Direct Raw Binary ONNX Model URL from Official ONNX Model Zoo
        // UltraFace RFB-320 Face Detector (1.27 MB binary ONNX)
        std::string direct_raw_onnx_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/body_analysis/ultraface/models/version-RFB-320.onnx";

        std::cout << "Requesting raw ONNX binary from ONNX Model Zoo...\n" << std::endl;

        // 3. Load Model (xinfer::ModelHub auto-downloads the binary over HTTPS)
        engine.load_model(direct_raw_onnx_url);

        // 4. Inspect Model Input & Output Tensor Shapes
        xinfer::Tensor& input          = engine.get_input_tensor("input");
        xinfer::Tensor& output_scores  = engine.get_output_tensor("scores");
        xinfer::Tensor& output_boxes   = engine.get_output_tensor("boxes");

        std::cout << "\nONNX Model Loaded Successfully!" << std::endl;
        std::cout << "Input Tensor Shape   : " << input.shape_string() << std::endl;
        std::cout << "Scores Output Shape  : " << output_scores.shape_string() << std::endl;
        std::cout << "Boxes Output Shape   : " << output_boxes.shape_string() << std::endl;

        // 5. Prepare Dummy 320x240 RGB Pixel Data
        size_t input_elements = input.element_count();
        std::vector<float> face_pixels(input_elements, 0.5f);

        input.copy_from_host(face_pixels.data(), input.get_size_in_bytes());

        // 6. Execute ONNX Inference
        std::cout << "\nExecuting UltraFace ONNX Inference..." << std::endl;
        engine.infer();

        // 7. Extract Bounding Box Results
        const float* scores_ptr = output_scores.data<float>();
        const float* boxes_ptr  = output_boxes.data<float>();

        std::cout << "\nInference Completed!" << std::endl;
        std::cout << "Detected Face Confidence Score: " << scores_ptr[1] << std::endl;
        std::cout << "Detected Bounding Box [x1, y1, x2, y2]: [" 
                  << boxes_ptr[0] << ", " << boxes_ptr[1] << ", "
                  << boxes_ptr[2] << ", " << boxes_ptr[3] << "]" << std::endl;

        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "xInfer Runtime Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
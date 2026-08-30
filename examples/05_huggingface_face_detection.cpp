#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Essential: Hugging Face ONNX Face Model Demo     " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        // 1. Initialize Engine for Intel OpenVINO (Runs on CPU/NPU inside Ubuntu VM)
        xinfer::Engine engine(xinfer::Target::OpenVINO);

        // 2. Direct Official Hugging Face ONNX Model URL
        // Model: UltraFace Slim 320x240 Lightweight Face Detector
        std::string huggingface_onnx_url = 
            "https://huggingface.co/onnx-community/ultraface-slim-320-onnx/resolve/main/model.onnx";

        std::cout << "Requesting model from Hugging Face Hub...\n" << std::endl;

        // 3. Load Model!
        // What happens under the hood:
        // - xinfer checks if "models/model.onnx" exists locally.
        // - If missing, xinfer::ModelHub automatically downloads it via HTTPS.
        // - OpenVINO compiles the ONNX graph directly in memory.
        engine.load_model(huggingface_onnx_url);

        // 4. Query Model Input & Output Tensor Information
        xinfer::Tensor& input          = engine.get_input_tensor("input");
        xinfer::Tensor& output_scores  = engine.get_output_tensor("scores");
        xinfer::Tensor& output_boxes   = engine.get_output_tensor("boxes");

        std::cout << "ONNX Model Loaded Successfully!" << std::endl;
        std::cout << "Input Tensor Shape   : " << input.shape_string() << std::endl;
        std::cout << "Scores Output Shape  : " << output_scores.shape_string() << std::endl;
        std::cout << "Boxes Output Shape   : " << output_boxes.shape_string() << std::endl;

        // 5. Prepare Dummy 320x240 RGB Face Image Data
        size_t input_elements = input.element_count();
        std::vector<float> face_image_pixels(input_elements, 0.5f); // Normalized RGB float values

        // Copy pixel data into input tensor memory buffer
        input.copy_from_host(face_image_pixels.data(), input.get_size_in_bytes());

        // 6. Run ONNX Model Inference on CPU/NPU
        std::cout << "\nExecuting Face Detection Inference..." << std::endl;
        engine.infer();

        // 7. Access Results
        const float* scores_ptr = output_scores.data<float>();
        const float* boxes_ptr  = output_boxes.data<float>();

        std::cout << "\nInference Completed!" << std::endl;
        std::cout << "Face Detection Confidence Score: " << scores_ptr[1] << std::endl;
        std::cout << "Top Detected Face Box [x1, y1, x2, y2]: [" 
                  << boxes_ptr[0] << ", " << boxes_ptr[1] << ", "
                  << boxes_ptr[2] << ", " << boxes_ptr[3] << "]" << std::endl;

        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "xInfer Runtime Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
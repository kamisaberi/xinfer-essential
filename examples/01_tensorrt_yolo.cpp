#include <iostream>
#include <vector>
#include "xinfer/xinfer.hpp"
#include "xinfer/utils/preproc.hpp"
#include "xinfer/utils/postproc.hpp"

int main() {
    std::cout << "Running Example 1: NVIDIA TensorRT YOLO Detector" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::TensorRT);
        engine.load_model("models/yolov8n.engine");

        xinfer::Tensor& input = engine.get_input_tensor("images");
        xinfer::Tensor& output = engine.get_output_tensor("output0");

        // Simulate 640x640 BGR Camera Frame
        std::vector<uint8_t> dummy_frame(640 * 640 * 3, 128);
        
        // Zero-Copy Host Pinned Memory Buffer
        size_t input_bytes = input.get_size_in_bytes();
        float* pinned_input = static_cast<float*>(xinfer::Memory::allocate_pinned(input_bytes));

        xinfer::utils::cpu_letterbox_normalize(dummy_frame.data(), 640, 640, pinned_input, 640, 640);
        input.copy_from_host(pinned_input, input_bytes);

        // Execute TensorRT Inference
        engine.infer();

        // Decode Bounding Boxes
        auto boxes = xinfer::utils::non_max_suppression(output.data<float>(), 8400, 80);
        std::cout << "Detected Objects: " << boxes.size() << std::endl;

        xinfer::Memory::free_pinned(pinned_input);

    } catch (const std::exception& e) {
        std::cerr << "TensorRT Example Error: " << e.what() << std::endl;
    }

    return 0;
}
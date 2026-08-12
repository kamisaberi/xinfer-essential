#include <iostream>
#include "xinfer/xinfer.hpp"

int main() {
    std::cout << "Running Example 3: Rockchip RK3588 ARM NPU Inference" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::RKNN);
        engine.load_model("models/yolov8_rk3588.rknn");

        xinfer::Tensor& input = engine.get_input_tensor("images");
        
        // Direct uint8 RGB Camera Buffer write
        std::vector<uint8_t> rgb_camera_frame(640 * 640 * 3, 200);
        std::memcpy(input.data_raw(), rgb_camera_frame.data(), rgb_camera_frame.size());

        // Run hardware execution on RKNPU2
        engine.infer();
        std::cout << "RKNN Inference successfully executed on ARM NPU." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "RKNN Example Error: " << e.what() << std::endl;
    }

    return 0;
}
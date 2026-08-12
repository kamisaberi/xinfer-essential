#include <iostream>
#include "xinfer/xinfer.hpp"

int main() {
    std::cout << "Starting xInfer Essential Demo..." << std::endl;

    try {
        // 1. Initialize Engine for NVIDIA TensorRT
        xinfer::Engine engine(xinfer::Target::TensorRT);
        std::cout << "Engine initialized for: " << xinfer::target_to_string(engine.target()) << std::endl;

        // 2. Load pre-compiled TensorRT Engine
        // engine.load_model("yolov8n.engine");

        // 3. Allocate Zero-Copy Pinned Input Buffer
        size_t sample_size = 1 * 3 * 640 * 640 * sizeof(float);
        void* host_buffer = xinfer::Memory::allocate_pinned(sample_size);
        std::cout << "Allocated " << sample_size << " bytes of pinned Host DMA memory." << std::endl;

        // Cleanup
        xinfer::Memory::free_pinned(host_buffer);
        std::cout << "xInfer Essential execution completed successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Runtime Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
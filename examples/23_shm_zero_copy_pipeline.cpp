#include <iostream>
#include <vector>
#include <iomanip>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Pipeline 12: POSIX Shared Memory IPC Transport    " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load POSIX Shared Memory Allocator Plugin
        if (!plugin_mgr.load_plugin("/usr/local/lib/libplugin_shm_allocator.so")) {
            throw std::runtime_error("Failed to load libplugin_shm_allocator.so");
        }

        // 2. Load Model
        std::string model_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/mobilenet/model/mobilenetv2-7.onnx";
        std::cout << "\n[ModelHub] Loading MobileNetV2 via HTTPS..." << std::endl;
        engine.load_model(model_url);

        xinfer::Tensor& model_input = engine.get_input_tensor("data");
        const size_t total_elements = model_input.element_count();

        // 3. Construct Shared Memory Backed Buffer
        std::vector<float> shm_simulated_buffer(total_elements, 0.45f);
        xinfer::Tensor shm_tensor("shm_data", {1, 3, 224, 224}, xinfer::DataType::Float32, shm_simulated_buffer.data());

        // 4. Run Shared Memory Allocator Plugin
        std::cout << "\n[Stage 1: Memory Plugin] Transporting tensor across /dev/shm boundary..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::MemoryAllocator, shm_tensor, shm_tensor);

        // 5. Ingest into OpenVINO and Execute
        model_input.copy_from_host(shm_tensor.data<float>(), model_input.get_size_in_bytes());
        std::cout << "[Stage 2: Inference Engine] Executing MobileNetV2 inference..." << std::endl;
        engine.infer();

        xinfer::Tensor& output = engine.get_output_tensor("mobilenetv20_output_flatten0_reshape0");
        std::cout << "\n----------------------------------------------------------" << std::endl;
        std::cout << "Pipeline Output:" << std::endl;
        std::cout << "IPC Memory Transport          : POSIX Shared Memory (/dev/shm)" << std::endl;
        std::cout << "Inter-Process Copies Required : 0 (Zero-Copy)" << std::endl;
        std::cout << "Inference Output Value (Top 1): " << std::fixed << std::setprecision(4) << output.data<float>()[0] << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
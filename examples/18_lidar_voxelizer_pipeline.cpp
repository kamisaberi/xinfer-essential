#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Pipeline 7: 3D LiDAR Voxelization & Spatial AI    " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load LiDAR Voxelizer Preprocessor Plugin (.so)
        if (!plugin_mgr.load_plugin("/usr/local/lib/libplugin_lidar_voxelizer.so")) {
            throw std::runtime_error("Failed to load libplugin_lidar_voxelizer.so");
        }

        // 2. Load Model Backbone
        std::string model_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/squeezenet/model/squeezenet1.1-7.onnx";
        
        std::cout << "\n[ModelHub] Loading 3D spatial evaluation model via HTTPS..." << std::endl;
        engine.load_model(model_url);

        xinfer::Tensor& model_input = engine.get_input_tensor("data");
        std::cout << "Model Input Tensor Shape : " << model_input.shape_string() << std::endl;

        const size_t voxel_tensor_elements = model_input.element_count(); // 150528 floats

        // 3. Construct 2,000 Raw 3D LiDAR Points (x, y, z, intensity)
        const size_t num_points = 2000;
        std::vector<float> raw_lidar_points(num_points * 4);

        for (size_t i = 0; i < num_points; ++i) {
            if (i < 1500) {
                // Ground plane points
                raw_lidar_points[i * 4 + 0] = static_cast<float>(i % 50) * 0.2f - 5.0f; // x
                raw_lidar_points[i * 4 + 1] = static_cast<float>(i / 50) * 0.2f;        // y
                raw_lidar_points[i * 4 + 2] = 0.0f;                                     // z (ground)
                raw_lidar_points[i * 4 + 3] = 0.2f;                                     // intensity
            } else {
                // Detected 3D obstacle cluster (e.g., unauthorized vehicle or drone at x=2.0, y=8.0, z=1.5)
                raw_lidar_points[i * 4 + 0] = 2.0f + static_cast<float>(i % 10) * 0.05f;
                raw_lidar_points[i * 4 + 1] = 8.0f + static_cast<float>((i / 10) % 10) * 0.05f;
                raw_lidar_points[i * 4 + 2] = 1.5f + static_cast<float>(i % 5) * 0.1f;
                raw_lidar_points[i * 4 + 3] = 0.95f; // High reflection intensity
            }
        }
        std::cout << "[Dummy Data] Generated 2,000 3D LiDAR points (1,500 ground + 500 obstacle reflection cluster)." << std::endl;

        // Backed Input Tensor
        xinfer::Tensor lidar_input_tensor("lidar_raw", {1, 1, static_cast<int64_t>(num_points), 4}, 
                                          xinfer::DataType::Float32, raw_lidar_points.data());

        // Backed Voxel Grid Tensor
        std::vector<float> voxel_buffer(voxel_tensor_elements, 0.0f);
        xinfer::Tensor voxel_grid_tensor("voxel_grid", {1, 3, 224, 224}, 
                                         xinfer::DataType::Float32, voxel_buffer.data());

        // 4. Run LiDAR Voxelizer Plugin
        std::cout << "\n[Stage 1: Preprocessor Plugin] Discretizing 3D point cloud into 224x224 voxel grid..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, lidar_input_tensor, voxel_grid_tensor);

        // 5. Copy Voxel Grid to Model Buffer
        model_input.copy_from_host(voxel_grid_tensor.data<float>(), model_input.get_size_in_bytes());

        // 6. Execute Model Inference
        std::cout << "[Stage 2: Inference Engine] Running 3D Voxel Perception on OpenVINO..." << std::endl;
        engine.infer();

        // 7. Inspect Predictions
        xinfer::Tensor& output = engine.get_output_tensor("squeezenet0_flatten0_reshape0");
        const float* logits = output.data<float>();

        float peak_spatial_response = 0.0f;
        for (size_t i = 0; i < output.element_count(); ++i) {
            if (std::abs(logits[i]) > peak_spatial_response) {
                peak_spatial_response = std::abs(logits[i]);
            }
        }

        std::cout << "\n----------------------------------------------------------" << std::endl;
        std::cout << "Pipeline Output:" << std::endl;
        std::cout << "Raw 3D Points Ingested        : " << num_points << " points" << std::endl;
        std::cout << "Structured Voxels Discretized : 224 x 224 x 3 cells" << std::endl;
        std::cout << "Peak 3D Spatial Response Score: " << std::fixed << std::setprecision(4) << peak_spatial_response << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
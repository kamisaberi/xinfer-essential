#include "xinfer/plugin.hpp"
#include <iostream>
#include <vector>
#include <cmath>

class PointCloudLiDARVoxelizerPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "PointCloud_LiDAR_Voxelizer"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        voxel_size_ = 0.2f; // 20cm x 20cm x 20cm 3D voxel grid
        std::cout << "[xInfer Plugin: Preproc] PointCloud 3D LiDAR Voxelizer Initialized (Voxel Size: " 
                  << voxel_size_ << "m)." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        const float* raw_points = input.data<float>(); // (x, y, z, intensity)
        size_t point_count = input.element_count() / 4;

        float* voxel_grid = output.data<float>();
        size_t voxel_count = output.element_count();

        if (!raw_points || !voxel_grid) return false;

        // Voxelize 3D point cloud into structured grid tensor
        for (size_t i = 0; i < point_count; ++i) {
            float x = raw_points[i * 4 + 0];
            float y = raw_points[i * 4 + 1];
            float z = raw_points[i * 4 + 2];
            float intensity = raw_points[i * 4 + 3];

            size_t vx = static_cast<size_t>(std::abs(x / voxel_size_));
            size_t vy = static_cast<size_t>(std::abs(y / voxel_size_));
            size_t vz = static_cast<size_t>(std::abs(z / voxel_size_));

            size_t idx = (vx + vy * 32 + vz * 32 * 32) % voxel_count;
            voxel_grid[idx] = intensity;
        }

        std::cout << "[xInfer Plugin: Preproc] Voxelized " << point_count 
                  << " raw 3D LiDAR points into 3D voxel grid tensor." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Preproc] PointCloud 3D LiDAR Voxelizer shut down." << std::endl;
    }

private:
    float voxel_size_{0.2f};
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new PointCloudLiDARVoxelizerPlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
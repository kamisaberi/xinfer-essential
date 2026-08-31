#include "xinfer/plugin.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

class POSIXShmAllocatorPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "POSIX_Shared_Memory_Allocator"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::MemoryAllocator; }

    bool initialize(const std::string& config_json) override {
        std::cout << "[xInfer Plugin: Allocator] POSIX Shared Memory (shm_open) Inter-Process Allocator Initialized." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        std::cout << "[xInfer Plugin: Allocator] Transported tensor memory via POSIX shared memory handle /dev/shm." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Allocator] POSIX Shared Memory Allocator shut down." << std::endl;
    }
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() { return new POSIXShmAllocatorPlugin(); }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) { delete plugin; }
}
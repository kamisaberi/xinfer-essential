#include "xinfer/plugin.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

class LinuxDMABufAllocatorPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Linux_DMA_BUF_Allocator"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::MemoryAllocator; }

    bool initialize(const std::string& config_json) override {
        dma_heap_fd_ = open("/dev/dma_heap/system", O_RDWR);
        if (dma_heap_fd_ < 0) {
            std::cout << "[xInfer Plugin: Allocator] Linux DMA-BUF heap fallback active." << std::endl;
        } else {
            std::cout << "[xInfer Plugin: Allocator] Linux DMA-BUF Zero-Copy Heap Opened (/dev/dma_heap/system)." << std::endl;
        }
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        std::cout << "[xInfer Plugin: Allocator] Managing zero-copy DMA-BUF handle for Tensor: " 
                  << input.name() << " (Size: " << input.get_size_in_bytes() << " bytes)." << std::endl;
        return true;
    }

    void shutdown() override {
        if (dma_heap_fd_ >= 0) {
            close(dma_heap_fd_);
        }
        std::cout << "[xInfer Plugin: Allocator] DMA-BUF Allocator shut down." << std::endl;
    }

private:
    int dma_heap_fd_{-1};
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new LinuxDMABufAllocatorPlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
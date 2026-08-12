#include "xinfer/memory.hpp"
#include <cstdlib>
#include <stdexcept>

#ifdef XINFER_ENABLE_TENSORRT
#include <cuda_runtime.h>
#endif

namespace xinfer {

void* Memory::allocate_pinned(size_t bytes) {
#ifdef XINFER_ENABLE_TENSORRT
    void* ptr = nullptr;
    if (cudaMallocHost(&ptr, bytes) == cudaSuccess) {
        return ptr;
    }
#endif
    return std::malloc(bytes);
}

void Memory::free_pinned(void* ptr) {
    if (!ptr) return;
#ifdef XINFER_ENABLE_TENSORRT
    cudaFreeHost(ptr);
#else
    std::free(ptr);
#endif
}

void* Memory::allocate_device(size_t bytes) {
#ifdef XINFER_ENABLE_TENSORRT
    void* ptr = nullptr;
    if (cudaMalloc(&ptr, bytes) == cudaSuccess) {
        return ptr;
    }
    throw std::runtime_error("CUDA Device allocation failed!");
#else
    return std::malloc(bytes);
#endif
}

void Memory::free_device(void* ptr) {
    if (!ptr) return;
#ifdef XINFER_ENABLE_TENSORRT
    cudaFree(ptr);
#else
    std::free(ptr);
#endif
}

void Memory::copy_host_to_device(void* dst_device, const void* src_host, size_t bytes, void* stream) {
#ifdef XINFER_ENABLE_TENSORRT
    cudaStream_t cuda_stream = stream ? static_cast<cudaStream_t>(stream) : 0;
    cudaMemcpyAsync(dst_device, src_host, bytes, cudaMemcpyHostToDevice, cuda_stream);
#else
    std::memcpy(dst_device, src_host, bytes);
#endif
}

void Memory::copy_device_to_host(void* dst_host, const void* src_device, size_t bytes, void* stream) {
#ifdef XINFER_ENABLE_TENSORRT
    cudaStream_t cuda_stream = stream ? static_cast<cudaStream_t>(stream) : 0;
    cudaMemcpyAsync(dst_host, src_device, bytes, cudaMemcpyDeviceToHost, cuda_stream);
#else
    std::memcpy(dst_host, src_device, bytes);
#endif
}

} // namespace xinfer
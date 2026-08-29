#pragma once
#include <cstddef>
#include "export.hpp"  // <--- ADD THIS LINE (Defines XINFER_API)

namespace xinfer {

class XINFER_API  Memory {
public:
    static void* allocate_pinned(size_t bytes);
    static void free_pinned(void* ptr);

    static void* allocate_device(size_t bytes);
    static void free_device(void* ptr);

    static void copy_host_to_device(void* dst_device, const void* src_host, size_t bytes, void* stream = nullptr);
    static void copy_device_to_host(void* dst_host, const void* src_device, size_t bytes, void* stream = nullptr);
};

} // namespace xinfer
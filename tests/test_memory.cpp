#include <iostream>
#include <cassert>
#include "xinfer/memory.hpp"

void test_pinned_memory() {
    size_t alloc_bytes = 1024 * 1024 * 4; // 4MB
    void* ptr = xinfer::Memory::allocate_pinned(alloc_bytes);
    assert(ptr != nullptr);
    xinfer::Memory::free_pinned(ptr);
    std::cout << "[PASS] Zero-copy pinned memory allocation test passed." << std::endl;
}

int main() {
    test_pinned_memory();
    return 0;
}
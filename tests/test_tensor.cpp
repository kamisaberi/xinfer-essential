#include <iostream>
#include <cassert>
#include "xinfer/tensor.hpp"

void test_tensor_creation() {
    xinfer::Tensor tensor("test_tensor", {1, 3, 224, 224}, xinfer::DataType::Float32);
    assert(tensor.element_count() == 150528);
    assert(tensor.get_size_in_bytes() == 150528 * 4);
    assert(tensor.shape_string() == "[1, 3, 224, 224]");
    std::cout << "[PASS] Tensor creation & byte size test passed." << std::endl;
}

int main() {
    test_tensor_creation();
    return 0;
}
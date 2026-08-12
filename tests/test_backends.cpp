#include <iostream>
#include "xinfer/engine.hpp"

void test_backend_instantiation() {
#ifdef XINFER_ENABLE_TENSORRT
    xinfer::Engine trt_engine(xinfer::Target::TensorRT);
    std::cout << "[PASS] TensorRT Engine initialized." << std::endl;
#endif

#ifdef XINFER_ENABLE_OPENVINO
    xinfer::Engine ov_engine(xinfer::Target::OpenVINO);
    std::cout << "[PASS] OpenVINO Engine initialized." << std::endl;
#endif

#ifdef XINFER_ENABLE_RKNN
    xinfer::Engine rknn_engine(xinfer::Target::RKNN);
    std::cout << "[PASS] RKNN Engine initialized." << std::endl;
#endif
}

int main() {
    test_backend_instantiation();
    return 0;
}
#pragma once
#include <string>

namespace xinfer {

enum class Target {
    TensorRT,
    OpenVINO,
    RKNN
};

enum class DataType {
    Float32,
    Float16,
    Int32,
    Int8,
    UInt8
};

inline std::string target_to_string(Target target) {
    switch (target) {
        case Target::TensorRT: return "NVIDIA TensorRT";
        case Target::OpenVINO: return "Intel OpenVINO";
        case Target::RKNN:     return "Rockchip RKNN";
        default:               return "Unknown Target";
    }
}

inline size_t data_type_size(DataType type) {
    switch (type) {
        case DataType::Float32: return 4;
        case DataType::Float16: return 2;
        case DataType::Int32:   return 4;
        case DataType::Int8:    return 1;
        case DataType::UInt8:   return 1;
        default:                return 0;
    }
}

} // namespace xinfer
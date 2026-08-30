#pragma once

#include "export.hpp"
#include <string>

namespace xinfer {

enum class XINFER_API Target {
    TensorRT,    // NVIDIA (RTX GPUs, Jetson Orin) [.engine]
    OpenVINO,    // Intel (Core Ultra NPU, Arc GPU, x86 CPU) [.xml/.bin]
    RKNN,        // Rockchip (RK3588, RK3568, RV1126) [.rknn]
    VitisAI,     // AMD / Xilinx (Kria SOM, Zynq MPSoC, Versal) [.xmodel]
    QNN,         // Qualcomm (Snapdragon 8 Gen 2/3, HTP) [.bin/.so]
    CoreML,      // Apple (M1/M2/M3, A-Series) [.mlmodelc]
    RyzenAI,     // AMD (Ryzen 7040/8040 IPU) [.onnx]
    NeuroPilot,  // MediaTek (Genio 1200, Dimensity) [.dla/.pte]
    Hailo,       // Hailo (Hailo-8 / 8L / 10) [.hef]
    CVFlow,      // Ambarella (CV2, CV3, CV5) [.cavalry]
    ENN,         // Samsung Exynos (Eden NPU) [.nnc]
    EdgeTPU,     // Google Coral (USB, M.2, Dev Board) [.tflite]
    FPGA_AI,     // Intel FPGA (Agilex, Stratix 10) [.aocx]
    VectorBlox,  // Microchip (PolarFire FPGA) [.blob]
    SensAI       // Lattice (CrossLink-NX) [.bin]
};

enum class XINFER_API DataType {
    Float32,
    Float16,
    Int32,
    Int8,
    UInt8
};

inline std::string target_to_string(Target target) {
    switch (target) {
        case Target::TensorRT:   return "NVIDIA TensorRT";
        case Target::OpenVINO:   return "Intel OpenVINO";
        case Target::RKNN:       return "Rockchip RKNN";
        case Target::VitisAI:    return "AMD/Xilinx Vitis AI";
        case Target::QNN:        return "Qualcomm QNN";
        case Target::CoreML:     return "Apple CoreML/Metal";
        case Target::RyzenAI:    return "AMD Ryzen AI";
        case Target::NeuroPilot: return "MediaTek NeuroPilot";
        case Target::Hailo:      return "Hailo HailoRT";
        case Target::CVFlow:     return "Ambarella CVFlow";
        case Target::ENN:        return "Samsung ENN";
        case Target::EdgeTPU:    return "Google Coral Edge TPU";
        case Target::FPGA_AI:    return "Intel FPGA AI Suite";
        case Target::VectorBlox: return "Microchip VectorBlox";
        case Target::SensAI:     return "Lattice sensAI";
        default:                 return "Unknown Target";
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
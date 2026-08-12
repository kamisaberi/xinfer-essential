#pragma once
#include <vector>
#include <cstdint>

namespace xinfer::utils {

struct BoundingBox {
    float x1, y1, x2, y2;
    float confidence;
    int class_id;
};

void cpu_letterbox_normalize(
    const uint8_t* src_bgr, int src_w, int src_h,
    float* dst_float, int target_w, int target_h
);

} // namespace xinfer::utils
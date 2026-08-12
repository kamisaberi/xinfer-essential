#pragma once
#include "preproc.hpp"
#include <vector>

namespace xinfer::utils {

std::vector<BoundingBox> non_max_suppression(
    const float* output_data, int num_boxes, int num_classes,
    float conf_threshold = 0.25f, float iou_threshold = 0.45f
);

} // namespace xinfer::utils
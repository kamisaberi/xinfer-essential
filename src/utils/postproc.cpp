#include "xinfer/utils/postproc.hpp"
#include <algorithm>
#include <cmath>

namespace xinfer::utils {

static float calculate_iou(const BoundingBox& a, const BoundingBox& b) {
    float inter_x1 = std::max(a.x1, b.x1);
    float inter_y1 = std::max(a.y1, b.y1);
    float inter_x2 = std::min(a.x2, b.x2);
    float inter_y2 = std::min(a.y2, b.y2);

    float inter_area = std::max(0.0f, inter_x2 - inter_x1) * std::max(0.0f, inter_y2 - inter_y1);
    float area_a = (a.x2 - a.x1) * (a.y2 - a.y1);
    float area_b = (b.x2 - b.x1) * (b.y2 - b.y1);

    return inter_area / (area_a + area_b - inter_area + 1e-6f);
}

std::vector<BoundingBox> non_max_suppression(
    const float* output_data, int num_boxes, int num_classes,
    float conf_threshold, float iou_threshold
) {
    std::vector<BoundingBox> candidate_boxes;

    for (int i = 0; i < num_boxes; ++i) {
        const float* ptr = output_data + i * (5 + num_classes);
        float objectness = ptr[4];

        if (objectness < conf_threshold) continue;

        int best_class = 0;
        float max_class_score = 0.0f;
        for (int c = 0; c < num_classes; ++c) {
            if (ptr[5 + c] > max_class_score) {
                max_class_score = ptr[5 + c];
                best_class = c;
            }
        }

        float final_score = objectness * max_class_score;
        if (final_score > conf_threshold) {
            float cx = ptr[0], cy = ptr[1], w = ptr[2], h = ptr[3];
            candidate_boxes.push_back({
                cx - w / 2.0f, cy - h / 2.0f, cx + w / 2.0f, cy + h / 2.0f,
                final_score, best_class
            });
        }
    }

    std::sort(candidate_boxes.begin(), candidate_boxes.end(), [](const BoundingBox& a, const BoundingBox& b) {
        return a.confidence > b.confidence;
    });

    std::vector<BoundingBox> final_boxes;
    std::vector<bool> suppressed(candidate_boxes.size(), false);

    for (size_t i = 0; i < candidate_boxes.size(); ++i) {
        if (suppressed[i]) continue;
        final_boxes.push_back(candidate_boxes[i]);

        for (size_t j = i + 1; j < candidate_boxes.size(); ++j) {
            if (suppressed[j]) continue;
            if (calculate_iou(candidate_boxes[i], candidate_boxes[j]) > iou_threshold) {
                suppressed[j] = true;
            }
        }
    }

    return final_boxes;
}

} // namespace xinfer::utils
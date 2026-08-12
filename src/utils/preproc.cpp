#include "xinfer/utils/preproc.hpp"
#include <algorithm>

namespace xinfer::utils {

void cpu_letterbox_normalize(
    const uint8_t* src_bgr, int src_w, int src_h,
    float* dst_float, int target_w, int target_h
) {
    float scale = std::min(static_cast<float>(target_w) / src_w, static_cast<float>(target_h) / src_h);
    int new_w = static_cast<int>(src_w * scale);
    int new_h = static_cast<int>(src_h * scale);

    int pad_x = (target_w - new_w) / 2;
    int pad_y = (target_h - new_h) / 2;

    // Fill background with 114 (gray letterboxing)
    std::fill(dst_float, dst_float + (target_w * target_h * 3), 114.0f / 255.0f);

    for (int y = 0; y < new_h; ++y) {
        for (int x = 0; x < new_w; ++x) {
            int src_x = static_cast<int>(x / scale);
            int src_y = static_cast<int>(y / scale);

            int src_idx = (src_y * src_w + src_x) * 3;
            int dst_x = x + pad_x;
            int dst_y = y + pad_y;

            // BGR to RGB Normalization
            int r_idx = 0 * target_w * target_h + (dst_y * target_w + dst_x);
            int g_idx = 1 * target_w * target_h + (dst_y * target_w + dst_x);
            int b_idx = 2 * target_w * target_h + (dst_y * target_w + dst_x);

            dst_float[r_idx] = src_bgr[src_idx + 2] / 255.0f;
            dst_float[g_idx] = src_bgr[src_idx + 1] / 255.0f;
            dst_float[b_idx] = src_bgr[src_idx + 0] / 255.0f;
        }
    }
}

} // namespace xinfer::utils
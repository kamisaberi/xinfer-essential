#pragma once

#include "export.hpp"
#include <string>

namespace xinfer {

class XINFER_API ModelHub {
public:
    // Checks local cache. If missing, downloads ONNX model from official repo URL.
    static std::string fetch_model(const std::string& model_name_or_url, const std::string& cache_dir = "models");

    // Downloads file over HTTPS to destination path
    static bool download_file(const std::string& url, const std::string& destination_path);

    // Helper: Checks if file exists on disk
    static bool file_exists(const std::string& path);

    // Resolves official short aliases (e.g. "yolov8n.onnx") to full repository URLs
    static std::string resolve_alias_url(const std::string& model_name);
};

} // namespace xinfer
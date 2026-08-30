#include "xinfer/hub.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace xinfer {

bool ModelHub::file_exists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string ModelHub::resolve_alias_url(const std::string& model_name) {
    // Check if it's already a full URL
    if (model_name.rfind("http://", 0) == 0 || model_name.rfind("https://", 0) == 0) {
        return model_name;
    }

    // Official Repository Aliases (Map short names to official GitHub releases or model zoo)
    if (model_name == "threat_detector.onnx") {
        return "https://github.com/kamisaberi/blackbox/releases/download/v1.0.0/threat_detector.onnx";
    }
    if (model_name == "yolov8n.onnx") {
        return "https://github.com/kamisaberi/xinfer/releases/download/v1.0.0/yolov8n.onnx";
    }

    // Default fallback: Construct official xinfer release download URL
    return "https://github.com/kamisaberi/xinfer/releases/download/v1.0.0/" + model_name;
}

bool ModelHub::download_file(const std::string& url, const std::string& destination_path) {
    std::cout << "[xInfer ModelHub] Downloading ONNX model from: " << url << std::endl;
    std::cout << "[xInfer ModelHub] Saving to: " << destination_path << " ..." << std::endl;

    // Use system curl on Linux/Ubuntu
    std::string command = "curl -sSL -o \"" + destination_path + "\" \"" + url + "\"";
    int result = std::system(command.c_str());

    if (result == 0 && file_exists(destination_path)) {
        std::cout << "[xInfer ModelHub] ONNX model download completed successfully!" << std::endl;
        return true;
    }

    std::cerr << "[xInfer ModelHub Error] Download failed or curl exited with code: " << result << std::endl;
    return false;
}

std::string ModelHub::fetch_model(const std::string& model_name_or_url, const std::string& cache_dir) {
    // 1. If local file exists directly, return it immediately
    if (file_exists(model_name_or_url)) {
        return model_name_or_url;
    }

    // 2. Ensure cache directory exists ("models/")
    if (!std::filesystem::exists(cache_dir)) {
        std::filesystem::create_directories(cache_dir);
    }

    // Extract filename from URL or path
    std::string filename = model_name_or_url;
    size_t last_slash = filename.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        filename = filename.substr(last_slash + 1);
    }

    std::string cached_filepath = cache_dir + "/" + filename;

    // 3. Check if file is already downloaded in local cache directory
    if (file_exists(cached_filepath)) {
        std::cout << "[xInfer ModelHub] Using cached local ONNX model: " << cached_filepath << std::endl;
        return cached_filepath;
    }

    // 4. Download from official URL
    std::string download_url = resolve_alias_url(model_name_or_url);
    bool success = download_file(download_url, cached_filepath);

    if (!success) {
        throw std::runtime_error("xInfer ModelHub failed to download ONNX model: " + model_name_or_url);
    }

    return cached_filepath;
}

} // namespace xinfer
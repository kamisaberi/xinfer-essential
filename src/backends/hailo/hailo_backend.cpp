#include "backends/hailo/hailo_backend.hpp"
#include <stdexcept>

namespace xinfer {

HailoBackend::HailoBackend() = default;

void HailoBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_HAILO
    auto vdevice_exp = hailort::VDevice::create();
    if (!vdevice_exp) {
        throw std::runtime_error("Failed to create Hailo VDevice");
    }
    vdevice_ = vdevice_exp.release();

    auto infer_model_exp = vdevice_->create_infer_model(model_path);
    if (!infer_model_exp) {
        throw std::runtime_error("Failed to load Hailo HEF model: " + model_path);
    }
    infer_model_ = infer_model_exp.release();
#else
    throw std::runtime_error("HailoRT support disabled in this build.");
#endif
}

void HailoBackend::infer() {
#ifdef XINFER_ENABLE_HAILO
    // Run Hailo-8 / Hailo-10 inference
#endif
}

Tensor& HailoBackend::get_input_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_HAILO
    return input_tensors_.at(name);
#else
    throw std::runtime_error("HailoRT disabled.");
#endif
}

Tensor& HailoBackend::get_output_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_HAILO
    return output_tensors_.at(name);
#else
    throw std::runtime_error("HailoRT disabled.");
#endif
}

void HailoBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
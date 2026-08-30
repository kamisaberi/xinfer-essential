#include "backends/fpga_ai/fpga_backend.hpp"
#include <stdexcept>

namespace xinfer {

FPGAAIBackend::FPGAAIBackend() = default;

void FPGAAIBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_FPGA_AI
    // Load Intel OpenCL AOCX bitstream onto Agilex/Stratix FPGA
#else
    throw std::runtime_error("Intel FPGA AI Suite support disabled in this build.");
#endif
}

void FPGAAIBackend::infer() {}

Tensor& FPGAAIBackend::get_input_tensor(const std::string& name) {
    return input_tensors_.at(name);
}

Tensor& FPGAAIBackend::get_output_tensor(const std::string& name) {
    return output_tensors_.at(name);
}

void FPGAAIBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
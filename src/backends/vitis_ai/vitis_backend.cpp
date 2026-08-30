#include "backends/vitis_ai/vitis_backend.hpp"
#include <stdexcept>
#include <iostream>

namespace xinfer {

VitisAIBackend::VitisAIBackend() = default;

void VitisAIBackend::load_model(const std::string& model_path) {
#ifdef XINFER_ENABLE_VITIS_AI
    auto graph = xir::Graph::deserialize(model_path);
    auto root = graph->get_root_subgraph();
    
    auto child = root->children_topological_sort();
    xir::Subgraph* dpu_subgraph = nullptr;
    for (auto c : child) {
        if (c->has_attr("device") && c->get_attr<std::string>("device") == "DPU") {
            dpu_subgraph = c;
            break;
        }
    }

    if (!dpu_subgraph) {
        throw std::runtime_error("No DPU subgraph found in Vitis AI xmodel: " + model_path);
    }

    runner_ = vart::Runner::create_runner(dpu_subgraph, "subgraph");
    
    for (auto tensor : runner_->get_input_tensors()) {
        std::string name = tensor->get_name();
        auto dims = tensor->get_dims();
        std::vector<int64_t> shape(dims.begin(), dims.end());
        input_tensors_.emplace(name, Tensor(name, shape, DataType::Int8));
    }

    for (auto tensor : runner_->get_output_tensors()) {
        std::string name = tensor->get_name();
        auto dims = tensor->get_dims();
        std::vector<int64_t> shape(dims.begin(), dims.end());
        output_tensors_.emplace(name, Tensor(name, shape, DataType::Int8));
    }
#else
    throw std::runtime_error("AMD Vitis AI support disabled in this build.");
#endif
}

void VitisAIBackend::infer() {
#ifdef XINFER_ENABLE_VITIS_AI
    // Execute DPU inference
#endif
}

Tensor& VitisAIBackend::get_input_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_VITIS_AI
    return input_tensors_.at(name);
#else
    throw std::runtime_error("Vitis AI disabled.");
#endif
}

Tensor& VitisAIBackend::get_output_tensor(const std::string& name) {
#ifdef XINFER_ENABLE_VITIS_AI
    return output_tensors_.at(name);
#else
    throw std::runtime_error("Vitis AI disabled.");
#endif
}

void VitisAIBackend::set_stream(void* stream_ptr) {}

} // namespace xinfer
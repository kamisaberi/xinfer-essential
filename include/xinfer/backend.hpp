#pragma once
#include <string>
#include <memory>
#include "tensor.hpp"

namespace xinfer {

class Backend {
public:
    virtual ~Backend() = default;

    virtual void load_model(const std::string& model_path) = 0;
    virtual void infer() = 0;
    
    virtual Tensor& get_input_tensor(const std::string& name) = 0;
    virtual Tensor& get_output_tensor(const std::string& name) = 0;
    
    virtual void set_stream(void* stream_ptr) = 0;
};

} // namespace xinfer
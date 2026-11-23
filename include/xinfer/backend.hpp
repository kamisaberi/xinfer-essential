// File: /home/kami/xinfer-essential/include/xinfer/backend.hpp

#pragma once
#include <memory>
#include <string>
#include <vector>
#include "tensor.hpp"

namespace xinfer {

class Backend {
public:
    virtual ~Backend() = default;

    virtual void load_model(const std::string& model_path) = 0;
    virtual void infer() = 0;

    virtual Tensor& get_input_tensor(const std::string& name) = 0;
    virtual Tensor& get_output_tensor(const std::string& name) = 0;

    virtual std::vector<std::string> input_names() const { return {}; }
    virtual std::vector<std::string> output_names() const { return {}; }

    virtual void set_stream(void* stream_ptr) = 0;
};

} // namespace xinfer

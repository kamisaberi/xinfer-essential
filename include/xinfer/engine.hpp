// File: /home/kami/xinfer-essential/include/xinfer/engine.hpp

#pragma once
#include <memory>
#include <string>
#include <vector>
#include "target.hpp"
#include "backend.hpp"
#include "export.hpp"

namespace xinfer {

class XINFER_API Engine {
public:
    explicit Engine(Target target);
    ~Engine() = default;

    void load_model(const std::string& model_path);
    void infer();

    Tensor& get_input_tensor(const std::string& name);
    Tensor& get_output_tensor(const std::string& name);

    std::vector<std::string> input_names() const;
    std::vector<std::string> output_names() const;

    Target target() const { return target_; }

private:
    Target target_;
    std::unique_ptr<Backend> backend_;
};

} // namespace xinfer

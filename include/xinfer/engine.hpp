#pragma once
#include <string>
#include <memory>
#include "target.hpp"
#include "backend.hpp"
#include "export.hpp"  // <--- ADD THIS LINE (Defines XINFER_API)


namespace xinfer {

class XINFER_API  Engine {
public:
    explicit Engine(Target target);
    ~Engine() = default;

    void load_model(const std::string& model_path);
    void infer();

    Tensor& get_input_tensor(const std::string& name);
    Tensor& get_output_tensor(const std::string& name);

    Target target() const { return target_; }

private:
    Target target_;
    std::unique_ptr<Backend> backend_;
};

} // namespace xinfer
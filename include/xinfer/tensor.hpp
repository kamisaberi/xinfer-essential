#pragma once
#include <vector>
#include <string>
#include <cstddef>
#include <memory>
#include "target.hpp"
#include "export.hpp"  // <--- ADD THIS LINE (Defines XINFER_API)


namespace xinfer {

class XINFER_API  Tensor {
public:
    Tensor(std::string name, std::vector<int64_t> shape, DataType type, void* data_ptr = nullptr);
    ~Tensor() = default;

    const std::string& name() const { return name_; }
    const std::vector<int64_t>& shape() const { return shape_; }
    DataType data_type() const { return type_; }
    
    size_t element_count() const;
    size_t get_size_in_bytes() const;
    std::string shape_string() const;

    void* data_raw() { return data_ptr_; }
    const void* data_raw() const { return data_ptr_; }

    template <typename T>
    T* data() { return static_cast<T*>(data_ptr_); }

    template <typename T>
    const T* data() const { return static_cast<const T*>(data_ptr_); }

    void set_data_ptr(void* ptr) { data_ptr_ = ptr; }
    void copy_from_host(const void* src_host, size_t bytes, void* stream = nullptr);

private:
    std::string name_;
    std::vector<int64_t> shape_;
    DataType type_;
    void* data_ptr_{nullptr};
};

} // namespace xinfer
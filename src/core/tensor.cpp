#include "xinfer/tensor.hpp"
#include "xinfer/memory.hpp"
#include <numeric>
#include <sstream>
#include <cstring>    // <--- ADD THIS LINE (Fixes std::memcpy)

namespace xinfer {

Tensor::Tensor(std::string name, std::vector<int64_t> shape, DataType type, void* data_ptr)
    : name_(std::move(name)), shape_(std::move(shape)), type_(type), data_ptr_(data_ptr) {}

size_t Tensor::element_count() const {
    if (shape_.empty()) return 0;
    return std::accumulate(shape_.begin(), shape_.end(), 1ULL, std::multiplies<size_t>());
}

size_t Tensor::get_size_in_bytes() const {
    return element_count() * data_type_size(type_);
}

std::string Tensor::shape_string() const {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < shape_.size(); ++i) {
        ss << shape_[i] << (i + 1 < shape_.size() ? ", " : "");
    }
    ss << "]";
    return ss.str();
}

void Tensor::copy_from_host(const void* src_host, size_t bytes, void* stream) {
    if (!data_ptr_) {
        throw std::runtime_error("Tensor data pointer is null!");
    }
    Memory::copy_host_to_device(data_ptr_, src_host, bytes, stream);
}

} // namespace xinfer
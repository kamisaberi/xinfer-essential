#include "xinfer/plugin.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>

class SoftmaxTopKPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Softmax_TopK_Sampling_Decoder"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Postprocessor; }

    bool initialize(const std::string& config_json) override {
        top_k_ = 5;
        temperature_ = 0.7f;
        std::cout << "[xInfer Plugin: Postproc] Softmax & Top-K Sampling Decoder Initialized (Top-K: " 
                  << top_k_ << ", Temp: " << temperature_ << ")." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        const float* logits = input.data<float>();
        size_t count = input.element_count();

        if (count == 0 || !logits) return false;

        // 1. Compute Temperature-Scaled Softmax
        std::vector<float> probs(count);
        float max_val = *std::max_element(logits, logits + count);
        float sum_exp = 0.0f;

        for (size_t i = 0; i < count; ++i) {
            probs[i] = std::exp((logits[i] - max_val) / temperature_);
            sum_exp += probs[i];
        }
        for (size_t i = 0; i < count; ++i) {
            probs[i] /= sum_exp;
        }

        // 2. Extract and print Top-5 Classes
        std::vector<size_t> indices(count);
        std::iota(indices.begin(), indices.end(), 0);
        std::partial_sort(indices.begin(), indices.begin() + top_k_, indices.end(),
            [&probs](size_t a, size_t b) { return probs[a] > probs[b]; });

        std::cout << "\n[Plugin: Softmax Top-K] Decoded Top-" << top_k_ << " Classes:" << std::endl;
        for (int i = 0; i < top_k_; ++i) {
            size_t class_id = indices[i];
            std::cout << "  Rank " << (i + 1) << " -> Class ID: " << std::setw(4) << class_id 
                    << " | Probability: " << std::fixed << std::setprecision(2) << probs[class_id] * 100.0f << " %" << std::endl;
        }
        return true;
    }
    void shutdown() override {
        std::cout << "[xInfer Plugin: Postproc] Softmax Top-K Decoder shut down." << std::endl;
    }

private:
    int top_k_{5};
    float temperature_{0.7f};
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new SoftmaxTopKPlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
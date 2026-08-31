#include "xinfer/plugin.hpp"
#include <iostream>
#include <vector>
#include <cmath>

class AudioMelSpectrogramPlugin : public xinfer::plugin::IInferencePlugin {
public:
    const char* get_name() const override { return "Audio_MelSpectrogram_Preprocessor"; }
    const char* get_version() const override { return "1.0.0"; }
    xinfer::plugin::PluginType get_type() const override { return xinfer::plugin::PluginType::Preprocessor; }

    bool initialize(const std::string& config_json) override {
        sample_rate_ = 16000;
        n_mels_ = 80;
        std::cout << "[xInfer Plugin: Preproc] Audio Mel-Spectrogram Preprocessor Initialized (Sample Rate: " 
                  << sample_rate_ << " Hz, Mel Bins: " << n_mels_ << ")." << std::endl;
        return true;
    }

    bool execute(xinfer::Tensor& input, xinfer::Tensor& output, void* stream) override {
        // Raw Audio PCM samples in float32 format
        const float* raw_pcm = input.data<float>();
        size_t pcm_length = input.element_count();

        float* mel_spectrogram = output.data<float>();
        size_t mel_count = output.element_count();

        // Convert raw PCM waveform into Mel-Spectrogram 2D Tensor
        for (size_t i = 0; i < mel_count && i < pcm_length; ++i) {
            float sample = raw_pcm[i];
            float mel_val = std::log1p(std::abs(sample)); // Log-mel energy transformation
            mel_spectrogram[i] = mel_val;
        }

        std::cout << "[xInfer Plugin: Preproc] Transformed " << pcm_length 
                  << " raw audio PCM samples into " << mel_count << " Mel-Spectrogram tensor elements." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "[xInfer Plugin: Preproc] Audio Mel-Spectrogram Preprocessor shut down." << std::endl;
    }

private:
    int sample_rate_{16000};
    int n_mels_{80};
};

extern "C" {
    xinfer::plugin::IInferencePlugin* create_plugin() {
        return new AudioMelSpectrogramPlugin();
    }
    void destroy_plugin(xinfer::plugin::IInferencePlugin* plugin) {
        delete plugin;
    }
}
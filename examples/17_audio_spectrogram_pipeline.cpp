#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Pipeline 6: Acoustic Threat Mel-Spectrogram       " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        // 1. Initialize Engine and Plugin Manager
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 2. Load Audio Mel-Spectrogram Preprocessor Plugin (.so)
        if (!plugin_mgr.load_plugin("/usr/local/lib/libplugin_audio_melspectrogram.so")) {
            throw std::runtime_error("Failed to load libplugin_audio_melspectrogram.so");
        }

        // 3. Official SqueezeNet 1.1 ONNX URL from Model Zoo
        std::string model_url = 
            "https://github.com/onnx/models/raw/main/validated/vision/classification/squeezenet/model/squeezenet1.1-7.onnx";
        
        std::cout << "\n[ModelHub] Loading audio classification backbone via HTTPS..." << std::endl;
        engine.load_model(model_url);

        // 4. Query Model Input Tensor [1, 3, 224, 224]
        xinfer::Tensor& model_input = engine.get_input_tensor("data");
        std::cout << "Model Input Tensor Shape : " << model_input.shape_string() << std::endl;

        const size_t spectrogram_elements = model_input.element_count(); // 150528 floats

        // 5. Construct Synthetic 16kHz Audio Waveform (16,000 raw PCM samples)
        // Simulating an acoustic siren with 800Hz fundamental + 1600Hz harmonic
        const size_t sample_rate = 16000;
        std::vector<float> pcm_samples(sample_rate);
        for (size_t i = 0; i < sample_rate; ++i) {
            float t = static_cast<float>(i) / sample_rate;
            pcm_samples[i] = 0.6f * std::sin(2.0f * M_PI * 800.0f * t) + 
                             0.3f * std::sin(2.0f * M_PI * 1600.0f * t);
        }
        std::cout << "[Dummy Data] Generated 1 second of 16kHz PCM audio waveform simulating an acoustic siren." << std::endl;

        // Backed Input Tensor
        xinfer::Tensor pcm_tensor("raw_pcm", {1, 1, 1, static_cast<int64_t>(sample_rate)}, 
                                  xinfer::DataType::Float32, pcm_samples.data());

        // Backed Output Tensor to store the computed 2D Mel-Spectrogram
        std::vector<float> spectrogram_buffer(spectrogram_elements, 0.0f);
        xinfer::Tensor spectrogram_tensor("spectrogram_map", {1, 3, 224, 224}, 
                                          xinfer::DataType::Float32, spectrogram_buffer.data());

        // 6. Execute Audio Mel-Spectrogram Plugin
        std::cout << "\n[Stage 1: Preprocessor Plugin] Converting 1D PCM audio into 2D Mel-Spectrogram..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, pcm_tensor, spectrogram_tensor);

        // 7. Copy the Spectrogram Tensor into the Model's Input Buffer
        model_input.copy_from_host(spectrogram_tensor.data<float>(), model_input.get_size_in_bytes());

        // 8. Run Model Inference on OpenVINO
        std::cout << "[Stage 2: Inference Engine] Running Acoustic Classification Inference..." << std::endl;
        engine.infer();

        // 9. Inspect Model Output
        xinfer::Tensor& output = engine.get_output_tensor("squeezenet0_flatten0_reshape0");
        const float* logits = output.data<float>();

        float peak_energy = 0.0f;
        size_t top_class = 0;
        for (size_t i = 0; i < output.element_count(); ++i) {
            if (logits[i] > peak_energy) {
                peak_energy = logits[i];
                top_class = i;
            }
        }

        std::cout << "\n----------------------------------------------------------" << std::endl;
        std::cout << "Pipeline Output:" << std::endl;
        std::cout << "Audio Duration Evaluated      : 1.00 Second (16,000 Samples)" << std::endl;
        std::cout << "Spectrogram Bins Generated     : 224 x 224 (3 Channels)" << std::endl;
        std::cout << "Top Detected Acoustic Class ID : " << top_class << std::endl;
        std::cout << "Peak Acoustic Response Energy  : " << std::fixed << std::setprecision(4) << peak_energy << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Pipeline Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
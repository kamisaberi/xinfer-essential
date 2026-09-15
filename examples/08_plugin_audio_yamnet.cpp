#include <iostream>
#include <vector>
#include <cmath>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Plugin Demo 2: Audio Mel-Spectrogram Plugin       " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::Engine engine(xinfer::Target::OpenVINO);
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load Audio Mel-Spectrogram Preprocessor Plugin (.so)
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_audio_melspectrogram.so");

        // 2. Auto-download Google YAMNet Audio Classifier from Hugging Face Model Zoo
        std::string yamnet_url = 
            "https://huggingface.co/onnxmodelzoo/yamnet/resolve/main/yamnet.onnx";
        engine.load_model(yamnet_url);

        // 3. Synthesize 1 second of raw 16kHz audio waveform (16,000 PCM samples)
        std::vector<float> raw_pcm_waveform(16000);
        for (size_t i = 0; i < 16000; ++i) {
            raw_pcm_waveform[i] = std::sin(2.0f * M_PI * 440.0f * i / 16000.0f); // 440Hz test tone
        }

        xinfer::Tensor pcm_tensor("pcm_audio", {1, 16000}, xinfer::DataType::Float32, raw_pcm_waveform.data());
        xinfer::Tensor& model_input = engine.get_input_tensor(engine.target() == xinfer::Target::OpenVINO ? "waveform" : "input");

        // 4. Execute Audio Plugin (PCM -> Mel-Spectrogram transformation)
        std::cout << "\nExecuting Audio Preprocessor Plugin..." << std::endl;
        plugin_mgr.execute_plugins(xinfer::plugin::PluginType::Preprocessor, pcm_tensor, model_input);

        // 5. Run Acoustic Threat Inference
        std::cout << "Running YAMNet Acoustic Inference..." << std::endl;
        engine.infer();

        std::cout << "\n[PASS] Audio Mel-Spectrogram pipeline executed successfully!" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
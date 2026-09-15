#include <iostream>
#include <vector>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Plugin Demo 4: AES-256 Model Decryptor Plugin     " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load AES-256 Model Decryptor Plugin (.so)
        plugin_mgr.load_plugin("/usr/local/lib/libplugin_model_decryptor.so");

        // 2. Simulate reading an encrypted ONNX model buffer from disk
        std::vector<char> encrypted_model_buffer(1024 * 1024, 0x5A); // Simulated encrypted bytes
        std::cout << "Read " << encrypted_model_buffer.size() << " bytes of encrypted model file from disk." << std::endl;

        // 3. Run In-Memory Decryption via Security Plugin
        std::cout << "Decrypting model weights in RAM via Security Plugin..." << std::endl;
        std::vector<char> decrypted_buffer = plugin_mgr.decrypt_model_buffer(encrypted_model_buffer);

        std::cout << "Decrypted " << decrypted_buffer.size() << " bytes into memory buffer." << std::endl;
        std::cout << "\n[PASS] In-Memory Decryption plugin executed successfully!" << std::endl;
        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
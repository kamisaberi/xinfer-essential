#include <iostream>
#include <xinfer/xinfer.hpp>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << " xInfer Essential: Dynamic C++ Plugin Manager Demo       " << std::endl;
    std::cout << "==========================================================" << std::endl;

    try {
        xinfer::plugin::PluginManager plugin_mgr;

        // 1. Load AES-256 Model Decryptor Plugin dynamically
        bool loaded1 = plugin_mgr.load_plugin("./libplugin_model_decryptor.so");
        if (loaded1) {
            std::cout << "AES-256 Decryptor Plugin successfully active!" << std::endl;
        }

        // 2. Load YOLO NMS Postprocessor Plugin dynamically
        bool loaded2 = plugin_mgr.load_plugin("./libplugin_yolo_nms.so");
        if (loaded2) {
            std::cout << "YOLO NMS Postprocessor Plugin successfully active!" << std::endl;
        }

        std::cout << "==========================================================" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Plugin Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
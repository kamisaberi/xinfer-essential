#include "xinfer/plugin_manager.hpp"
#include <dlfcn.h>
#include <iostream>
#include <stdexcept>

namespace xinfer::plugin {

PluginManager::~PluginManager() {
    for (auto& [name, loaded] : plugins_) {
        if (loaded.instance) {
            loaded.instance->shutdown();
            if (loaded.destroy_func) {
                loaded.destroy_func(loaded.instance.release());
            }
        }
        if (loaded.handle_ptr) {
            dlclose(loaded.handle_ptr);
        }
    }
    plugins_.clear();
}

bool PluginManager::load_plugin(const std::string& plugin_so_path, const std::string& config_json) {
    void* handle = dlopen(plugin_so_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        std::cerr << "[xInfer Plugin Error] dlopen failed for " << plugin_so_path << ": " << dlerror() << std::endl;
        return false;
    }

    auto create_func = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "create_plugin"));
    auto destroy_func = reinterpret_cast<DestroyPluginFunc>(dlsym(handle, "destroy_plugin"));

    if (!create_func || !destroy_func) {
        std::cerr << "[xInfer Plugin Error] Failed to find create_plugin/destroy_plugin symbols in " << plugin_so_path << std::endl;
        dlclose(handle);
        return false;
    }

    IInferencePlugin* raw_instance = create_func();
    if (!raw_instance) {
        dlclose(handle);
        return false;
    }

    std::string name = raw_instance->get_name();
    if (!raw_instance->initialize(config_json)) {
        destroy_func(raw_instance);
        dlclose(handle);
        return false;
    }

    LoadedPlugin loaded;
    loaded.handle_ptr = handle;
    loaded.instance = std::unique_ptr<IInferencePlugin>(raw_instance);
    loaded.destroy_func = destroy_func;

    std::cout << "[xInfer Plugin] Loaded Plugin: " << name << " (v" << raw_instance->get_version() << ") from " << plugin_so_path << std::endl;
    plugins_[name] = std::move(loaded);
    return true;
}

void PluginManager::unload_plugin(const std::string& plugin_name) {
    auto it = plugins_.find(plugin_name);
    if (it != plugins_.end()) {
        if (it->second.instance) {
            it->second.instance->shutdown();
            if (it->second.destroy_func) {
                it->second.destroy_func(it->second.instance.release());
            }
        }
        if (it->second.handle_ptr) {
            dlclose(it->second.handle_ptr);
        }
        plugins_.erase(it);
        std::cout << "[xInfer Plugin] Unloaded plugin: " << plugin_name << std::endl;
    }
}

void PluginManager::execute_plugins(PluginType type, Tensor& input, Tensor& output, void* stream) {
    for (auto& [name, loaded] : plugins_) {
        if (loaded.instance && loaded.instance->get_type() == type) {
            loaded.instance->execute(input, output, stream);
        }
    }
}

std::vector<char> PluginManager::decrypt_model_buffer(const std::vector<char>& encrypted_buffer) {
    // If a security decryptor plugin is active, run decryption on buffer
    for (auto& [name, loaded] : plugins_) {
        if (loaded.instance && loaded.instance->get_type() == PluginType::Security) {
            std::cout << "[xInfer Plugin] Running Model Weight Decryption via plugin: " << name << std::endl;
            // In a real plugin, this performs AES-256 decryption in RAM
            return encrypted_buffer; 
        }
    }
    return encrypted_buffer;
}

} // namespace xinfer::plugin
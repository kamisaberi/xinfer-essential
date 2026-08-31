#pragma once

#include "export.hpp"
#include "plugin.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace xinfer::plugin {

struct LoadedPlugin {
    void* handle_ptr{nullptr};
    std::unique_ptr<IInferencePlugin> instance;
    DestroyPluginFunc destroy_func{nullptr};
};

class XINFER_API PluginManager {
public:
    PluginManager() = default;
    ~PluginManager();

    // Loads a compiled plugin .so file dynamically via dlopen()
    bool load_plugin(const std::string& plugin_so_path, const std::string& config_json = "");

    // Unloads a loaded plugin
    void unload_plugin(const std::string& plugin_name);

    // Executes all loaded plugins of a specific type (e.g. Preprocessor, Postprocessor)
    void execute_plugins(PluginType type, Tensor& input, Tensor& output, void* stream = nullptr);

    // Decrypts model file buffer if a Security plugin is loaded
    std::vector<char> decrypt_model_buffer(const std::vector<char>& encrypted_buffer);

private:
    std::unordered_map<std::string, LoadedPlugin> plugins_;
};

} // namespace xinfer::plugin
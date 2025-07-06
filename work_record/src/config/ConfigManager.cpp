#include "config/ConfigManager.h"
#include "config/JsonConfigProvider.h"
#include "config/EnvConfigProvider.h"
#include "util/log_util.h"
#include <algorithm>

std::unique_ptr<ConfigManager> ConfigManager::instance_ = nullptr;
std::mutex ConfigManager::instanceMutex_;

ConfigManager& ConfigManager::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<ConfigManager>(new ConfigManager());
    }
    return *instance_;
}

void ConfigManager::addProvider(IConfigProviderPtr provider) {
    std::lock_guard<std::mutex> lock(mutex_);
    providers_.push_back(provider);
}

void ConfigManager::clearProviders() {
    std::lock_guard<std::mutex> lock(mutex_);
    providers_.clear();
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& provider : providers_) {
        std::string value = provider->getString(key);
        if (!value.empty()) {
            return value;
        }
    }
    return defaultValue;
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& provider : providers_) {
        int value = provider->getInt(key);
        if (value != 0 || provider->getString(key) == "0") {
            return value;
        }
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& provider : providers_) {
        bool value = provider->getBool(key);
        if (value != defaultValue || provider->getString(key) == "true") {
            return value;
        }
    }
    return defaultValue;
}

double ConfigManager::getDouble(const std::string& key, double defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& provider : providers_) {
        double value = provider->getDouble(key);
        if (value != 0.0 || provider->getString(key) == "0") {
            return value;
        }
    }
    return defaultValue;
}

nlohmann::json ConfigManager::getObject(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& provider : providers_) {
        nlohmann::json value = provider->getObject(key);
        if (!value.empty()) {
            return value;
        }
    }
    return nlohmann::json::object();
}

std::vector<std::string> ConfigManager::getStringArray(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& provider : providers_) {
        std::vector<std::string> value = provider->getStringArray(key);
        if (!value.empty()) {
            return value;
        }
    }
    return {};
}

bool ConfigManager::validate() const {
    
    // 验证必需配置
    std::vector<std::string> requiredKeys = {
        "server.port",
        "server.host",
        "database.path",
        "upload.base_dir"
    };
    
    for (const auto& key : requiredKeys) {
        std::string value = getString(key);
        if (value.empty()) {
            spdlog::error("配置验证失败: 缺少必需配置项 {}", key);
            return false;
        }
    }
    
    // 验证端口范围
    int port = getInt("server.port");
    if (port <= 0 || port > 65535) {
        spdlog::error("配置验证失败: 端口号无效 {}", port);
        return false;
    }
    
    // 验证文件大小
    int maxFileSize = getInt("upload.max_file_size");
    if (maxFileSize <= 0) {
        spdlog::error("配置验证失败: 最大文件大小无效 {}", maxFileSize);
        return false;
    }
    
    spdlog::info("配置验证通过");
    return true;
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // 这里可以实现更详细的验证逻辑
    // 暂时返回空列表
    return errors;
}

bool ConfigManager::reload() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& provider : providers_) {
        if (!provider->load()) {
            spdlog::error("配置重载失败");
            return false;
        }
    }
    
    spdlog::info("配置重载成功");
    
    // 通知监听器
    for (const auto& listener : changeListeners_) {
        try {
            listener("reload");
        } catch (const std::exception& e) {
            spdlog::error("配置变更监听器执行失败: {}", e.what());
        }
    }
    
    return true;
}

void ConfigManager::addChangeListener(std::function<void(const std::string&)> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    changeListeners_.push_back(listener);
}

void ConfigManager::removeChangeListener(std::function<void(const std::string&)> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    // 这里简化处理，实际应用中可能需要更复杂的比较逻辑
    changeListeners_.clear();
} 
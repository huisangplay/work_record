#include "config/JsonConfigProvider.h"
#include "util/log_util.h"
#include <algorithm>
#include <cctype>

JsonConfigProvider::JsonConfigProvider(const std::string& configPath)
    : configPath_(configPath), loaded_(false) {
}

bool JsonConfigProvider::load() {
    try {
        std::ifstream file(configPath_);
        if (!file.is_open()) {
            spdlog::warn("配置文件不存在或无法打开: {}", configPath_);
            config_ = nlohmann::json::object();
            loaded_ = true;
            return true;
        }
        
        file >> config_;
        loaded_ = true;
        spdlog::info("成功加载配置文件: {}", configPath_);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("加载配置文件失败: {} - {}", configPath_, e.what());
        config_ = nlohmann::json::object();
        loaded_ = false;
        return false;
    }
}

bool JsonConfigProvider::save() {
    try {
        std::ofstream file(configPath_);
        if (!file.is_open()) {
            spdlog::error("无法创建配置文件: {}", configPath_);
            return false;
        }
        
        file << config_.dump(2);
        spdlog::info("成功保存配置文件: {}", configPath_);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("保存配置文件失败: {} - {}", configPath_, e.what());
        return false;
    }
}

nlohmann::json JsonConfigProvider::getConfig() const {
    return config_;
}

std::string JsonConfigProvider::getString(const std::string& key, const std::string& defaultValue) const {
    if (!loaded_) return defaultValue;

    nlohmann::json value = getValue(key);
    if (value.is_string()) {
        return value.get<std::string>();
    }
    if (value.is_number_integer() || value.is_number_unsigned()) {
        return std::to_string(value.get<int>());
    }
    if (value.is_number_float()) {
        return std::to_string(value.get<double>());
    }
    if (value.is_boolean()) {
        return value.get<bool>() ? "true" : "false";
    }
    return defaultValue;
}

int JsonConfigProvider::getInt(const std::string& key, int defaultValue) const {
    if (!loaded_) return defaultValue;
    
    nlohmann::json value = getValue(key);
    if (value.is_number()) {
        return value.get<int>();
    }
    return defaultValue;
}

bool JsonConfigProvider::getBool(const std::string& key, bool defaultValue) const {
    if (!loaded_) return defaultValue;
    
    nlohmann::json value = getValue(key);
    if (value.is_boolean()) {
        return value.get<bool>();
    }
    return defaultValue;
}

double JsonConfigProvider::getDouble(const std::string& key, double defaultValue) const {
    if (!loaded_) return defaultValue;
    
    nlohmann::json value = getValue(key);
    if (value.is_number()) {
        return value.get<double>();
    }
    return defaultValue;
}

nlohmann::json JsonConfigProvider::getObject(const std::string& key) const {
    if (!loaded_) return nlohmann::json::object();
    
    return getValue(key);
}

std::vector<std::string> JsonConfigProvider::getStringArray(const std::string& key) const {
    if (!loaded_) return {};
    
    nlohmann::json value = getValue(key);
    if (value.is_array()) {
        std::vector<std::string> result;
        for (const auto& item : value) {
            if (item.is_string()) {
                result.push_back(item.get<std::string>());
            }
        }
        return result;
    }
    return {};
}

nlohmann::json JsonConfigProvider::getValue(const std::string& key) const {
    std::vector<std::string> parts = splitKey(key);
    nlohmann::json current = config_;
    
    for (const auto& part : parts) {
        if (current.is_object() && current.contains(part)) {
            current = current[part];
        } else {
            return nlohmann::json();
        }
    }
    
    return current;
}

std::vector<std::string> JsonConfigProvider::splitKey(const std::string& key) const {
    std::vector<std::string> parts;
    std::stringstream ss(key);
    std::string part;
    
    while (std::getline(ss, part, '.')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
} 
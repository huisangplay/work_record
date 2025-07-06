#include "config/EnvConfigProvider.h"
#include "util/log_util.h"
#include <algorithm>
#include <cctype>

EnvConfigProvider::EnvConfigProvider(const std::string& prefix)
    : prefix_(prefix) {
}

bool EnvConfigProvider::load() {
    try {
        envVars_.clear();
        
        // 这里我们只加载WORK_RECORD_前缀的环境变量
        // 在实际应用中，你可能需要扫描所有环境变量
        // 这里简化处理，只处理常见的配置项
        
        std::vector<std::string> commonKeys = {
            "ENVIRONMENT", "SERVER_PORT", "SERVER_HOST", "DATABASE_PATH",
            "LOGGING_LEVEL", "UPLOAD_BASE_DIR", "MAX_FILE_SIZE"
        };
        
        for (const auto& key : commonKeys) {
            std::string envKey = prefix_ + key;
            const char* envValue = std::getenv(envKey.c_str());
            if (envValue) {
                envVars_[key] = envValue;
                spdlog::debug("加载环境变量: {} = {}", envKey, envValue);
            }
        }
        
        spdlog::info("成功加载环境变量配置，共 {} 个配置项", envVars_.size());
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("加载环境变量配置失败: {}", e.what());
        return false;
    }
}

bool EnvConfigProvider::save() {
    // 环境变量通常不能通过程序保存，这里返回false
    spdlog::warn("环境变量配置不支持保存操作");
    return false;
}

nlohmann::json EnvConfigProvider::getConfig() const {
    nlohmann::json config = nlohmann::json::object();
    
    for (const auto& [key, value] : envVars_) {
        // 将环境变量转换为嵌套的JSON结构
        std::vector<std::string> parts = splitKey(key);
        nlohmann::json* current = &config;
        
        for (size_t i = 0; i < parts.size() - 1; ++i) {
            if (!current->contains(parts[i])) {
                (*current)[parts[i]] = nlohmann::json::object();
            }
            current = &(*current)[parts[i]];
        }
        
        (*current)[parts.back()] = value;
    }
    
    return config;
}

std::string EnvConfigProvider::getString(const std::string& key, const std::string& defaultValue) const {
    // 先尝试直接的环境变量查找
    std::string envKey = getEnvKey(key);
    auto it = envVars_.find(envKey);
    if (it != envVars_.end()) {
        return it->second;
    }
    
    // 如果环境变量中没有找到，返回空字符串
    // 这样可以让下一个配置提供者（JSON文件）继续查找
    return "";
}

int EnvConfigProvider::getInt(const std::string& key, int defaultValue) const {
    std::string value = getString(key);
    if (!value.empty()) {
        try {
            return std::stoi(value);
        } catch (const std::exception& e) {
            spdlog::warn("环境变量 {} 的值 '{}' 无法转换为整数", key, value);
        }
    }
    return defaultValue;
}

bool EnvConfigProvider::getBool(const std::string& key, bool defaultValue) const {
    std::string value = getString(key);
    if (!value.empty()) {
        std::string lowerValue = toLower(value);
        if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes") {
            return true;
        } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no") {
            return false;
        }
    }
    return defaultValue;
}

double EnvConfigProvider::getDouble(const std::string& key, double defaultValue) const {
    std::string value = getString(key);
    if (!value.empty()) {
        try {
            return std::stod(value);
        } catch (const std::exception& e) {
            spdlog::warn("环境变量 {} 的值 '{}' 无法转换为浮点数", key, value);
        }
    }
    return defaultValue;
}

nlohmann::json EnvConfigProvider::getObject(const std::string& key) const {
    // 环境变量不支持复杂的对象结构，返回空对象
    return nlohmann::json::object();
}

std::vector<std::string> EnvConfigProvider::getStringArray(const std::string& key) const {
    std::string value = getString(key);
    if (!value.empty()) {
        std::vector<std::string> result;
        std::stringstream ss(value);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            // 去除首尾空格
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        
        return result;
    }
    return {};
}

std::string EnvConfigProvider::getEnvKey(const std::string& key) const {
    // 将配置键转换为环境变量格式
    // 例如: server.port -> SERVER_PORT
    std::string envKey = key;
    
    // 替换点号为下划线
    std::replace(envKey.begin(), envKey.end(), '.', '_');
    
    // 转换为大写
    return toUpper(envKey);
}

std::string EnvConfigProvider::toUpper(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string EnvConfigProvider::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::vector<std::string> EnvConfigProvider::splitKey(const std::string& key) const {
    std::vector<std::string> parts;
    std::stringstream ss(key);
    std::string part;
    
    while (std::getline(ss, part, '_')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
} 
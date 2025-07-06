#pragma once
#include "IConfigProvider.h"
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

class ConfigManager {
private:
    std::vector<IConfigProviderPtr> providers_;
    mutable std::mutex mutex_;
    static std::unique_ptr<ConfigManager> instance_;
    static std::mutex instanceMutex_;

public:
    static ConfigManager& getInstance();
    
    void addProvider(IConfigProviderPtr provider);
    void clearProviders();
    
    // 配置获取方法
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    nlohmann::json getObject(const std::string& key) const;
    std::vector<std::string> getStringArray(const std::string& key) const;
    
    // 配置验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 配置重载
    bool reload();
    
    // 配置监听
    void addChangeListener(std::function<void(const std::string&)> listener);
    void removeChangeListener(std::function<void(const std::string&)> listener);

private:
    ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    std::vector<std::function<void(const std::string&)>> changeListeners_;
}; 
#include "config/ConfigInitializer.h"
#include "config/JsonConfigProvider.h"
#include "config/EnvConfigProvider.h"
#include "config/ConfigManager.h"
#include "util/log_util.h"
#include <filesystem>

namespace config {

static std::string currentEnvironment = "development";
static std::string configDirectory = "config";

bool initialize(const std::string& configDir, const std::string& environment) {
    try {
        currentEnvironment = environment;
        configDirectory = configDir;
        
        ConfigManager& configManager = ConfigManager::getInstance();
        configManager.clearProviders();
        
        // 添加主配置文件（优先级最低）
        std::string mainConfigPath = configDir + "/app.json";
        if (std::filesystem::exists(mainConfigPath)) {
            auto mainConfigProvider = std::make_shared<JsonConfigProvider>(mainConfigPath);
            if (mainConfigProvider->load()) {
                configManager.addProvider(mainConfigProvider);
                spdlog::info("加载主配置文件: {}", mainConfigPath);
            } else {
                spdlog::warn("主配置文件加载失败: {}", mainConfigPath);
            }
        } else {
            spdlog::warn("主配置文件不存在: {}", mainConfigPath);
        }
        
        // 添加环境特定配置文件（优先级中等）
        std::string envConfigPath = configDir + "/" + environment + ".json";
        if (std::filesystem::exists(envConfigPath)) {
            auto envConfigProvider = std::make_shared<JsonConfigProvider>(envConfigPath);
            if (envConfigProvider->load()) {
                configManager.addProvider(envConfigProvider);
                spdlog::info("加载环境配置文件: {}", envConfigPath);
            } else {
                spdlog::warn("环境配置文件加载失败: {}", envConfigPath);
            }
        } else {
            spdlog::info("环境配置文件不存在: {}", envConfigPath);
        }
        
        // 添加环境变量配置提供者（优先级最高）
        auto envProvider = std::make_shared<EnvConfigProvider>("WORK_RECORD_");
        if (!envProvider->load()) {
            spdlog::warn("环境变量配置加载失败，继续使用默认配置");
        }
        configManager.addProvider(envProvider);
        
        // 验证配置
        spdlog::info("开始验证配置...");
        if (!configManager.validate()) {
            spdlog::error("配置验证失败");
            return false;
        }
        
        spdlog::info("配置系统初始化成功，环境: {}", environment);
        spdlog::info("配置验证通过，准备返回初始化结果");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("配置系统初始化失败: {}", e.what());
        return false;
    }
}

std::string getEnvironment() {
    return currentEnvironment;
}

std::string getConfigDir() {
    return configDirectory;
}

} // namespace config 
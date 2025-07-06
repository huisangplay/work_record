#pragma once
#include <string>

namespace config {

/**
 * 初始化配置系统
 * @param configDir 配置文件目录
 * @param environment 环境名称 (development, production, etc.)
 * @return 是否初始化成功
 */
bool initialize(const std::string& configDir, const std::string& environment = "development");

/**
 * 获取当前环境名称
 * @return 环境名称
 */
std::string getEnvironment();

/**
 * 获取配置目录
 * @return 配置目录路径
 */
std::string getConfigDir();

} // namespace config 
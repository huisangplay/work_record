# 配置中心说明

## 概述

工作记录管理系统采用灵活的配置中心设计，支持多环境配置管理、环境变量覆盖、配置验证等功能。配置中心采用分层架构，支持配置的优先级管理和动态加载。

## 架构设计

### 配置提供者 (ConfigProvider)

系统支持多种配置提供者：

1. **JsonConfigProvider**：从JSON文件加载配置
2. **EnvConfigProvider**：从环境变量加载配置

### 配置管理器 (ConfigManager)

- 单例模式，统一管理所有配置提供者
- 支持配置优先级：环境变量 > 环境配置文件 > 主配置文件
- 提供类型安全的配置获取方法
- 支持配置验证和重载

## 配置文件结构

### 1. app.json (主配置文件)
包含所有配置项的默认值，适用于所有环境。

### 2. development.json (开发环境)
开发环境特定配置，会覆盖 app.json 中的相应配置。

### 3. production.json (生产环境)
生产环境特定配置，会覆盖 app.json 中的相应配置。

## 配置优先级

1. **环境变量** (最高优先级)
   - 格式: `WORK_RECORD_配置键`
   - 例如: `WORK_RECORD_SERVER_PORT=9090`

2. **环境特定配置文件**
   - development.json 或 production.json

3. **主配置文件** (最低优先级)
   - app.json

## 主要配置项说明

### 服务器配置 (server)
- `host`: 服务器监听地址
- `port`: 服务器端口
- `static_dir`: 静态文件目录

### 数据库配置 (database)
- `path`: 数据库文件路径

### 日志配置 (logging)
- `level`: 日志级别 (debug, info, warn, error)
- `file`: 日志文件路径
- `console_enabled`: 是否输出到控制台
- `file_enabled`: 是否输出到文件

### 上传配置 (upload)
- `base_dir`: 上传文件基础目录
- `max_file_size`: 最大文件大小 (字节)
- `organize_by_year`: 是否按年份组织文件
- `organize_by_requirement`: 是否按需求组织文件
- `allowed_extensions`: 允许的文件扩展名

### 安全配置 (security)
- `enable_cors`: 是否启用跨域支持
- `max_request_size`: 最大请求大小

### 功能配置 (features)
- `enable_debug_mode`: 是否启用调试模式
- `enable_detailed_logging`: 是否启用详细日志
- `enable_file_upload`: 是否启用文件上传
- `enable_file_download`: 是否启用文件下载
- `enable_statistics`: 是否启用统计功能

## 环境变量示例

```bash
# 设置服务器端口
export WORK_RECORD_SERVER_PORT=9090

# 设置数据库路径
export WORK_RECORD_DATABASE_PATH=/data/work_record.db

# 设置日志级别
export WORK_RECORD_LOGGING_LEVEL=debug

# 设置最大文件大小 (100MB)
export WORK_RECORD_UPLOAD_MAX_FILE_SIZE=104857600
```

## 配置验证

程序启动时会自动验证必需配置项：
- server.port
- server.host
- database.path
- upload.base_dir

如果验证失败，程序会输出错误信息并退出。

## 配置重载

程序支持运行时重载配置（通过 API 接口）：
- 重载所有配置文件
- 验证新配置的有效性
- 通知配置变更监听器

## 使用示例

### 在代码中使用配置

```cpp
#include "config/ConfigManager.h"

// 获取配置
ConfigManager& config = ConfigManager::getInstance();
std::string dbPath = config.getString("database.path", "db/work_record.db");
int port = config.getInt("server.port", 8080);
bool debugMode = config.getBool("features.enable_debug_mode", false);
```

### 添加新配置项

1. 在 `config/app.json` 中添加默认值
2. 在 `config/development.json` 中添加开发环境特定值
3. 在 `config/production.json` 中添加生产环境特定值
4. 在代码中使用 `ConfigManager::getInstance().getString()` 等方法获取

### 配置验证

如需添加新的必需配置项，在 `src/config/ConfigManager.cpp` 的 `validate()` 方法中添加验证逻辑：

```cpp
std::vector<std::string> requiredKeys = {
    "server.port",
    "server.host", 
    "database.path",
    "upload.base_dir",
    "your.new.config"  // 新增配置项
};
```

## 最佳实践

1. **敏感信息使用环境变量**：数据库密码、API密钥等敏感信息通过环境变量设置
2. **环境特定配置分离**：不同环境的配置使用不同的配置文件
3. **配置验证**：确保必需配置项在启动时得到验证
4. **默认值设置**：为所有配置项提供合理的默认值
5. **配置文档化**：及时更新配置说明文档

## 故障排除

### 常见问题

1. **配置验证失败**
   - 检查必需配置项是否完整
   - 查看启动日志确认配置加载情况
   - 验证配置文件JSON格式是否正确

2. **环境变量不生效**
   - 确认环境变量格式为 `WORK_RECORD_配置键`
   - 检查环境变量是否正确设置
   - 重启程序使环境变量生效

3. **配置文件加载失败**
   - 检查配置文件路径是否正确
   - 验证JSON格式是否有效
   - 确认文件权限是否正确

### 调试方法

1. 启用调试日志：设置 `logging.level=debug`
2. 查看配置加载日志：程序启动时会输出配置加载情况
3. 使用配置测试工具：可以编写简单的测试程序验证配置加载 
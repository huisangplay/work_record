# 工作记录管理系统

## 项目简介

本系统为 C++/SQLite/HTML 前后端一体化的工作记录管理平台，支持工作记录、需求进展、高频问题、统计分析、部门与员工管理、字典维护等功能，适用于研发、项目管理等场景。

**技术栈：**
- **后端**：C++23 + SQLite3 + httplib + nlohmann/json + spdlog
- **前端**：HTML5 + CSS3 + JavaScript + Bootstrap 5 + Axios
- **构建工具**：CMake 3.16+
- **数据库**：SQLite3

---

## 主要功能模块

- **工作记录**：登记、查询、筛选、导出各类工作任务，支持文件上传和管理。
- **需求进展**：需求全流程跟踪，支持来源、影响等多维度筛选。
- **高频问题**：问题登记、进展跟踪、责任人管理。
- **统计分析**：多维度工作记录统计，支持按员工、时间等维度分析。
- **部门与员工管理**：支持部门、员工的增删改查及关联。
- **字典维护**：支持各类字典（类型、状态、来源、影响等）自定义维护。

---

## 项目结构

```
work_record/
├── build_cmake/           # CMake构建输出目录
├── include/              # 头文件目录
│   ├── config/          # 配置中心
│   ├── constants/        # 常量定义
│   ├── dao/             # 数据访问层
│   ├── model/           # 数据模型
│   ├── service/         # 业务逻辑层
│   └── util/            # 工具类
├── src/                 # 源代码目录
│   ├── config/         # 配置中心实现
│   ├── dao/            # 数据访问层实现
│   ├── service/        # 业务逻辑层实现
│   └── util/           # 工具类实现
├── config/             # 配置文件目录
│   ├── app.json        # 主配置文件
│   ├── development.json # 开发环境配置
│   └── production.json  # 生产环境配置
├── static/             # 前端静态文件
│   └── pages/work/     # 工作页面
├── third_party/        # 第三方库
│   ├── httplib/        # HTTP服务器库
│   ├── nlohmann/       # JSON库
│   ├── spdlog/         # 日志库
│   └── sqlite3/        # SQLite数据库
├── db/                 # 数据库文件
├── CMakeLists.txt      # CMake构建配置
├── build_cmake.bat     # Windows一键构建脚本
└── main.cpp           # 主程序入口
```

---

## 主要数据库表结构（简要）

| 表名                  | 说明           |
|----------------------|----------------|
| file_record          | 文件记录表     |
| department_dict      | 部门字典表     |
| employee_dict        | 员工字典表     |
| requirement_record   | 需求记录表     |
| work_record          | 工作记录表     |
| work_record_files    | 工作记录-文件关联表 |
| issue_record         | 问题记录表     |
| requirement_status_dict | 需求状态字典表 |
| work_type_dict       | 工作类型字典表 |
| affected_type_dict   | 影响类型字典表 |
| source_type_dict     | 来源类型字典表 |
| work_record_status_dict | 工作记录状态字典表 |
| issue_progress_dict  | 问题进度字典表 |

> 详细字段见 `db/work_record_design.md` 或 `db/init.sql`

---

## 主外键关系

- work_record.requirement_id → requirement_record.id
- work_record.work_type_id → work_type_dict.id
- work_record.affected_id → affected_type_dict.id
- work_record.source_type_id → source_type_dict.id
- work_record.work_record_status_id → work_record_status_dict.id
- work_record.employee_id → employee_dict.id
- requirement_record.requirement_status_id → requirement_status_dict.id
- requirement_record.affected_id → affected_type_dict.id
- requirement_record.source_type_id → source_type_dict.id
- requirement_record.employee_id → employee_dict.id
- employee_dict.department_id → department_dict.id
- issue_record.progress_id → issue_progress_dict.id
- issue_record.affected_id → affected_type_dict.id
- issue_record.source_type_id → source_type_dict.id
- issue_record.employee_id → employee_dict.id

---

## 编译与运行

### 环境要求

- **编译器**：支持 C++23 的编译器（如 GCC 11+, Clang 14+, MSVC 2022）
- **CMake**：3.16 或更高版本
- **操作系统**：Windows 10+, Linux, macOS

### Windows 一键编译

```bash
# 使用提供的批处理脚本
./build_cmake.bat
```

### 手动编译

```bash
# 创建构建目录
mkdir build_cmake
cd build_cmake

# 配置项目
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# 编译项目
cmake --build . --config Release
```

### 运行

```bash
# 进入构建目录
cd build_cmake/bin

# 运行程序
./work_record.exe
# 默认服务地址：http://localhost:8080
```

---

## 配置中心

系统采用灵活的配置中心设计，支持多环境配置管理和动态配置加载。

### 配置文件结构

- **app.json**：主配置文件，包含所有配置项的默认值
- **development.json**：开发环境特定配置，会覆盖 app.json 中的相应配置
- **production.json**：生产环境特定配置，会覆盖 app.json 中的相应配置

### 配置优先级

1. **环境变量**（最高优先级）
   - 格式：`WORK_RECORD_配置键`
   - 例如：`WORK_RECORD_SERVER_PORT=9090`

2. **环境特定配置文件**
   - development.json 或 production.json

3. **主配置文件**（最低优先级）
   - app.json

### 主要配置项

#### 服务器配置 (server)
```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "static_dir": "./static"
  }
}
```

#### 数据库配置 (database)
```json
{
  "database": {
    "path": "db/work_record.db"
  }
}
```

#### 日志配置 (logging)
```json
{
  "logging": {
    "level": "info",
    "file": "logs/app.log",
    "console_enabled": true,
    "file_enabled": true
  }
}
```

#### 上传配置 (upload)
```json
{
  "upload": {
    "base_dir": "./static/upload",
    "max_file_size": 104857600,
    "organize_by_year": true,
    "organize_by_requirement": true
  }
}
```

### 环境变量示例

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

### 配置验证

程序启动时会自动验证必需配置项：
- server.port
- server.host
- database.path
- upload.base_dir

如果验证失败，程序会输出错误信息并退出。

---

## 数据库初始化与升级

- **初始化**：首次部署请执行 `db/init.sql` 创建数据库结构。
- **升级**：如需升级部门/员工等结构，执行 `update_database.sql`。

```bash
sqlite3 db/work_record.db < db/init.sql
sqlite3 db/work_record.db < update_database.sql
```

---

## API 接口文档

### 响应格式

所有API返回标准JSON格式：

**成功响应：**
```json
{
  "success": true,
  "data": { ... },
  "message": "操作成功"
}
```

**错误响应：**
```json
{
  "success": false,
  "message": "错误信息"
}
```

### 工作记录 Work Records
- **GET /api/work_records** 查询工作记录，支持分页、筛选
- **GET /api/work_record** 根据ID获取单条工作记录
- **POST /api/get_work_records** 新建工作记录（含文件上传）
- **POST /api/update_work_record** 修改工作记录
- **DELETE /api/work_record/{id}** 删除工作记录
- **POST /api/work_record/{id}/upload** 上传文件到指定工作记录
- **GET /api/download** 下载文件
- **DELETE /api/file/{id}** 删除单个文件

### 需求进展 Requirement Records
- **GET /api/requirement_records** 查询需求记录，支持分页、筛选
- **POST /api/requirement_record** 新增/修改需求记录
- **DELETE /api/delete_requirement_record?id=xxx** 删除需求记录

### 高频问题 Issue Records
- **GET /api/issues** 查询问题记录，支持分页、筛选
- **POST /api/add_issue** 新建问题
- **POST /api/update_issue** 修改问题
- **POST /api/delete_issue** 删除问题

### 字典与基础数据
- **GET /api/dict?table=xxx** 查询字典表（如 `work_type_dict`、`affected_type_dict` 等）
- **POST /api/dict** 新增字典项
- **PUT /api/dict** 修改字典项
- **DELETE /api/dict** 删除字典项

### 部门与员工
- **GET /api/departments** 查询所有部门
- **POST /api/department** 新增部门
- **POST /api/update_department** 修改部门
- **DELETE /api/department** 删除部门
- **GET /api/employees** 查询所有员工，支持 `department_id` 筛选
- **POST /api/employee** 新增员工
- **POST /api/update_employee** 修改员工
- **DELETE /api/employee** 删除员工

### 状态字典
- **GET /api/work_record_status_dict** 获取工作记录状态字典
- **GET /api/requirement_status_dict** 获取需求状态字典
- **GET /api/issue_progress_dict** 获取问题进度字典

---

## 前端使用指南

### 主界面
- 访问 `http://localhost:8080/pages/work/work_record.html` 进入系统主界面
- 支持多Tab切换，功能涵盖工作记录、需求进展、问题、统计等
- 右下角"字典维护"按钮可进入字典管理页面

### 页面功能
- **工作记录页** (`work_tab_work.html`)：工作记录管理、文件上传、状态跟踪
- **需求进展页** (`work_tab_requirement.html`)：需求跟踪、状态管理
- **问题管理页** (`work_tab_issue.html`)：问题登记、进度跟踪
- **统计分析页** (`work_tab_stats.html`)：多维度数据统计
- **字典维护页** (`dict_maintenance.html`)：所有字典的统一管理

### 字典维护
- 通过 `dict_maintenance.html` 页面可维护所有字典（类型、状态、来源、影响、部门、员工等）
- 支持增删改查、字段自适应、下拉选择等，界面美观
- 支持实时预览和批量操作

---

## 日志系统

系统使用 spdlog 日志库，日志文件位置：
- **应用日志**：`logs/app.log`
- **日志级别**：INFO 及以上
- **日志格式**：时间戳 + 级别 + 消息内容

---

## 文件管理

- **上传目录**：`upload/` 按年份和项目分类存储
- **文件命名**：自动生成唯一文件名，保留原始扩展名
- **文件关联**：通过 `work_record_files` 表关联工作记录和文件

---

## 设计与扩展

- 详细设计文档见 `db/work_record_design.md`
- 代码分层清晰，便于二次开发和功能扩展
- 支持自定义字典、灵活扩展业务字段
- 采用 DAO-Service 分层架构，便于维护

---

## 常见问题

### 编译问题
- **CMake 配置失败**：确保 CMake 版本 >= 3.16
- **编译器不支持 C++23**：升级到最新版本的 GCC/Clang/MSVC
- **依赖库缺失**：所有第三方库已包含在 `third_party/` 目录中

### 运行问题
- **端口冲突**：如8080端口被占用，可通过配置文件或环境变量修改：
  ```bash
  # 方法1：修改 config/development.json 中的 server.port
  # 方法2：设置环境变量 WORK_RECORD_SERVER_PORT=9090
  ```
- **数据库连接失败**：检查 `db/` 目录权限和数据库文件完整性
- **文件上传失败**：检查 `upload/` 目录权限
- **配置验证失败**：检查必需配置项是否完整，查看启动日志确认配置加载情况

### 数据问题
- **数据库升级**：升级脚本如遇外键约束问题，请先备份数据
- **字典数据丢失**：可通过 `dict_maintenance.html` 重新维护

---

## 开发指南

### 代码结构
- **Config层**：配置中心，负责配置管理和环境适配
- **DAO层**：数据访问层，负责数据库操作
- **Service层**：业务逻辑层，处理业务规则
- **Util层**：工具类，提供通用功能
- **Model层**：数据模型，定义数据结构

### 添加新功能
1. 在 `include/model/` 中定义数据模型
2. 在 `include/dao/` 中定义数据访问接口
3. 在 `src/dao/` 中实现数据访问逻辑
4. 在 `include/service/` 中定义业务接口
5. 在 `src/service/` 中实现业务逻辑
6. 在 `main.cpp` 中添加API路由
7. 在 `static/pages/work/` 中添加前端页面

### 配置管理
- 新增配置项时，在 `config/app.json` 中添加默认值
- 环境特定配置在 `config/development.json` 或 `config/production.json` 中覆盖
- 敏感配置可通过环境变量设置，格式为 `WORK_RECORD_配置键`
- 配置验证逻辑在 `src/config/ConfigManager.cpp` 的 `validate()` 方法中

---

## 贡献与维护

- 欢迎提交 issue 和 PR，完善功能与文档
- 代码风格统一，建议使用 C++23 标准
- 提交前请运行测试确保功能正常

---

## 许可证

本项目采用 MIT 许可证，详见 LICENSE 文件。

---

如需详细API、字段说明、前端交互细节等，请查阅 `db/work_record_design.md` 和各页面源码。

# Work Record 系统设计文档

## 系统概述

Work Record 是一个基于 C++23 + SQLite3 + HTTP 的工作记录管理系统，采用前后端一体化架构，提供工作记录、需求管理、问题跟踪、统计分析等核心功能。

### 技术架构

- **后端框架**：C++23 + httplib + nlohmann/json + spdlog
- **数据库**：SQLite3
- **前端技术**：HTML5 + CSS3 + JavaScript + Bootstrap 5 + Axios
- **构建系统**：CMake 3.16+
- **日志系统**：spdlog

### 系统架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   前端界面      │    │   HTTP服务      │    │   SQLite数据库  │
│  (HTML/JS/CSS)  │◄──►│  (httplib)      │◄──►│   (work_record.db)│
└─────────────────┘    └─────────────────┘    └─────────────────┘
                              │
                       ┌─────────────────┐
                       │   业务逻辑层     │
                       │   (Service)     │
                       └─────────────────┘
                              │
                       ┌─────────────────┐
                       │   数据访问层     │
                       │   (DAO)         │
                       └─────────────────┘
```

---

## 数据库设计

### 核心表结构

#### 1. file_record（文件记录表）
| 字段         | 类型         | 约束           | 说明         |
| ------------ | ------------ | -------------- | ------------ |
| id           | INTEGER      | PRIMARY KEY    | 文件ID       |
| file_name    | TEXT         | NOT NULL       | 文件名称     |
| file_path    | TEXT         | NOT NULL       | 文件路径     |
| create_time  | DATETIME     | NOT NULL, 默认当前 | 创建时间 |

#### 2. department_dict（部门字典表）
| 字段         | 类型         | 约束           | 说明         |
| ------------ | ------------ | -------------- | ------------ |
| id           | INTEGER      | PRIMARY KEY    | 部门ID       |
| name         | TEXT         | NOT NULL, UNIQUE | 部门名称   |
| description  | TEXT         |                | 部门描述     |
| create_time  | DATETIME     | NOT NULL, 默认当前 | 创建时间 |

#### 3. employee_dict（员工字典表）
| 字段           | 类型     | 约束           | 说明         |
| -------------- | -------- | -------------- | ------------ |
| id             | INTEGER  | PRIMARY KEY    | 员工ID       |
| name           | TEXT     | NOT NULL       | 员工姓名     |
| employee_number| TEXT     | NOT NULL, UNIQUE | 员工编号   |
| create_time    | DATETIME | NOT NULL, 默认当前 | 创建时间 |
| department_id  | INTEGER  | NOT NULL, 外键 | 所属部门ID   |

#### 4. requirement_status_dict（需求状态字典表）
| 字段         | 类型     | 约束           | 说明         |
| ------------ | -------- | -------------- | ------------ |
| id           | INTEGER  | PRIMARY KEY    | 状态ID       |
| status       | TEXT     | NOT NULL, UNIQUE | 状态名称   |
| comment      | TEXT     |                | 状态描述     |
| requirement_status_class | TEXT | 默认 'bg-secondary' | 状态样式 |

#### 5. work_type_dict（工作类型字典表）
| 字段   | 类型   | 约束           | 说明         |
| ------ | ------ | -------------- | ------------ |
| id     | INTEGER| PRIMARY KEY    | 类型ID       |
| type   | TEXT   | NOT NULL, UNIQUE | 类型名称   |
| comment| TEXT   |                | 类型描述     |

#### 6. affected_type_dict（影响类型字典表）
| 字段   | 类型   | 约束           | 说明         |
| ------ | ------ | -------------- | ------------ |
| id     | INTEGER| PRIMARY KEY    | 影响ID       |
| affected| TEXT  | NOT NULL, UNIQUE | 影响名称   |
| comment| TEXT   |                | 影响描述     |

#### 7. source_type_dict（来源类型字典表）
| 字段   | 类型   | 约束           | 说明         |
| ------ | ------ | -------------- | ------------ |
| id     | INTEGER| PRIMARY KEY    | 来源ID       |
| type   | TEXT   | NOT NULL, UNIQUE | 来源名称   |
| comment| TEXT   |                | 来源描述     |

#### 8. work_record_status_dict（工作记录状态字典表）
| 字段        | 类型   | 约束           | 说明         |
| ----------- | ------ | -------------- | ------------ |
| id          | INTEGER| PRIMARY KEY    | 状态ID       |
| status_name | TEXT   | NOT NULL, UNIQUE | 状态名称   |
| status_class| TEXT   |                | 状态样式     |

#### 9. issue_progress_dict（问题进度字典表）
| 字段          | 类型   | 约束           | 说明         |
| ------------- | ------ | -------------- | ------------ |
| id            | INTEGER| PRIMARY KEY    | 进度ID       |
| progress      | TEXT   | NOT NULL, UNIQUE | 进度名称   |
| progress_class| TEXT   | NOT NULL, 默认 'bg-secondary' | 进度样式 |
| comment       | TEXT   |                | 进度描述     |

#### 10. work_record（工作记录表）
| 字段                 | 类型     | 约束           | 说明         |
| -------------------- | -------- | -------------- | ------------ |
| id                   | INTEGER  | PRIMARY KEY    | 工作记录ID       |
| requirement_id       | INTEGER  | NOT NULL, 外键 | 需求ID       |
| create_time          | DATETIME | NOT NULL, 默认当前 | 创建时间 |
| work_type_id         | INTEGER  | NOT NULL, 外键 | 工作类型ID   |
| work_content         | TEXT     | NOT NULL       | 工作内容     |
| affected_id          | INTEGER  | NOT NULL, 外键 | 影响类型ID   |
| source_type_id       | INTEGER  | NOT NULL, 外键 | 来源类型ID   |
| work_record_status_id| INTEGER  | NOT NULL, 外键 | 工作记录状态ID   |
| completion_time      | DATETIME | NOT NULL, 默认当前 | 完成时间 |
| employee_id          | INTEGER  | 外键           | 完成人ID     |

#### 11. work_record_files（工作记录文件关联表）
| 字段           | 类型     | 约束           | 说明         |
| -------------- | -------- | -------------- | ------------ |
| id             | INTEGER  | PRIMARY KEY    | 记录ID       |
| work_record_id | INTEGER  | NOT NULL, 外键 | ID       |
| file_record_id | INTEGER  | NOT NULL, 外键 | 文件ID       |

#### 12. issue_record（高频问题表）
| 字段              | 类型     | 约束           | 说明         |
| ----------------- | -------- | -------------- | ------------ |
| id                | INTEGER  | PRIMARY KEY    | 问题ID       |
| reported_by       | TEXT     | NOT NULL       | 来源工作记录/报告|
| issue_title       | TEXT     | NOT NULL       | 问题标题     |
| description       | TEXT     | NOT NULL       | 问题描述     |
| progress_id       | INTEGER  | NOT NULL, 外键 | 进度ID       |
| responsible_person| TEXT     |                | 责任人       |
| affected_id       | INTEGER  | NOT NULL, 外键 | 影响类型ID   |
| source_type_id    | INTEGER  | NOT NULL, 外键 | 来源类型ID   |
| create_time       | DATETIME | NOT NULL, 默认当前 | 创建时间 |
| update_time       | DATETIME | NOT NULL, 默认当前 | 更新时间 |
| employee_id       | INTEGER  | 外键           | 责任人ID     |

#### 13. requirement_record（需求记录表）
| 字段                 | 类型     | 约束           | 说明         |
| -------------------- | -------- | -------------- | ------------ |
| id                   | INTEGER  | PRIMARY KEY    | 需求ID       |
| create_time          | DATETIME | NOT NULL, 默认当前 | 创建时间 |
| update_time          | DATETIME | NOT NULL, 默认当前 | 更新时间 |
| title                | TEXT     | NOT NULL       | 需求名称     |
| requirement_status_id| INTEGER  | NOT NULL, 外键 | 需求状态ID   |
| affected_id          | INTEGER  | NOT NULL, 外键 | 影响类型ID   |
| source_type_id       | INTEGER  | NOT NULL, 外键 | 来源类型ID   |
| employee_id          | INTEGER  | 外键           | 负责人ID     |

---

## 主外键关系

### 核心关系图
```
work_record
├── requirement_id → requirement_record.id
├── work_type_id → work_type_dict.id
├── affected_id → affected_type_dict.id
├── source_type_id → source_type_dict.id
├── work_record_status_id → work_record_status_dict.id
└── employee_id → employee_dict.id

requirement_record
├── requirement_status_id → requirement_status_dict.id
├── affected_id → affected_type_dict.id
├── source_type_id → source_type_dict.id
└── employee_id → employee_dict.id

employee_dict
└── department_id → department_dict.id

issue_record
├── progress_id → issue_progress_dict.id
├── affected_id → affected_type_dict.id
├── source_type_id → source_type_dict.id
└── employee_id → employee_dict.id

work_record_files
├── work_record_id → work_record.id
└── file_record_id → file_record.id
```

---

## API 设计

### 响应格式规范

#### 成功响应
```json
{
  "success": true,
  "data": {
    // 具体数据内容
  },
  "message": "操作成功"
}
```

#### 错误响应
```json
{
  "success": false,
  "message": "错误信息"
}
```

#### 分页响应
```json
{
  "success": true,
  "data": {
    "records": [...],
    "total": 100,
    "page": 1,
    "pageSize": 20
  }
}
```

### 核心API接口

#### 工作记录管理
- `GET /api/work_records` - 查询工作记录（支持分页、筛选）
- `GET /api/work_record` - 根据ID获取单条工作记录
- `POST /api/get_work_records` - 新建工作记录（含文件上传）
- `POST /api/update_work_record` - 修改工作记录
- `DELETE /api/work_record/{id}` - 删除工作记录

#### 文件管理
- `POST /api/work_record/{id}/upload` - 上传文件到指定工作记录
- `GET /api/download` - 下载文件
- `DELETE /api/file/{id}` - 删除单个文件

#### 需求管理
- `GET /api/requirement_records` - 查询需求记录
- `POST /api/requirement_record` - 新增/修改需求记录
- `DELETE /api/delete_requirement_record` - 删除需求记录

#### 问题管理
- `GET /api/issues` - 查询问题记录
- `POST /api/add_issue` - 新建问题
- `POST /api/update_issue` - 修改问题
- `POST /api/delete_issue` - 删除问题

#### 字典管理
- `GET /api/dict?table=xxx` - 查询字典表
- `POST /api/dict` - 新增字典项
- `PUT /api/dict` - 修改字典项
- `DELETE /api/dict` - 删除字典项

#### 部门员工管理
- `GET /api/departments` - 查询所有部门
- `POST /api/department` - 新增部门
- `POST /api/update_department` - 修改部门
- `DELETE /api/department` - 删除部门
- `GET /api/employees` - 查询所有员工
- `POST /api/employee` - 新增员工
- `POST /api/update_employee` - 修改员工
- `DELETE /api/employee` - 删除员工

---

## 业务逻辑设计

### 核心业务流程

#### 1. 工作记录创建流程
```
用户输入 → 前端验证 → API调用 → 业务验证 → 数据库操作 → 文件处理 → 返回结果
```

#### 2. 文件上传流程
```
文件选择 → 前端验证 → 上传API → 文件存储 → 数据库记录 → 关联工作记录 → 返回结果
```

#### 3. 需求跟踪流程
```
需求创建 → 状态更新 → 工作记录关联 → 进度跟踪 → 统计分析
```

### 数据验证规则

#### 工作记录验证
- requirement_id: 必须存在且有效
- work_content: 不能为空
- work_type_id: 必须为有效字典值
- affected_id: 必须为有效字典值
- source_type_id: 必须为有效字典值

#### 文件上传验证
- 文件大小限制：50MB
- 支持格式：常见文档、图片、压缩包
- 文件名：自动生成唯一标识

#### 字典数据验证
- 名称唯一性检查
- 外键约束检查
- 删除前检查引用关系

---

## 前端架构

### 页面结构
```
work_record.html (主框架)
├── work_tab_work.html (工作记录)
├── work_tab_requirement.html (需求管理)
├── work_tab_issue.html (问题管理)
├── work_tab_stats.html (统计分析)
└── dict_maintenance.html (字典维护)
```

### 技术栈
- **UI框架**：Bootstrap 5
- **HTTP客户端**：Axios
- **数据处理**：原生JavaScript
- **样式**：CSS3 + Bootstrap组件

### 核心功能模块

#### 1. 工作记录管理
- 工作记录列表展示（分页、筛选）
- 工作记录创建/编辑表单
- 文件上传/下载
- 状态管理

#### 2. 需求管理
- 需求列表展示
- 需求创建/编辑
- 状态跟踪
- 关联工作记录

#### 3. 问题管理
- 问题列表展示
- 问题创建/编辑
- 进度跟踪
- 责任人管理

#### 4. 统计分析
- 多维度统计图表
- 数据导出
- 趋势分析

#### 5. 字典维护
- 统一字典管理界面
- 动态表单生成
- 实时预览
- 批量操作

---

## 系统配置

### 数据库配置
- **数据库文件**：`db/work_record.db`
- **字符编码**：UTF-8
- **外键约束**：启用
- **WAL模式**：启用（提高并发性能）

### 服务器配置
- **监听地址**：0.0.0.0:8080
- **静态文件目录**：`./static`
- **上传目录**：`upload/`
- **日志级别**：INFO

### 文件管理配置
- **上传目录结构**：`upload/年份/项目名称/`
- **文件命名规则**：`{原始名}_{时间戳}.{扩展名}`
- **大小限制**：50MB
- **支持格式**：常见文档、图片、压缩包

---

## 安全设计

### 数据安全
- SQL注入防护：参数化查询
- XSS防护：输入验证和输出编码
- 文件上传安全：类型检查、大小限制

### 访问控制
- 文件访问控制：基于数据库记录
- API访问控制：统一的错误处理
- 数据验证：前后端双重验证

---

## 性能优化

### 数据库优化
- 索引设计：主键、外键、常用查询字段
- 查询优化：避免N+1查询问题
- 连接池：复用数据库连接

### 前端优化
- 分页加载：减少数据传输量
- 缓存策略：字典数据缓存
- 异步处理：非阻塞操作

---

## 扩展性设计

### 模块化架构
- DAO层：数据访问抽象
- Service层：业务逻辑封装
- Util层：通用工具类
- Model层：数据结构定义

### 字典系统
- 支持自定义字典类型
- 动态字段扩展
- 多语言支持预留

### 插件机制
- 服务接口标准化
- 事件驱动架构
- 配置化管理

---

## 部署说明

### 开发环境
1. 安装CMake 3.16+
2. 安装支持C++23的编译器
3. 运行 `build_cmake.bat` 构建项目
4. 启动服务：`./work_record.exe`

### 生产环境
1. 编译Release版本
2. 配置数据库和日志目录
3. 设置防火墙规则
4. 配置反向代理（可选）

---

## 维护指南

### 日志管理
- 应用日志：`logs/app.log`
- 错误日志：自动记录异常信息
- 访问日志：HTTP请求记录

### 数据备份
- 定期备份数据库文件
- 备份上传文件目录
- 配置文件备份

### 性能监控
- 数据库查询性能
- 文件上传下载速度
- 内存使用情况

---

## 版本历史

### v1.0.0 (当前版本)
- 基础工作记录管理
- 需求跟踪功能
- 问题管理功能
- 统计分析功能
- 字典维护系统
- 文件上传下载
- 部门员工管理

### 未来规划
- 工作流引擎
- 报表导出功能
- 移动端适配
- 多租户支持
- API文档自动生成

---

## 技术支持

- **文档**：详见 `README.md`
- **源码**：查看各模块实现
- **问题反馈**：提交Issue
- **功能建议**：提交PR

---

*本文档最后更新：2024年12月*
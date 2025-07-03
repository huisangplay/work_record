# 工作记录管理系统

## 项目简介

本系统为 C++/SQLite/HTML 前后端一体化的工作记录管理平台，支持工作记录、需求进展、高频问题、统计分析、部门与员工管理、字典维护等功能，适用于研发、项目管理等场景。

---

## 主要功能模块

- **工作记录**：登记、查询、筛选、导出各类工作任务。
- **需求进展**：需求全流程跟踪，支持来源、影响等多维度筛选。
- **高频问题**：问题登记、进展跟踪、责任人管理。
- **统计分析**：多维度工单统计，支持按员工、时间等维度分析。
- **部门与员工管理**：支持部门、员工的增删改查及关联。
- **字典维护**：支持各类字典（类型、状态、来源、影响等）自定义维护。

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
| work_record_status_dict | 工单状态字典表 |
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

### Windows 一键编译

```bash
./build.bat
```

### 手动编译

```bash
g++ -std=c++20 -o myapp.exe main.cpp db/sqlite3.c -lWs2_32 -lpthread
```

### 运行

```bash
./myapp.exe
# 默认服务地址：http://localhost:8080
```

---

## 数据库初始化与升级

- **初始化**：首次部署请执行 `db/init.sql` 创建数据库结构。
- **升级**：如需升级部门/员工等结构，执行 `update_database.sql`。

```bash
sqlite3 db/work_record.db < db/init.sql
sqlite3 db/work_record.db < update_database.sql
```

---

## 主要接口文档

### 工作记录 Work Records
- **GET /api/work_records** 查询工作记录，支持分页、筛选。
- **POST /api/get_work_records** 新建工作记录（含文件上传）。
- **POST /api/update_work_record** 修改工作记录。
- **DELETE /api/work_record/{id}** 删除工作记录。
- **POST /api/work_record/{id}/upload** 上传文件到指定工单。
- **GET /api/download** 下载文件。
- **DELETE /api/file/{id}** 删除单个文件。

### 需求进展 Requirement Records
- **GET /api/requirement_records** 查询需求记录，支持分页、筛选。
- **POST /api/requirement_record** 新增/修改需求记录。
- **DELETE /api/delete_requirement_record?id=xxx** 删除需求记录。

### 高频问题 Issue Records
- **GET /api/issues** 查询问题记录，支持分页、筛选。
- **POST /api/add_issue** 新建问题。
- **POST /api/update_issue** 修改问题。
- **POST /api/delete_issue** 删除问题。

### 字典与基础数据
- **GET /api/dict?table=xxx** 查询字典表（如 `work_type_dict`、`affected_type_dict` 等）。
- **POST /api/dict** 新增字典项。
- **PUT /api/dict** 修改字典项。
- **DELETE /api/dict** 删除字典项。

### 部门与员工
- **GET /api/departments** 查询所有部门。
- **POST /api/department** 新增部门。
- **POST /api/update_department** 修改部门。
- **DELETE /api/department** 删除部门。
- **GET /api/employees** 查询所有员工，支持 `department_id` 筛选。
- **POST /api/employee** 新增员工。
- **POST /api/update_employee** 修改员工。
- **DELETE /api/employee** 删除员工。

### 其它
- **GET /api/work_record_status_dict** 获取工单状态字典。
- **GET /api/requirement_status_dict** 获取需求状态字典。
- **GET /api/issue_progress_dict** 获取问题进度字典。

**所有接口返回均为标准JSON，错误时返回 `{ error: '错误信息' }`，成功时返回 `{ success: true }` 或数据对象。**

如需详细字段说明，请查阅 `db/work_record_design.md` 或源码。

---

## 前端入口与使用

- 访问 `static/pages/work_record.html` 即可进入系统主界面。
- 支持多Tab切换，功能涵盖工作记录、需求进展、问题、统计等。
- 右下角"字典维护"按钮可进入字典管理页面。

---

## 字典维护

- 通过 `dict_maintenance.html` 页面可维护所有字典（类型、状态、来源、影响、部门、员工等）。
- 支持增删改查、字段自适应、下拉选择等，界面美观。

---

## 设计与扩展

- 详细设计文档见 `db/work_record_design.md`。
- 代码分层清晰，便于二次开发和功能扩展。
- 支持自定义字典、灵活扩展业务字段。

---

## 常见问题

- **编译失败**：请确保已安装 g++、sqlite3 等依赖。
- **端口冲突**：如8080端口被占用，可在 `main.cpp` 修改监听端口。
- **数据库升级**：升级脚本如遇外键约束问题，请先备份数据。

---

## 贡献与维护

- 欢迎提交 issue 和 PR，完善功能与文档。
- 代码风格统一，建议使用 C++17 及以上标准。

---

如需详细API、字段说明、前端交互细节等，请查阅 `DEPARTMENT_EMPLOYEE_README.md` 和各页面源码。

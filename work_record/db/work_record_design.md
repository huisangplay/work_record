# work_record 设计文档

---

## 主要表结构

### 1. file_record（文件记录表）
| 字段         | 类型         | 约束           | 说明         |
| ------------ | ------------ | -------------- | ------------ |
| id           | INTEGER      | PRIMARY KEY    | 文件ID       |
| file_name    | TEXT         | NOT NULL       | 文件名称     |
| file_path    | TEXT         | NOT NULL       | 文件路径     |
| create_time  | DATETIME     | NOT NULL, 默认当前 | 创建时间 |

### 2. department_dict（部门字典表）
| 字段         | 类型         | 约束           | 说明         |
| ------------ | ------------ | -------------- | ------------ |
| id           | INTEGER      | PRIMARY KEY    | 部门ID       |
| name         | TEXT         | NOT NULL, UNIQUE | 部门名称   |
| description  | TEXT         |                | 部门描述     |
| create_time  | DATETIME     | NOT NULL, 默认当前 | 创建时间 |

### 3. employee_dict（员工字典表）
| 字段           | 类型     | 约束           | 说明         |
| -------------- | -------- | -------------- | ------------ |
| id             | INTEGER  | PRIMARY KEY    | 员工ID       |
| name           | TEXT     | NOT NULL       | 员工姓名     |
| employee_number| TEXT     | NOT NULL, UNIQUE | 员工编号   |
| create_time    | DATETIME | NOT NULL, 默认当前 | 创建时间 |
| department_id  | INTEGER  | NOT NULL, 外键 | 所属部门ID   |

### 4. requirement_status_dict（需求状态字典表）
| 字段         | 类型     | 约束           | 说明         |
| ------------ | -------- | -------------- | ------------ |
| id           | INTEGER  | PRIMARY KEY    | 状态ID       |
| status       | TEXT     | NOT NULL, UNIQUE | 状态名称   |
| comment      | TEXT     |                | 状态描述     |
| requirement_status_class | TEXT | 默认 'bg-secondary' | 状态样式 |

### 5. work_type_dict（工作类型字典表）
| 字段   | 类型   | 约束           | 说明         |
| ------ | ------ | -------------- | ------------ |
| id     | INTEGER| PRIMARY KEY    | 类型ID       |
| type   | TEXT   | NOT NULL, UNIQUE | 类型名称   |
| comment| TEXT   |                | 类型描述     |

### 6. affected_type_dict（影响类型字典表）
| 字段   | 类型   | 约束           | 说明         |
| ------ | ------ | -------------- | ------------ |
| id     | INTEGER| PRIMARY KEY    | 影响ID       |
| affected| TEXT  | NOT NULL, UNIQUE | 影响名称   |
| comment| TEXT   |                | 影响描述     |

### 7. source_type_dict（来源类型字典表）
| 字段   | 类型   | 约束           | 说明         |
| ------ | ------ | -------------- | ------------ |
| id     | INTEGER| PRIMARY KEY    | 来源ID       |
| type   | TEXT   | NOT NULL, UNIQUE | 来源名称   |
| comment| TEXT   |                | 来源描述     |

### 8. work_record_status_dict（工单状态字典表）
| 字段        | 类型   | 约束           | 说明         |
| ----------- | ------ | -------------- | ------------ |
| id          | INTEGER| PRIMARY KEY    | 状态ID       |
| status_name | TEXT   | NOT NULL, UNIQUE | 状态名称   |
| status_class| TEXT   |                | 状态样式     |

### 9. issue_progress_dict（问题进度字典表）
| 字段          | 类型   | 约束           | 说明         |
| ------------- | ------ | -------------- | ------------ |
| id            | INTEGER| PRIMARY KEY    | 进度ID       |
| progress      | TEXT   | NOT NULL, UNIQUE | 进度名称   |
| progress_class| TEXT   | NOT NULL, 默认 'bg-secondary' | 进度样式 |
| comment       | TEXT   |                | 进度描述     |

### 10. work_record（工作记录表）
| 字段                 | 类型     | 约束           | 说明         |
| -------------------- | -------- | -------------- | ------------ |
| id                   | INTEGER  | PRIMARY KEY    | 工单ID       |
| requirement_id       | INTEGER  | NOT NULL, 外键 | 需求ID       |
| create_time          | DATETIME | NOT NULL, 默认当前 | 创建时间 |
| work_type_id         | INTEGER  | NOT NULL, 外键 | 工作类型ID   |
| work_content         | TEXT     | NOT NULL       | 工作内容     |
| affected_id          | INTEGER  | NOT NULL, 外键 | 影响类型ID   |
| source_type_id       | INTEGER  | NOT NULL, 外键 | 来源类型ID   |
| work_record_status_id| INTEGER  | NOT NULL, 外键 | 工单状态ID   |
| update_time          | DATETIME | NOT NULL, 默认当前 | 更新时间 |
| employee_id          | INTEGER  | 外键           | 完成人ID     |

### 11. work_record_files（工单文件关联表）
| 字段           | 类型     | 约束           | 说明         |
| -------------- | -------- | -------------- | ------------ |
| id             | INTEGER  | PRIMARY KEY    | 记录ID       |
| work_record_id | INTEGER  | NOT NULL, 外键 | 工单ID       |
| file_record_id | INTEGER  | NOT NULL, 外键 | 文件ID       |

### 12. issue_record（高频问题表）
| 字段              | 类型     | 约束           | 说明         |
| ----------------- | -------- | -------------- | ------------ |
| id                | INTEGER  | PRIMARY KEY    | 问题ID       |
| reported_by       | TEXT     | NOT NULL       | 来源工单/报告|
| issue_title       | TEXT     | NOT NULL       | 问题标题     |
| description       | TEXT     | NOT NULL       | 问题描述     |
| progress_id       | INTEGER  | NOT NULL, 外键 | 进度ID       |
| responsible_person| TEXT     |                | 责任人       |
| affected_id       | INTEGER  | NOT NULL, 外键 | 影响类型ID   |
| source_type_id    | INTEGER  | NOT NULL, 外键 | 来源类型ID   |
| create_time       | DATETIME | NOT NULL, 默认当前 | 创建时间 |
| update_time       | DATETIME | NOT NULL, 默认当前 | 更新时间 |
| employee_id       | INTEGER  | 外键           | 责任人ID     |

### 13. requirement_record（需求记录表）
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

## 主要业务流
- **新建工作记录**：涉及 requirement_record、work_record、employee_dict、work_type_dict 等表。
- **新建问题**：涉及 issue_record、employee_dict、issue_progress_dict、affected_type_dict、source_type_dict。
- **文件上传**：涉及 file_record、work_record_files。

---

## 扩展建议
- 所有字典表（如类型、状态、来源、影响等）均可自定义扩展。
- 支持多部门、多员工、多维度统计分析。
- 代码分层清晰，便于二次开发和功能扩展。

---

## 其它说明
- 详细API、字段说明、前端交互细节等，请查阅 `README.md`、源码和页面注释。
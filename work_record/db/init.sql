PRAGMA foreign_keys=ON;
BEGIN TRANSACTION;
CREATE TABLE file_record (

    id INTEGER PRIMARY KEY AUTOINCREMENT,

    file_name TEXT NOT NULL,

    file_path TEXT NOT NULL,

    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP

);
INSERT INTO file_record VALUES(121,'build.bat','/upload/2025/啊手动阀/build_1751540068.bat','2025-07-03 10:54:28');
INSERT INTO file_record VALUES(122,'README.md','/upload/2025/啊手动阀/README_1751540238.md','2025-07-03 10:57:18');
CREATE TABLE requirement_status_dict (

    id INTEGER PRIMARY KEY AUTOINCREMENT,

    status TEXT NOT NULL UNIQUE,

    comment TEXT

, requirement_status_class TEXT DEFAULT 'bg-secondary');
INSERT INTO requirement_status_dict VALUES(1,'需求确认中','需求初步确认阶段','bg-secondary');
INSERT INTO requirement_status_dict VALUES(2,'业务方案编制中','业务方案编写阶段','bg-info');
INSERT INTO requirement_status_dict VALUES(3,'详细设计方案编制中','技术方案设计阶段','bg-primary');
INSERT INTO requirement_status_dict VALUES(4,'开发中','功能开发阶段','bg-warning');
INSERT INTO requirement_status_dict VALUES(5,'测试中','测试验证阶段','bg-danger');
INSERT INTO requirement_status_dict VALUES(6,'业务确认中','业务验收阶段','bg-light');
INSERT INTO requirement_status_dict VALUES(7,'待上线','准备上线阶段','bg-dark');
INSERT INTO requirement_status_dict VALUES(8,'已上线','已完成上线','bg-success');
INSERT INTO requirement_status_dict VALUES(9,'进行中','通用进行中状态','bg-primary');
CREATE TABLE IF NOT EXISTS "source_type_dict" (

    id INTEGER PRIMARY KEY AUTOINCREMENT,

    type TEXT NOT NULL UNIQUE,

    comment TEXT

);
INSERT INTO source_type_dict VALUES(1,'C919','国产大飞机型号');
INSERT INTO source_type_dict VALUES(2,'C909','宽体客机型号');
INSERT INTO source_type_dict VALUES(3,'CXF','新型概念机型');
INSERT INTO source_type_dict VALUES(4,'ALL','通用所有机型');
CREATE TABLE work_type_dict (

    id INTEGER PRIMARY KEY AUTOINCREMENT,

    type TEXT NOT NULL UNIQUE,

    comment TEXT

);
INSERT INTO work_type_dict VALUES(1,'需求对接','需求分析沟通');
INSERT INTO work_type_dict VALUES(2,'功能优化','已有功能改进');
INSERT INTO work_type_dict VALUES(3,'bug修复','缺陷修复工作');
INSERT INTO work_type_dict VALUES(4,'业务方案编制','业务文档编写工作');
INSERT INTO work_type_dict VALUES(5,'模块开发','新功能开发');
INSERT INTO work_type_dict VALUES(6,'详细设计文档编制','详细设计文档编制');
INSERT INTO work_type_dict VALUES(7,'测试问题修复','测试阶段问题处理');
CREATE TABLE issue_progress_dict (

    id INTEGER PRIMARY KEY AUTOINCREMENT,

    progress TEXT NOT NULL UNIQUE,

    progress_class TEXT NOT NULL DEFAULT 'bg-secondary',

    comment TEXT

);
INSERT INTO issue_progress_dict VALUES(1,'未处理','bg-secondary','待处理');
INSERT INTO issue_progress_dict VALUES(2,'处理中','bg-info','处理中');
INSERT INTO issue_progress_dict VALUES(3,'已解决','bg-success','已解决');
INSERT INTO issue_progress_dict VALUES(4,'已关闭','bg-dark','已关闭');
INSERT INTO issue_progress_dict VALUES(5,'挂起','bg-warning','挂起');
CREATE TABLE work_record_status_dict (

  id INTEGER PRIMARY KEY AUTOINCREMENT,

  status_name TEXT NOT NULL UNIQUE,

  status_class TEXT

);
INSERT INTO work_record_status_dict VALUES(1,'已完成','bg-success');
INSERT INTO work_record_status_dict VALUES(2,'待确认','bg-warning');
INSERT INTO work_record_status_dict VALUES(3,'处理中','bg-primary');
CREATE TABLE work_record_files (

    id INTEGER PRIMARY KEY AUTOINCREMENT,

    work_record_id INTEGER NOT NULL,

    file_record_id INTEGER NOT NULL,

    FOREIGN KEY (work_record_id) REFERENCES work_record(id) ON DELETE CASCADE,

    FOREIGN KEY (file_record_id) REFERENCES file_record(id) ON DELETE CASCADE

);
INSERT INTO work_record_files VALUES(102,1,121);
INSERT INTO work_record_files VALUES(103,2,122);
CREATE TABLE [affected_type_dict](

  [id] INTEGER PRIMARY KEY AUTOINCREMENT, 

  [affected] TEXT NOT NULL UNIQUE, 

  [comment] TEXT);
INSERT INTO affected_type_dict VALUES(1,'IDEAL','集成开发环境系统');
INSERT INTO affected_type_dict VALUES(2,'XBOM','物料管理系统');
INSERT INTO affected_type_dict VALUES(3,'CPC','协同设计系统');
INSERT INTO affected_type_dict VALUES(4,'ALL','通用所有系统');
CREATE TABLE department_dict (

    id INTEGER PRIMARY KEY AUTOINCREMENT,

    name TEXT NOT NULL UNIQUE,

    description TEXT,

    create_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP

);
INSERT INTO department_dict VALUES(2,'业务部','负责业务需求分析','2025-01-01 00:00:00');
INSERT INTO department_dict VALUES(7,'構型','構型控制','2025-07-02 14:29:38');
CREATE TABLE [employee_dict](

  [id] INTEGER PRIMARY KEY AUTOINCREMENT, 

  [name] TEXT NOT NULL, 

  [employee_number] TEXT NOT NULL UNIQUE, 

  [create_time] DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 

  [department_id] INTEGER NOT NULL REFERENCES [department_dict]([id]));
INSERT INTO employee_dict VALUES(5,'132','132','2025-06-30 12:49:22',2);
INSERT INTO employee_dict VALUES(6,'test','203434','2025-07-02 14:29:52',7);
CREATE TABLE IF NOT EXISTS "issue_record"(

  [id] INTEGER PRIMARY KEY AUTOINCREMENT, 

  [reported_by] TEXT NOT NULL, 

  [issue_title] TEXT NOT NULL, 

  [description] TEXT NOT NULL, 

  [progress_id] INTEGER NOT NULL REFERENCES [issue_progress_dict]([id]), 

  [responsible_person] TEXT, 

  [affected_id] INTEGER NOT NULL REFERENCES [affected_type_dict]([id]), 

  [source_type_id] INTEGER NOT NULL REFERENCES [source_type_dict]([id]), 

  [create_time] DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 

  [update_time] DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 

  [employee_id] INTEGER REFERENCES [employee_dict]([id]));
INSERT INTO issue_record VALUES(1,'用户反馈','飞行控制响应延迟','在海拔3000米以上出现控制延迟',3,'王工程师',1,1,'2025-06-27 07:59:03','2025-06-27 07:59:03',NULL);
INSERT INTO issue_record VALUES(2,'测试报告','物料编码冲突','批量导入时出现重复编码',3,'张设计师',2,2,'2025-06-27 07:59:03','2025-06-27 07:59:03',NULL);
INSERT INTO issue_record VALUES(3,'运维监控','协同设计文件锁异常','偶发文件锁定无法释放',5,'',2,1,'2025-06-27 07:59:03','2025-06-27 07:59:03',5);
CREATE TABLE [requirement_record](

  [id] INTEGER PRIMARY KEY AUTOINCREMENT, 

  [create_time] DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 

  [update_time] DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 

  [title] TEXT NOT NULL, 

  [requirement_status_id] INTEGER NOT NULL REFERENCES [requirement_status_dict]([id]), 

  [affected_id] INTEGER NOT NULL REFERENCES [affected_type_dict]([id]), 

  [source_type_id] INTEGER NOT NULL REFERENCES [source_type_dict]([id]), 

  [employee_id] INTEGER REFERENCES [employee_dict]([id]));
INSERT INTO requirement_record VALUES(23,'2025-07-03 10:54:04','2025-07-03 10:54:04','啊手动阀',3,2,2,5);
CREATE TABLE [work_record](

  [id] INTEGER PRIMARY KEY AUTOINCREMENT, 

  [requirement_id] INTEGER NOT NULL REFERENCES [requirement_record]([id]), 

  [create_time] DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 

  [work_type_id] INTEGER NOT NULL REFERENCES [work_type_dict]([id]), 

  [work_content] TEXT NOT NULL, 

  [affected_id] INTEGER NOT NULL REFERENCES [affected_type_dict]([id]), 

  [source_type_id] INTEGER NOT NULL REFERENCES [source_type_dict]([id]), 

  [work_record_status_id] INTEGER NOT NULL REFERENCES [work_record_status_dict]([id]), 

  [completion_time] DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, 

  [employee_id] INTEGER);
INSERT INTO work_record VALUES(1,23,'2025-07-03 10:54:28',2,'撒大蘇打搜索的',2,2,2,'2025-03-14T10:54',6);
INSERT INTO work_record VALUES(2,23,'2025-07-03 10:57:18',2,'撒大蘇打搜索的',1,1,1,'2025-07-03 10:57:18',5);
INSERT INTO sqlite_sequence VALUES('requirement_status_dict',12);
INSERT INTO sqlite_sequence VALUES('source_type_dict',7);
INSERT INTO sqlite_sequence VALUES('work_type_dict',10);
INSERT INTO sqlite_sequence VALUES('file_record',122);
INSERT INTO sqlite_sequence VALUES('issue_progress_dict',9);
INSERT INTO sqlite_sequence VALUES('work_record_status_dict',8);
INSERT INTO sqlite_sequence VALUES('work_record_files',103);
INSERT INTO sqlite_sequence VALUES('affected_type_dict',7);
INSERT INTO sqlite_sequence VALUES('department_dict',7);
INSERT INTO sqlite_sequence VALUES('employee_dict',6);
INSERT INTO sqlite_sequence VALUES('issue_record',11);
INSERT INTO sqlite_sequence VALUES('requirement_record',23);
INSERT INTO sqlite_sequence VALUES('work_record',2);
COMMIT;

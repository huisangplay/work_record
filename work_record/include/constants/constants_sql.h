#pragma once
namespace constants_sql {
    // =====================
    // Work Record 相关SQL
    // =====================
    constexpr const char* SQL_INSERT_WORK_RECORD = "INSERT INTO work_record (requirement_id, work_type_id, affected_id, source_type_id, work_record_status_id, work_content, employee_id, completion_time) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    constexpr const char* SQL_SELECT_WORK_RECORD_BY_ID = "SELECT id, requirement_id, work_type_id, affected_id, source_type_id, work_record_status_id, work_content, create_time, completion_time, employee_id FROM work_record WHERE id = ?;";
    constexpr const char* SQL_UPDATE_WORK_RECORD = "UPDATE work_record SET requirement_id=?, work_type_id=?, affected_id=?, source_type_id=?, work_record_status_id=?, work_content=?, employee_id=?, completion_time=? WHERE id=?;";
    constexpr const char* SQL_SELECT_WORK_RECORD_PAGED_BASE = "SELECT w.id, w.requirement_id, r.title as requirement_title, w.work_type_id, t.type as work_type, w.affected_id, a.affected as affected_name, w.source_type_id, m.type as source_type_name, w.work_record_status_id, d.status_name, d.status_class as status_class, w.work_content, w.create_time, w.completion_time, w.employee_id, e.name as employee_name, e.department_id FROM work_record w LEFT JOIN requirement_record r ON w.requirement_id = r.id LEFT JOIN work_type_dict t ON w.work_type_id = t.id LEFT JOIN affected_type_dict a ON w.affected_id = a.id LEFT JOIN source_type_dict m ON w.source_type_id = m.id LEFT JOIN work_record_status_dict d ON w.work_record_status_id = d.id LEFT JOIN employee_dict e ON w.employee_id = e.id ";
    constexpr const char* SQL_COUNT_WORK_RECORD_BASE = "SELECT COUNT(*) FROM work_record w ";
    constexpr const char* SQL_SELECT_WORK_FILES_BY_IDS_BASE = "SELECT wf.work_record_id, f.id, f.file_name, f.file_path, f.create_time FROM work_record_files wf JOIN file_record f ON wf.file_record_id = f.id WHERE wf.work_record_id IN ";
    constexpr const char* SQL_SELECT_WORK_FILES = "SELECT f.id, f.file_path FROM work_record_files wf JOIN file_record f ON wf.file_record_id = f.id WHERE wf.work_record_id = ?;";
    constexpr const char* SQL_INSERT_FILE_RECORD = "INSERT INTO file_record (file_name, file_path) VALUES (?, ?);";
    constexpr const char* SQL_INSERT_WORK_FILE_REL = "INSERT INTO work_record_files (work_record_id, file_record_id) VALUES (?, ?);";
    constexpr const char* SQL_DELETE_WORK_FILE_REL = "DELETE FROM work_record_files WHERE work_record_id = ?;";
    constexpr const char* SQL_DELETE_FILE_RECORD = "DELETE FROM file_record WHERE id = ?;";
    constexpr const char* SQL_DELETE_WORK_RECORD = "DELETE FROM work_record WHERE id = ?;";
    constexpr const char* SQL_SELECT_FILES_BY_REQUIREMENT_ID = "SELECT f.file_path FROM workrecord_file f JOIN work_record w ON f.work_record_id = w.id WHERE w.requirement_id = ?;";
    // ... 可继续补充 work_record 相关SQL

    // =====================
    // Requirement 相关SQL
    // =====================
    constexpr const char* SQL_INSERT_REQUIREMENT_RECORD = "INSERT INTO requirement_record (title, requirement_status_id, source_type_id, affected_id, employee_id) VALUES (?, ?, ?, ?, ?);";
    constexpr const char* SQL_UPDATE_REQUIREMENT_RECORD = "UPDATE requirement_record SET title=?, requirement_status_id=?, source_type_id=?, affected_id=?, employee_id=? WHERE id=?;";
    constexpr const char* SQL_DELETE_REQUIREMENT_RECORD = "DELETE FROM requirement_record WHERE id = ?;";
    constexpr const char* SQL_SELECT_ALL_REQUIREMENT_RECORD = "SELECT r.id, r.title, r.requirement_status_id, d.status as requirement_status_name, d.requirement_status_class as requirement_status_class, r.affected_id, a.affected as affected_name, r.source_type_id, m.type as source_type_name, r.create_time, r.update_time, r.employee_id, e.name as employee_name, e.department_id FROM requirement_record r LEFT JOIN requirement_status_dict d ON r.requirement_status_id = d.id LEFT JOIN affected_type_dict a ON r.affected_id = a.id LEFT JOIN source_type_dict m ON r.source_type_id = m.id LEFT JOIN employee_dict e ON r.employee_id = e.id ORDER BY r.id DESC;";
    constexpr const char* SQL_SELECT_REQUIREMENT_BY_ID = "SELECT r.id, r.title, r.requirement_status_id, r.affected_id, a.affected as affected_name, r.source_type_id, s.type as source_type_name, r.create_time, r.update_time FROM requirement_record r LEFT JOIN affected_type_dict a ON r.affected_id = a.id LEFT JOIN source_type_dict s ON r.source_type_id = s.id WHERE r.id = ?;";
    constexpr const char* SQL_COUNT_WORK_RECORD_BY_REQUIREMENT = "SELECT COUNT(*) FROM work_record WHERE requirement_id = ?;";
    constexpr const char* SQL_SELECT_REQUIREMENT_RECORD_PAGED_BASE = "SELECT r.id, r.title, r.requirement_status_id, d.status as requirement_status_name, d.requirement_status_class as requirement_status_class, r.affected_id, a.affected as affected_name, r.source_type_id, m.type as source_type_name, r.create_time, r.update_time, r.employee_id, e.name as employee_name, e.department_id FROM requirement_record r LEFT JOIN requirement_status_dict d ON r.requirement_status_id = d.id LEFT JOIN affected_type_dict a ON r.affected_id = a.id LEFT JOIN source_type_dict m ON r.source_type_id = m.id LEFT JOIN employee_dict e ON r.employee_id = e.id ";
    constexpr const char* SQL_COUNT_REQUIREMENT_RECORD_BASE = "SELECT COUNT(*) FROM requirement_record r ";
    // ... 可继续补充 requirement 相关SQL

    // =====================
    // Affected Type Dict 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_ALL_AFFECTED_TYPE_DICT = "SELECT id, affected, comment FROM affected_type_dict ORDER BY id;";
    constexpr const char* SQL_INSERT_AFFECTED_TYPE_DICT = "INSERT INTO affected_type_dict (affected, comment) VALUES (?, ?);";
    constexpr const char* SQL_UPDATE_AFFECTED_TYPE_DICT = "UPDATE affected_type_dict SET affected = ?, comment = ? WHERE id = ?;";
    constexpr const char* SQL_DELETE_AFFECTED_TYPE_DICT = "DELETE FROM affected_type_dict WHERE id = ?;";

    // =====================
    // Source Type Dict 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_ALL_SOURCE_TYPE_DICT = "SELECT id, type, comment FROM source_type_dict ORDER BY id;";
    constexpr const char* SQL_INSERT_SOURCE_TYPE_DICT = "INSERT INTO source_type_dict (type, comment) VALUES (?, ?);";
    constexpr const char* SQL_UPDATE_SOURCE_TYPE_DICT = "UPDATE source_type_dict SET type = ?, comment = ? WHERE id = ?;";
    constexpr const char* SQL_DELETE_SOURCE_TYPE_DICT = "DELETE FROM source_type_dict WHERE id = ?;";

    // =====================
    // Work Type Dict 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_ALL_WORK_TYPE_DICT = "SELECT id, type, comment FROM work_type_dict ORDER BY id;";
    constexpr const char* SQL_INSERT_WORK_TYPE_DICT = "INSERT INTO work_type_dict (type, comment) VALUES (?, ?);";
    constexpr const char* SQL_UPDATE_WORK_TYPE_DICT = "UPDATE work_type_dict SET type = ?, comment = ? WHERE id = ?;";
    constexpr const char* SQL_DELETE_WORK_TYPE_DICT = "DELETE FROM work_type_dict WHERE id = ?;";

    // =====================
    // Work Record Status Dict 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_ALL_WORK_RECORD_STATUS_DICT = "SELECT id, status_name, status_class FROM work_record_status_dict ORDER BY id;";
    constexpr const char* SQL_INSERT_WORK_RECORD_STATUS_DICT = "INSERT INTO work_record_status_dict (status_name, status_class) VALUES (?, ?);";
    constexpr const char* SQL_UPDATE_WORK_RECORD_STATUS_DICT = "UPDATE work_record_status_dict SET status_name = ?, status_class = ? WHERE id = ?;";
    constexpr const char* SQL_DELETE_WORK_RECORD_STATUS_DICT = "DELETE FROM work_record_status_dict WHERE id = ?;";

    // =====================
    // Employee Dict 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_ALL_EMPLOYEE_DICT = "SELECT e.id, e.name, e.employee_number, d.name as department_name FROM employee_dict e LEFT JOIN department_dict d ON e.department_id = d.id ORDER BY e.id;";
    constexpr const char* SQL_SELECT_EMPLOYEE_DEPT_ID_BY_NAME = "SELECT id FROM department_dict WHERE name = ?;";
    constexpr const char* SQL_INSERT_EMPLOYEE_DICT = "INSERT INTO employee_dict (name, employee_number, department_id) VALUES (?, ?, ?);";
    constexpr const char* SQL_UPDATE_EMPLOYEE_DICT = "UPDATE employee_dict SET name = ?, employee_number = ?, department_id = ? WHERE id = ?;";
    constexpr const char* SQL_DELETE_EMPLOYEE_DICT = "DELETE FROM employee_dict WHERE id = ?;";
    constexpr const char* SQL_SELECT_EMPLOYEE_BY_DEPARTMENT = "SELECT e.id, e.name, e.employee_number, d.name as department_name FROM employee_dict e LEFT JOIN department_dict d ON e.department_id = d.id WHERE e.department_id = ? ORDER BY e.id;";

    // =====================
    // Department Dict 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_ALL_DEPARTMENT_DICT = "SELECT id, name, description, create_time FROM department_dict ORDER BY id;";
    constexpr const char* SQL_INSERT_DEPARTMENT_DICT = "INSERT INTO department_dict (name, description) VALUES (?, ?);";
    constexpr const char* SQL_UPDATE_DEPARTMENT_DICT = "UPDATE department_dict SET name = ?, description = ? WHERE id = ?;";
    constexpr const char* SQL_DELETE_DEPARTMENT_DICT = "DELETE FROM department_dict WHERE id = ?;";

    // =====================
    // Issue Progress Dict 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_ALL_ISSUE_PROGRESS_DICT = "SELECT id, progress, progress_class, comment FROM issue_progress_dict ORDER BY id;";
    constexpr const char* SQL_INSERT_ISSUE_PROGRESS_DICT = "INSERT INTO issue_progress_dict (progress, progress_class, comment) VALUES (?, ?, ?);";
    constexpr const char* SQL_UPDATE_ISSUE_PROGRESS_DICT = "UPDATE issue_progress_dict SET progress = ?, progress_class = ?, comment = ? WHERE id = ?;";
    constexpr const char* SQL_DELETE_ISSUE_PROGRESS_DICT = "DELETE FROM issue_progress_dict WHERE id = ?;";

    // =====================
    // Issue Record 相关SQL
    // =====================
    constexpr const char* SQL_INSERT_ISSUE_RECORD = "INSERT INTO issue_record (issue_title, employee_id, description, progress_id, responsible_person, affected_id, source_type_id, reported_by) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    constexpr const char* SQL_UPDATE_ISSUE_RECORD = "UPDATE issue_record SET issue_title=?, employee_id=?, description=?, progress_id=?, responsible_person=?, affected_id=?, source_type_id=?, reported_by=? WHERE id=?;";
    constexpr const char* SQL_DELETE_ISSUE_RECORD = "DELETE FROM issue_record WHERE id=?;";
    constexpr const char* SQL_SELECT_ALL_ISSUE_RECORD = "SELECT i.id, i.issue_title, i.reported_by, i.description, i.progress_id, d.progress as progress_name, d.progress_class as progress_class, i.responsible_person, i.affected_id, a.affected as affected_name, i.source_type_id, m.type as source_type_name, i.create_time, i.update_time, i.employee_id, e.name as employee_name, e.department_id FROM issue_record i LEFT JOIN issue_progress_dict d ON i.progress_id = d.id LEFT JOIN affected_type_dict a ON i.affected_id = a.id LEFT JOIN source_type_dict m ON i.source_type_id = m.id LEFT JOIN employee_dict e ON i.employee_id = e.id ORDER BY i.id DESC;";
    // 新增分页和 count 查询的 SQL 模板
    constexpr const char* SQL_SELECT_ISSUE_RECORD_PAGED_BASE = "SELECT i.id, i.issue_title, i.reported_by, i.description, i.progress_id, d.progress as progress_name, d.progress_class as progress_class, i.responsible_person, i.affected_id, a.affected as affected_name, i.source_type_id, m.type as source_type_name, i.create_time, i.update_time, i.employee_id, e.name as employee_name, e.department_id FROM issue_record i LEFT JOIN issue_progress_dict d ON i.progress_id = d.id LEFT JOIN affected_type_dict a ON i.affected_id = a.id LEFT JOIN source_type_dict m ON i.source_type_id = m.id LEFT JOIN employee_dict e ON i.employee_id = e.id ";
    constexpr const char* SQL_COUNT_ISSUE_RECORD_BASE = "SELECT COUNT(*) FROM issue_record i ";
    // ... 可继续补充其它SQL

    // =====================
    // Requirement Status Dict 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_ALL_REQUIREMENT_STATUS_DICT = "SELECT id, status, comment, requirement_status_class FROM requirement_status_dict ORDER BY id;";
    constexpr const char* SQL_INSERT_REQUIREMENT_STATUS_DICT = "INSERT INTO requirement_status_dict (status, comment, requirement_status_class) VALUES (?, ?, ?);";
    constexpr const char* SQL_UPDATE_REQUIREMENT_STATUS_DICT = "UPDATE requirement_status_dict SET status = ?, comment = ?, requirement_status_class = ? WHERE id = ?;";
    constexpr const char* SQL_DELETE_REQUIREMENT_STATUS_DICT = "DELETE FROM requirement_status_dict WHERE id = ?;";

    // =====================
    // File Record 相关SQL
    // =====================
    constexpr const char* SQL_SELECT_FILE_RECORD_BY_ID = "SELECT id, file_name, file_path, create_time FROM file_record WHERE id = ?;";
}

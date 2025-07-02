#pragma once
#include "../model/requirement_record_model.h"
#include "../db/sqlite3.h"
#include <vector>
#include <string>
#include <iostream>
#include <numeric>

// 查询所有需求记录
inline std::vector<RequirementRecord> queryAllRequirementRecordsDao(sqlite3* db) {
    std::vector<RequirementRecord> records;
    std::string sql = R"(
        SELECT r.id, r.title, r.requirement_status_id, d.status as requirement_status_name, d.requirement_status_class as requirement_status_class,
               r.affected_id, a.affected as affected_name, r.source_type_id, m.type as source_type_name, r.create_time, r.update_time, r.employee_id, e.name as employee_name, e.department_id
        FROM requirement_record r
        LEFT JOIN requirement_status_dict d ON r.requirement_status_id = d.id
        LEFT JOIN affected_type_dict a ON r.affected_id = a.id
        LEFT JOIN source_type_dict m ON r.source_type_id = m.id
        LEFT JOIN employee_dict e ON r.employee_id = e.id
        ORDER BY r.id DESC
    )";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    int rowCount = 0;
    while (true) {
        int step = sqlite3_step(stmt);
        if (step == SQLITE_ROW) {
            RequirementRecord r;
            r.id = sqlite3_column_int(stmt, 0);
            r.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            r.requirement_status_id = sqlite3_column_int(stmt, 2);
            r.requirement_status_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            r.requirement_status_class = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            r.affected_id = sqlite3_column_int(stmt, 5);
            r.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            r.source_type_id = sqlite3_column_int(stmt, 7);
            r.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
            r.create_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            r.update_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
            r.employee_id = sqlite3_column_type(stmt, 11) == SQLITE_NULL ? 0 : sqlite3_column_int(stmt, 11);
            r.employee_name = sqlite3_column_type(stmt, 12) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
            r.department_id = sqlite3_column_type(stmt, 13) == SQLITE_NULL ? 0 : sqlite3_column_int(stmt, 13);
            records.push_back(r);
            rowCount++;
        } else if (step == SQLITE_DONE) {
            break;
        } else {
            sqlite3_finalize(stmt);
            throw std::runtime_error(std::string("SQL step failed: ") + sqlite3_errmsg(db));
        }
    }
    sqlite3_finalize(stmt);
    return records;
}

// 根据ID获取RequirementRecord
inline bool getRequirementById(sqlite3* db, int id, RequirementRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    // 联表查出 source_type_name 和 affected_name
    const char* sql = "SELECT r.id, r.title, r.requirement_status_id, r.affected_id, a.affected as affected_name, r.source_type_id, s.type as source_type_name, r.create_time, r.update_time FROM requirement_record r LEFT JOIN affected_type_dict a ON r.affected_id = a.id LEFT JOIN source_type_dict s ON r.source_type_id = s.id WHERE r.id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        record.id = sqlite3_column_int(stmt, 0);
        record.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        record.requirement_status_id = sqlite3_column_int(stmt, 2);
        record.affected_id = sqlite3_column_int(stmt, 3);
        record.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        record.source_type_id = sqlite3_column_int(stmt, 5);
        record.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        record.create_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        record.update_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

// 需求相关DAO声明补全，确保接口可用
bool insertRequirementRecord(sqlite3* db, const RequirementRecord& record, int& newId);
inline void updateRequirementRecord(sqlite3* db, const RequirementRecord& record);
int countWorkRecordByRequirement(sqlite3* db, int requirementId);
bool deleteRequirementRecord(sqlite3* db, int id);
std::vector<RequirementRecord> queryRequirementRecordsPagedDao(sqlite3* db, int page, int page_size, int& total, const std::string& status_id, const std::string& affected_id, const std::string& source_type_id);
std::vector<std::string> getAllFilesByRequirement(sqlite3* db, int requirement_id);

// requirement_dao.h 内联实现，解决链接错误
inline bool insertRequirementRecord(sqlite3* db, const RequirementRecord& record, int& newId) {
    const char* sql = "INSERT INTO requirement_record (title, requirement_status_id, source_type_id, affected_id, employee_id) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_bind_text(stmt, 1, record.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, record.requirement_status_id);
    sqlite3_bind_int(stmt, 3, record.source_type_id);
    sqlite3_bind_int(stmt, 4, record.affected_id);
    sqlite3_bind_int(stmt, 5, record.employee_id);
    int rc = sqlite3_step(stmt);
    bool ok = (rc == SQLITE_DONE);
    if (ok) {
        newId = (int)sqlite3_last_insert_rowid(db);
    } else {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQL step failed: " + err);
    }
    sqlite3_finalize(stmt);
    return ok;
}

inline void updateRequirementRecord(sqlite3* db, const RequirementRecord& record) {
    const char* sql = "UPDATE requirement_record SET title=?, requirement_status_id=?, source_type_id=?, affected_id=?, employee_id=? WHERE id=?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_bind_text(stmt, 1, record.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, record.requirement_status_id);
    sqlite3_bind_int(stmt, 3, record.source_type_id);
    sqlite3_bind_int(stmt, 4, record.affected_id);
    sqlite3_bind_int(stmt, 5, record.employee_id);
    sqlite3_bind_int(stmt, 6, record.id);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQL step failed: " + err);
    }
    sqlite3_finalize(stmt);
}

// 统计某需求下工单数量
inline int countWorkRecordByRequirement(sqlite3* db, int requirementId) {
    const char* sql = "SELECT COUNT(*) FROM work_record WHERE requirement_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int count = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, requirementId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return count;
}
// 删除需求记录
inline bool deleteRequirementRecord(sqlite3* db, int id) {
    const char* sql = "DELETE FROM requirement_record WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    bool ok = (rc == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// 分页查询需求记录
inline std::vector<RequirementRecord> queryRequirementRecordsPagedDao(sqlite3* db, int page, int page_size, int& total, const std::string& status_id, const std::string& affected_id, const std::string& source_type_id) {
    std::vector<RequirementRecord> records;
    // 构建WHERE条件
    std::string whereClause = "";
    std::vector<std::string> conditions;
    if (!status_id.empty()) {
        conditions.push_back("r.requirement_status_id = " + status_id);
    }
    if (!affected_id.empty()) {
        conditions.push_back("r.affected_id = " + affected_id);
    }
    if (!source_type_id.empty()) {
        conditions.push_back("r.source_type_id = " + source_type_id);
    }
    if (!conditions.empty()) {
        whereClause = "WHERE " + std::accumulate(conditions.begin(), conditions.end(), std::string(),
            [](const std::string& a, const std::string& b) { return a + (a.empty() ? "" : " AND ") + b; });
    }
    // 先查询总数
    std::string countSql = "SELECT COUNT(*) FROM requirement_record r " + whereClause;
    sqlite3_stmt* countStmt = nullptr;
    if (sqlite3_prepare_v2(db, countSql.c_str(), -1, &countStmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(countStmt) == SQLITE_ROW) {
            total = sqlite3_column_int(countStmt, 0);
        }
    }
    sqlite3_finalize(countStmt);
    // 查询分页数据
    std::string sql = R"(
        SELECT r.id, r.title, r.requirement_status_id, d.status as requirement_status_name, d.requirement_status_class as requirement_status_class,
               r.affected_id, a.affected as affected_name, r.source_type_id, m.type as source_type_name, r.create_time, r.update_time,
               r.employee_id, e.name as employee_name, e.department_id
        FROM requirement_record r
        LEFT JOIN requirement_status_dict d ON r.requirement_status_id = d.id
        LEFT JOIN affected_type_dict a ON r.affected_id = a.id
        LEFT JOIN source_type_dict m ON r.source_type_id = m.id
        LEFT JOIN employee_dict e ON r.employee_id = e.id
    )" + whereClause + " ORDER BY r.id DESC LIMIT ? OFFSET ?";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    int offset = (page - 1) * page_size;
    sqlite3_bind_int(stmt, 1, page_size);
    sqlite3_bind_int(stmt, 2, offset);
    while (true) {
        int step = sqlite3_step(stmt);
        if (step == SQLITE_ROW) {
            RequirementRecord r;
            r.id = sqlite3_column_int(stmt, 0);
            r.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            r.requirement_status_id = sqlite3_column_int(stmt, 2);
            r.requirement_status_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            r.requirement_status_class = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            r.affected_id = sqlite3_column_int(stmt, 5);
            r.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            r.source_type_id = sqlite3_column_int(stmt, 7);
            r.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
            r.create_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            r.update_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
            r.employee_id = sqlite3_column_type(stmt, 11) == SQLITE_NULL ? 0 : sqlite3_column_int(stmt, 11);
            r.employee_name = sqlite3_column_type(stmt, 12) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
            r.department_id = sqlite3_column_type(stmt, 13) == SQLITE_NULL ? 0 : sqlite3_column_int(stmt, 13);
            records.push_back(r);
        } else if (step == SQLITE_DONE) {
            break;
        } else {
            sqlite3_finalize(stmt);
            throw std::runtime_error(std::string("SQL step failed: ") + sqlite3_errmsg(db));
        }
    }
    sqlite3_finalize(stmt);
    return records;
}

// 获取需求相关的所有文件路径
inline std::vector<std::string> getAllFilesByRequirement(sqlite3* db, int requirement_id) {
    std::vector<std::string> filePaths;
    const char* sql = R"(
        SELECT f.file_path 
        FROM workrecord_file f
        JOIN work_record w ON f.work_record_id = w.id
        WHERE w.requirement_id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, requirement_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (path) {
                filePaths.push_back(std::string(path));
            }
        }
    }
    sqlite3_finalize(stmt);
    return filePaths;
}

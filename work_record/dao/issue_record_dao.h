#pragma once
#include "../model/issue_record_model.h"
#include "../db/sqlite3.h"
#include <vector>
#include <string>
#include <numeric>
#include "../constants/constants_sql.h"

// 查询所有问题记录
inline std::vector<IssueRecord> queryAllIssuesDao(sqlite3* db) {
    std::vector<IssueRecord> records;
    std::string sql = R"(
        SELECT i.id, i.issue_title, i.reported_by, i.description, i.progress_id, d.progress as progress_name, d.progress_class as progress_class,
               i.responsible_person, i.affected_id, a.affected as affected_name, i.source_type_id, m.type as source_type_name, i.create_time, i.update_time,
               i.employee_id, e.name as employee_name, e.department_id
        FROM issue_record i
        LEFT JOIN issue_progress_dict d ON i.progress_id = d.id
        LEFT JOIN affected_type_dict a ON i.affected_id = a.id
        LEFT JOIN source_type_dict m ON i.source_type_id = m.id
        LEFT JOIN employee_dict e ON i.employee_id = e.id
        ORDER BY i.id DESC
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
            IssueRecord r;
            r.id = sqlite3_column_int(stmt, 0);
            r.issue_title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            r.reported_by = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            r.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            r.progress_id = sqlite3_column_int(stmt, 4);
            r.progress_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            r.progress_class = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            r.responsible_person = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            r.affected_id = sqlite3_column_int(stmt, 8);
            r.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            r.source_type_id = sqlite3_column_int(stmt, 10);
            r.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
            r.create_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
            r.update_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
            r.employee_id = sqlite3_column_type(stmt, 14) != SQLITE_NULL ? sqlite3_column_int(stmt, 14) : 0;
            r.employee_name = sqlite3_column_type(stmt, 15) != SQLITE_NULL ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15)) : "";
            r.department_id = sqlite3_column_type(stmt, 16) != SQLITE_NULL ? sqlite3_column_int(stmt, 16) : 0;
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

// 新增问题记录
inline void insertIssueDao(sqlite3* db, const IssueRecord& r, int& newId) {
    const char* sql = constants_sql::SQL_INSERT_ISSUE_RECORD;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    sqlite3_bind_text(stmt, 1, r.issue_title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, r.employee_id);
    sqlite3_bind_text(stmt, 3, r.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, r.progress_id);
    sqlite3_bind_text(stmt, 5, r.responsible_person.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, r.affected_id);
    sqlite3_bind_int(stmt, 7, r.source_type_id);
    sqlite3_bind_text(stmt, 8, r.reported_by.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQL step failed: " + err);
    }
    newId = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
}

// 修改问题记录
inline void updateIssueDao(sqlite3* db, const IssueRecord& r) {
    const char* sql = constants_sql::SQL_UPDATE_ISSUE_RECORD;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_bind_text(stmt, 1, r.issue_title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, r.employee_id);
    sqlite3_bind_text(stmt, 3, r.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, r.progress_id);
    sqlite3_bind_text(stmt, 5, r.responsible_person.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, r.affected_id);
    sqlite3_bind_int(stmt, 7, r.source_type_id);
    sqlite3_bind_text(stmt, 8, r.reported_by.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, r.id);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQL step failed: " + err);
    }
    sqlite3_finalize(stmt);
}

// 删除问题记录
inline bool deleteIssueDao(sqlite3* db, int id) {
    const char* sql = constants_sql::SQL_DELETE_ISSUE_RECORD;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// 分页查询问题记录
inline std::vector<IssueRecord> queryIssuesPagedDao(sqlite3* db, int page, int page_size, int& total, const std::string& source_type_id, const std::string& affected_id) {
    std::vector<IssueRecord> records;
    
    // 构建WHERE条件
    std::string whereClause = "";
    std::vector<std::string> conditions;
    if (!source_type_id.empty()) {
        conditions.push_back("i.source_type_id = " + source_type_id);
    }
    if (!affected_id.empty()) {
        conditions.push_back("i.affected_id = " + affected_id);
    }
    if (!conditions.empty()) {
        whereClause = "WHERE " + std::accumulate(conditions.begin(), conditions.end(), std::string(),
            [](const std::string& a, const std::string& b) { return a + (a.empty() ? "" : " AND ") + b; });
    }
    
    // 先查询总数
    std::string countSql = "SELECT COUNT(*) FROM issue_record i " + whereClause;
    sqlite3_stmt* countStmt = nullptr;
    if (sqlite3_prepare_v2(db, countSql.c_str(), -1, &countStmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(countStmt) == SQLITE_ROW) {
            total = sqlite3_column_int(countStmt, 0);
        }
    }
    sqlite3_finalize(countStmt);
    
    // 查询分页数据
    std::string sql = R"(
        SELECT i.id, i.issue_title, i.reported_by, i.description, i.progress_id, d.progress as progress_name, d.progress_class as progress_class,
               i.responsible_person, i.affected_id, a.affected as affected_name, i.source_type_id, m.type as source_type_name, i.create_time, i.update_time,
               i.employee_id, e.name as employee_name, e.department_id
        FROM issue_record i
        LEFT JOIN issue_progress_dict d ON i.progress_id = d.id
        LEFT JOIN affected_type_dict a ON i.affected_id = a.id
        LEFT JOIN source_type_dict m ON i.source_type_id = m.id
        LEFT JOIN employee_dict e ON i.employee_id = e.id
    )" + whereClause + " ORDER BY i.id DESC LIMIT ? OFFSET ?";
    
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
            IssueRecord r;
            r.id = sqlite3_column_int(stmt, 0);
            r.issue_title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            r.reported_by = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            r.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            r.progress_id = sqlite3_column_int(stmt, 4);
            r.progress_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            r.progress_class = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            r.responsible_person = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            r.affected_id = sqlite3_column_int(stmt, 8);
            r.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            r.source_type_id = sqlite3_column_int(stmt, 10);
            r.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
            r.create_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
            r.update_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
            r.employee_id = sqlite3_column_type(stmt, 14) != SQLITE_NULL ? sqlite3_column_int(stmt, 14) : 0;
            r.employee_name = sqlite3_column_type(stmt, 15) != SQLITE_NULL ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15)) : "";
            r.department_id = sqlite3_column_type(stmt, 16) != SQLITE_NULL ? sqlite3_column_int(stmt, 16) : 0;
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

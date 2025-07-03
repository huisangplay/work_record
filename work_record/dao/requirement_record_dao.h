#pragma once
#include "../model/requirement_record_model.h"
#include "../db/sqlite3.h"
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include "../constants/constants_sql.h"
#include "../db/db_util.h"
using namespace db_util;    
// 查询所有需求记录
inline std::vector<RequirementRecord> queryAllRequirementRecordsDao(sqlite3* db) {
    std::vector<RequirementRecord> records;
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_REQUIREMENT_RECORD, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        RequirementRecord r;
        r.id = sqlite3_column_int(s, 0);
        r.title = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        r.requirement_status_id = sqlite3_column_int(s, 2);
        r.requirement_status_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        r.requirement_status_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 4));
        r.affected_id = sqlite3_column_int(s, 5);
        r.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
        r.source_type_id = sqlite3_column_int(s, 7);
        r.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 8));
        r.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 9));
        r.update_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 10));
        r.employee_id = sqlite3_column_type(s, 11) == SQLITE_NULL ? 0 : sqlite3_column_int(s, 11);
        r.employee_name = sqlite3_column_type(s, 12) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(s, 12));
        r.department_id = sqlite3_column_type(s, 13) == SQLITE_NULL ? 0 : sqlite3_column_int(s, 13);
        records.push_back(r);
    });
    return records;
}

// 根据ID获取RequirementRecord
inline bool getRequirementById(sqlite3* db, int id, RequirementRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_REQUIREMENT_BY_ID, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    bool found = false;
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        record.id = sqlite3_column_int(s, 0);
        record.title = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        record.requirement_status_id = sqlite3_column_int(s, 2);
        record.affected_id = sqlite3_column_int(s, 3);
        record.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 4));
        record.source_type_id = sqlite3_column_int(s, 5);
        record.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
        record.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 7));
        record.update_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 8));
        found = true;
    });
    return found;
}

// requirement_dao.h 内联实现，解决链接错误
inline bool insertRequirementRecord(sqlite3* db, const RequirementRecord& record, int& newId) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_REQUIREMENT_RECORD, &stmt);
    sqlite3_bind_text(stmt, 1, record.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, record.requirement_status_id);
    sqlite3_bind_int(stmt, 3, record.source_type_id);
    sqlite3_bind_int(stmt, 4, record.affected_id);
    sqlite3_bind_int(stmt, 5, record.employee_id);
    db_util::exec_stmt_done(db, stmt);
    newId = (int)sqlite3_last_insert_rowid(db);
    return true;
}

inline void updateRequirementRecord(sqlite3* db, const RequirementRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_REQUIREMENT_RECORD, &stmt);
    sqlite3_bind_text(stmt, 1, record.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, record.requirement_status_id);
    sqlite3_bind_int(stmt, 3, record.source_type_id);
    sqlite3_bind_int(stmt, 4, record.affected_id);
    sqlite3_bind_int(stmt, 5, record.employee_id);
    sqlite3_bind_int(stmt, 6, record.id);
    db_util::exec_stmt_done(db, stmt);
}

// 统计某需求下工单数量
inline int countWorkRecordByRequirement(sqlite3* db, int requirementId) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_COUNT_WORK_RECORD_BY_REQUIREMENT, &stmt);
    int count = 0;
    sqlite3_bind_int(stmt, 1, requirementId);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        count = sqlite3_column_int(s, 0);
    });
    return count;
}

// 删除需求记录
inline bool deleteRequirementRecord(sqlite3* db, int id) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_REQUIREMENT_RECORD, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}

// 分页查询需求记录
inline std::vector<RequirementRecord> queryRequirementRecordsPagedDao(sqlite3* db, int page, int page_size, int& total, const std::string& status_id, const std::string& affected_id, const std::string& source_type_id) {
    std::vector<RequirementRecord> records;
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
    // 查询总数
    std::string countSql = std::string(constants_sql::SQL_COUNT_REQUIREMENT_RECORD_BASE) + whereClause + ";";
    sqlite3_stmt* countStmt = nullptr;
    db_util::prepare_throw(db, countSql.c_str(), &countStmt);
    db_util::exec_select(db, countStmt, [&](sqlite3_stmt* s){
        total = sqlite3_column_int(s, 0);
    });
    // 查询分页数据
    std::string sql = std::string(constants_sql::SQL_SELECT_REQUIREMENT_RECORD_PAGED_BASE) + whereClause + " ORDER BY r.id DESC LIMIT ? OFFSET ?";
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, sql.c_str(), &stmt);
    int offset = (page - 1) * page_size;
    sqlite3_bind_int(stmt, 1, page_size);
    sqlite3_bind_int(stmt, 2, offset);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        RequirementRecord r;
        r.id = sqlite3_column_int(s, 0);
        r.title = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        r.requirement_status_id = sqlite3_column_int(s, 2);
        r.requirement_status_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        r.requirement_status_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 4));
        r.affected_id = sqlite3_column_int(s, 5);
        r.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
        r.source_type_id = sqlite3_column_int(s, 7);
        r.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 8));
        r.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 9));
        r.update_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 10));
        r.employee_id = sqlite3_column_type(s, 11) == SQLITE_NULL ? 0 : sqlite3_column_int(s, 11);
        r.employee_name = sqlite3_column_type(s, 12) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(s, 12));
        r.department_id = sqlite3_column_type(s, 13) == SQLITE_NULL ? 0 : sqlite3_column_int(s, 13);
        records.push_back(r);
    });
    return records;
}

// 获取需求相关的所有文件路径
inline std::vector<std::string> getAllFilesByRequirement(sqlite3* db, int requirement_id) {
    std::vector<std::string> filePaths;
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_FILES_BY_REQUIREMENT_ID, &stmt);
    sqlite3_bind_int(stmt, 1, requirement_id);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        const char* path = reinterpret_cast<const char*>(sqlite3_column_text(s, 0));
        if (path) filePaths.push_back(std::string(path));
    });
    return filePaths;
}

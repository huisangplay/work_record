#include "dao/requirement_record_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"
#include <string>
#include <numeric>

using namespace dao_util;

// 查询所有需求记录
DaoResult queryAllRequirementRecords(sqlite3* db, std::vector<RequirementRecord>& records) {
    records.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_REQUIREMENT_RECORD, stmt, "queryAllRequirementRecords");
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
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
    }, "queryAllRequirementRecords");
    
    return DaoResult::SUCCESS;
}

// 根据ID获取RequirementRecord
DaoResult getRequirementById(sqlite3* db, int64_t id, RequirementRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_REQUIREMENT_BY_ID, stmt, "getRequirementById");
    
    sqlite3_bind_int64(stmt, 1, id);
    
    bool found = false;
    auto result = exec_select_safe(db, stmt, [&](sqlite3_stmt* s) {
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
    }, "getRequirementById");
    
    if (result != DaoResult::SUCCESS) {
        return DaoResult::FAILED;
    }
    
    return found ? DaoResult::SUCCESS : DaoResult::NOT_FOUND;
}

// 插入需求记录
DaoResult insertRequirementRecord(sqlite3* db, const RequirementRecord& record, int64_t& newId) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_REQUIREMENT_RECORD, stmt, "insertRequirementRecord");
    
    sqlite3_bind_text(stmt, 1, record.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, record.requirement_status_id);
    sqlite3_bind_int64(stmt, 3, record.source_type_id);
    sqlite3_bind_int64(stmt, 4, record.affected_id);
    sqlite3_bind_int64(stmt, 5, record.employee_id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertRequirementRecord");
    
    newId = static_cast<int>(sqlite3_last_insert_rowid(db));
    return DaoResult::SUCCESS;
}

// 更新需求记录
DaoResult updateRequirementRecord(sqlite3* db, const RequirementRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_REQUIREMENT_RECORD, stmt, "updateRequirementRecord");
    
    sqlite3_bind_text(stmt, 1, record.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, record.requirement_status_id);
    sqlite3_bind_int64(stmt, 3, record.source_type_id);
    sqlite3_bind_int64(stmt, 4, record.affected_id);
    sqlite3_bind_int64(stmt, 5, record.employee_id);
    sqlite3_bind_int64(stmt, 6, record.id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "updateRequirementRecord");
    
    return DaoResult::SUCCESS;
}

// 统计某需求下工作记录数量
DaoResult countWorkRecordByRequirement(sqlite3* db, int64_t requirementId, int& count) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_COUNT_WORK_RECORD_BY_REQUIREMENT, stmt, "countWorkRecordByRequirement");
    
    sqlite3_bind_int64(stmt, 1, requirementId);
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        count = sqlite3_column_int(s, 0);
    }, "countWorkRecordByRequirement");
    
    return DaoResult::SUCCESS;
}

// 删除需求记录
DaoResult deleteRequirementRecord(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_REQUIREMENT_RECORD, stmt, "deleteRequirementRecord");
    
    sqlite3_bind_int(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteRequirementRecord");
    
    return DaoResult::SUCCESS;
}

// 分页查询需求记录
DaoResult queryRequirementRecordsPaged(sqlite3* db, int page, int page_size, int& total,
                                          const std::string& status_id, const std::string& affected_id, 
                                          const std::string& source_type_id, std::vector<RequirementRecord>& records) {
    records.clear();
    
    std::string whereClause;
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
    
    DAO_SAFE_PREPARE(db, countSql.c_str(), countStmt, "queryRequirementRecordsPaged_count");
    
    DAO_SAFE_EXEC_SELECT(db, countStmt, [&](sqlite3_stmt* s) {
        total = sqlite3_column_int(s, 0);
    }, "queryRequirementRecordsPaged_count");
    
    // 查询分页数据
    std::string sql = std::string(constants_sql::SQL_SELECT_REQUIREMENT_RECORD_PAGED_BASE) + whereClause + " ORDER BY r.id DESC LIMIT ? OFFSET ?";
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, sql.c_str(), stmt, "queryRequirementRecordsPaged");
    
    int offset = (page - 1) * page_size;
    sqlite3_bind_int(stmt, 1, page_size);
    sqlite3_bind_int(stmt, 2, offset);
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
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
    }, "queryRequirementRecordsPaged");
    
    return DaoResult::SUCCESS;
}
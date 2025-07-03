#pragma once
#include "../model/work_record_model.h"
#include "../constants/constants_sql.h"
#include "../db/sqlite3.h"
#include "../db/db_util.h"
using namespace db_util;

inline bool insertWorkRecord(sqlite3* db, const WorkRecord& record, int& newId) {
    sqlite3_stmt* stmt = nullptr;
    prepare_throw(db, constants_sql::SQL_INSERT_WORK_RECORD, &stmt);
    sqlite3_bind_int(stmt, 1, record.requirement_id);
    sqlite3_bind_int(stmt, 2, record.work_type_id);
    sqlite3_bind_int(stmt, 3, record.affected_id);
    sqlite3_bind_int(stmt, 4, record.source_type_id);
    sqlite3_bind_int(stmt, 5, record.work_record_status_id);
    sqlite3_bind_text(stmt, 6, record.work_content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, record.employee_id);
    exec_stmt_done(db, stmt);
    newId = (int)sqlite3_last_insert_rowid(db);
    return true;
}

// 根据ID获取WorkRecord
inline bool getWorkRecordById(sqlite3* db, int id, WorkRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    prepare_throw(db, constants_sql::SQL_SELECT_WORK_RECORD_BY_ID, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    bool found = false;
    exec_select(db, stmt, [&](sqlite3_stmt* s){
        record.id = sqlite3_column_int(s, 0);
        record.requirement_id = sqlite3_column_int(s, 1);
        record.work_type_id = sqlite3_column_int(s, 2);
        record.affected_id = sqlite3_column_int(s, 3);
        record.source_type_id = sqlite3_column_int(s, 4);
        record.work_record_status_id = sqlite3_column_int(s, 5);
        record.work_content = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
        record.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 7));
        record.completion_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 8));
        record.employee_id = sqlite3_column_int(s, 9);
        found = true;
    });
    return found;
}

// 更新工单
inline bool updateWorkRecord(sqlite3* db, const WorkRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    prepare_throw(db, constants_sql::SQL_UPDATE_WORK_RECORD, &stmt);
    sqlite3_bind_int(stmt, 1, record.requirement_id);
    sqlite3_bind_int(stmt, 2, record.work_type_id);
    sqlite3_bind_int(stmt, 3, record.affected_id);
    sqlite3_bind_int(stmt, 4, record.source_type_id);
    sqlite3_bind_int(stmt, 5, record.work_record_status_id);
    sqlite3_bind_text(stmt, 6, record.work_content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, record.employee_id);
    sqlite3_bind_text(stmt, 8, record.completion_time.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, record.id);
    return exec_stmt_done(db, stmt);
}

// 分页+过滤查询工单
inline std::vector<WorkRecord> queryWorkRecordsPagedDao(sqlite3* db, const std::string& scope, int page, int page_size, int& total, const std::string& status_id, const std::string& affected_id, const std::string& source_type_id, const std::string& requirement_id, const std::string& work_type_id) {
    std::vector<WorkRecord> records;
    std::string sql = constants_sql::SQL_SELECT_WORK_RECORD_PAGED_BASE;
    std::string where;
    if (scope == "month") {
        where += (where.empty() ? " WHERE " : " AND ") + std::string("strftime('%Y-%m', w.completion_time) = strftime('%Y-%m', 'now', 'localtime')");
    } else if (scope == "year") {
        where += (where.empty() ? " WHERE " : " AND ") + std::string("strftime('%Y', w.completion_time) = strftime('%Y', 'now', 'localtime')");
    }
    if (!status_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.work_record_status_id = ?");
    if (!affected_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.affected_id = ?");
    if (!source_type_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.source_type_id = ?");
    if (!requirement_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.requirement_id = ?");
    if (!work_type_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.work_type_id = ?");
    sql += where;
    sql += " ORDER BY w.completion_time DESC LIMIT ? OFFSET ?;";
    sqlite3_stmt* stmt = nullptr;
    prepare_throw(db, sql.c_str(), &stmt);
    int idx = 1;
    if (!status_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(status_id));
    if (!affected_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(affected_id));
    if (!source_type_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(source_type_id));
    if (!requirement_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(requirement_id));
    if (!work_type_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(work_type_id));
    sqlite3_bind_int(stmt, idx++, page_size);
    sqlite3_bind_int(stmt, idx++, (page-1)*page_size);
    exec_select(db, stmt, [&](sqlite3_stmt* s){
        WorkRecord r;
        r.id = sqlite3_column_int(s, 0);
        r.requirement_id = sqlite3_column_int(s, 1);
        const char* tmp2 = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        r.requirement_title = tmp2 ? tmp2 : "";
        r.work_type_id = sqlite3_column_int(s, 3);
        const char* tmp4 = reinterpret_cast<const char*>(sqlite3_column_text(s, 4));
        r.work_type = tmp4 ? tmp4 : "";
        r.affected_id = sqlite3_column_int(s, 5);
        const char* tmp6 = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
        r.affected_name = tmp6 ? tmp6 : "";
        r.source_type_id = sqlite3_column_int(s, 7);
        const char* tmp8 = reinterpret_cast<const char*>(sqlite3_column_text(s, 8));
        r.source_type_name = tmp8 ? tmp8 : "";
        r.work_record_status_id = sqlite3_column_int(s, 9);
        const char* tmp10 = reinterpret_cast<const char*>(sqlite3_column_text(s, 10));
        r.status_name = tmp10 ? tmp10 : "";
        const char* tmp11 = reinterpret_cast<const char*>(sqlite3_column_text(s, 11));
        r.status_class = tmp11 ? tmp11 : "";
        const char* tmp12 = reinterpret_cast<const char*>(sqlite3_column_text(s, 12));
        r.work_content = tmp12 ? tmp12 : "";
        const char* tmp13 = reinterpret_cast<const char*>(sqlite3_column_text(s, 13));
        r.create_time = tmp13 ? tmp13 : "";
        const char* tmp14 = reinterpret_cast<const char*>(sqlite3_column_text(s, 14));
        r.completion_time = tmp14 ? tmp14 : "";
        r.employee_id = sqlite3_column_int(s, 15);
        const char* tmp16 = reinterpret_cast<const char*>(sqlite3_column_text(s, 16));
        r.employee_name = tmp16 ? tmp16 : "";
        r.department_id = sqlite3_column_type(s, 17) == SQLITE_NULL ? 0 : sqlite3_column_int(s, 17);
        records.push_back(r);
    });
    // 查询总数
    std::string countSql = std::string(constants_sql::SQL_COUNT_WORK_RECORD_BASE) + where + ";";
    sqlite3_stmt* countStmt = nullptr;
    prepare_throw(db, countSql.c_str(), &countStmt);
    int cidx = 1;
    if (!status_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(status_id));
    if (!affected_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(affected_id));
    if (!source_type_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(source_type_id));
    if (!requirement_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(requirement_id));
    if (!work_type_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(work_type_id));
    exec_select(db, countStmt, [&](sqlite3_stmt* s){
        total = sqlite3_column_int(s, 0);
    });
    // 批量查询文件信息
    if (!records.empty()) {
        std::string inClause = "(";
        for (size_t i = 0; i < records.size(); ++i) {
            inClause += std::to_string(records[i].id);
            if (i != records.size() - 1) inClause += ",";
        }
        inClause += ")";
        std::string fileSql = std::string(constants_sql::SQL_SELECT_WORK_FILES_BY_IDS_BASE) + inClause + ";";
        sqlite3_stmt* fileStmt = nullptr;
        prepare_throw(db, fileSql.c_str(), &fileStmt);
        exec_select(db, fileStmt, [&](sqlite3_stmt* s){
            int workId = sqlite3_column_int(s, 0);
            int fileId = sqlite3_column_int(s, 1);
            nlohmann::json fileJson = {
                {"id", fileId},
                {"name", reinterpret_cast<const char*>(sqlite3_column_text(s, 2))},
                {"path", reinterpret_cast<const char*>(sqlite3_column_text(s, 3))},
                {"upload_time", reinterpret_cast<const char*>(sqlite3_column_text(s, 4))}
            };
            auto it = std::find_if(records.begin(), records.end(), [workId](const WorkRecord& r){ return r.id == workId; });
            if (it != records.end()) {
                it->file_info[fileId] = fileJson.dump();
            }
        });
    }
    return records;
}

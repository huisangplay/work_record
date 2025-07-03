#pragma once
#include "../model/work_record_model.h"
#include "../constants/constants_sql.h"
#include "../db/sqlite3.h"
#include "../db/db_util.h"

inline bool insertWorkRecord(sqlite3* db, const WorkRecord& record, int& newId) {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, constants_sql::SQL_INSERT_WORK_RECORD, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_bind_int(stmt, 1, record.requirement_id);
    sqlite3_bind_int(stmt, 2, record.work_type_id);
    sqlite3_bind_int(stmt, 3, record.affected_id);
    sqlite3_bind_int(stmt, 4, record.source_type_id);
    sqlite3_bind_int(stmt, 5, record.work_record_status_id);
    sqlite3_bind_text(stmt, 6, record.work_content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, record.employee_id);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQL step failed: " + err);
    }
    newId = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return true;
}

// 根据ID获取WorkRecord
inline bool getWorkRecordById(sqlite3* db, int id, WorkRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, requirement_id, work_type_id, affected_id, source_type_id, work_record_status_id, work_content, create_time, completion_time, employee_id FROM work_record WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_bind_int(stmt, 1, id);
    bool found = false;
    int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW) {
        record.id = sqlite3_column_int(stmt, 0);
        record.requirement_id = sqlite3_column_int(stmt, 1);
        record.work_type_id = sqlite3_column_int(stmt, 2);
        record.affected_id = sqlite3_column_int(stmt, 3);
        record.source_type_id = sqlite3_column_int(stmt, 4);
        record.work_record_status_id = sqlite3_column_int(stmt, 5);
        record.work_content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        record.create_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        record.completion_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        record.employee_id = sqlite3_column_int(stmt, 9);
        found = true;
    } else if (step != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQL step failed: " + err);
    }
    sqlite3_finalize(stmt);
    return found;
}

// 更新工单
inline bool updateWorkRecord(sqlite3* db, const WorkRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE work_record SET requirement_id=?, work_type_id=?, affected_id=?, source_type_id=?, work_record_status_id=?, work_content=?, employee_id=?, completion_time=? WHERE id=?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_bind_int(stmt, 1, record.requirement_id);
    sqlite3_bind_int(stmt, 2, record.work_type_id);
    sqlite3_bind_int(stmt, 3, record.affected_id);
    sqlite3_bind_int(stmt, 4, record.source_type_id);
    sqlite3_bind_int(stmt, 5, record.work_record_status_id);
    sqlite3_bind_text(stmt, 6, record.work_content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, record.employee_id);
    sqlite3_bind_text(stmt, 8, record.completion_time.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, record.id);
    int step = sqlite3_step(stmt);
    bool ok = (step == SQLITE_DONE);
    if (!ok) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("SQL step failed: " + err);
    }
    sqlite3_finalize(stmt);
    return ok;
}

// 分页+过滤查询工单
inline std::vector<WorkRecord> queryWorkRecordsPagedDao(sqlite3* db, const std::string& scope, int page, int page_size, int& total, const std::string& status_id, const std::string& affected_id = "", const std::string& source_type_id = "", const std::string& requirement_id = "", const std::string& work_type_id = "") {
    std::vector<WorkRecord> records;
    std::string sql = R"(
        SELECT w.id, w.requirement_id, r.title as requirement_title, w.work_type_id, t.type as work_type,
               w.affected_id, a.affected as affected_name, w.source_type_id, m.type as source_type_name,
               w.work_record_status_id, d.status_name, d.status_class as status_class,
               w.work_content, w.create_time, w.completion_time, w.employee_id, e.name as employee_name, e.department_id
        FROM work_record w
        LEFT JOIN requirement_record r ON w.requirement_id = r.id
        LEFT JOIN work_type_dict t ON w.work_type_id = t.id
        LEFT JOIN affected_type_dict a ON w.affected_id = a.id
        LEFT JOIN source_type_dict m ON w.source_type_id = m.id
        LEFT JOIN work_record_status_dict d ON w.work_record_status_id = d.id
        LEFT JOIN employee_dict e ON w.employee_id = e.id
    )";
    std::string where;
    if (!status_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.work_record_status_id = ?");
    if (!affected_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.affected_id = ?");
    if (!source_type_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.source_type_id = ?");
    if (!requirement_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.requirement_id = ?");
    if (!work_type_id.empty()) where += (where.empty() ? " WHERE " : " AND ") + std::string("w.work_type_id = ?");
    sql += where;
    sql += " ORDER BY w.completion_time DESC LIMIT ? OFFSET ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    int idx = 1;
    if (!status_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(status_id));
    if (!affected_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(affected_id));
    if (!source_type_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(source_type_id));
    if (!requirement_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(requirement_id));
    if (!work_type_id.empty()) sqlite3_bind_int(stmt, idx++, std::stoi(work_type_id));
    sqlite3_bind_int(stmt, idx++, page_size);
    sqlite3_bind_int(stmt, idx++, (page-1)*page_size);
    while (true) {
        int step = sqlite3_step(stmt);
        if (step == SQLITE_ROW) {
            WorkRecord r;
            r.id = sqlite3_column_int(stmt, 0);
            r.requirement_id = sqlite3_column_int(stmt, 1);
            const char* tmp2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            r.requirement_title = tmp2 ? tmp2 : "";
            r.work_type_id = sqlite3_column_int(stmt, 3);
            const char* tmp4 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            r.work_type = tmp4 ? tmp4 : "";
            r.affected_id = sqlite3_column_int(stmt, 5);
            const char* tmp6 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            r.affected_name = tmp6 ? tmp6 : "";
            r.source_type_id = sqlite3_column_int(stmt, 7);
            const char* tmp8 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
            r.source_type_name = tmp8 ? tmp8 : "";
            r.work_record_status_id = sqlite3_column_int(stmt, 9);
            const char* tmp10 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
            r.status_name = tmp10 ? tmp10 : "";
            const char* tmp11 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
            r.status_class = tmp11 ? tmp11 : "";
            const char* tmp12 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
            r.work_content = tmp12 ? tmp12 : "";
            const char* tmp13 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
            r.create_time = tmp13 ? tmp13 : "";
            const char* tmp14 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14));
            r.completion_time = tmp14 ? tmp14 : "";
            r.employee_id = sqlite3_column_int(stmt, 15);
            const char* tmp16 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 16));
            r.employee_name = tmp16 ? tmp16 : "";
            r.department_id = sqlite3_column_type(stmt, 17) == SQLITE_NULL ? 0 : sqlite3_column_int(stmt, 17);
            records.push_back(r);
        } else if (step == SQLITE_DONE) {
            break;
        } else {
            sqlite3_finalize(stmt);
            throw std::runtime_error(std::string("SQL step failed: ") + sqlite3_errmsg(db));
        }
    }
    sqlite3_finalize(stmt);
    // 查询总数
    std::string countSql = "SELECT COUNT(*) FROM work_record" + where + ";";
    sqlite3_stmt* countStmt = nullptr;
    rc = sqlite3_prepare_v2(db, countSql.c_str(), -1, &countStmt, nullptr);
    if (rc == SQLITE_OK) {
        int cidx = 1;
        if (!status_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(status_id));
        if (!affected_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(affected_id));
        if (!source_type_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(source_type_id));
        if (!requirement_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(requirement_id));
        if (!work_type_id.empty()) sqlite3_bind_int(countStmt, cidx++, std::stoi(work_type_id));
        if (sqlite3_step(countStmt) == SQLITE_ROW) {
            total = sqlite3_column_int(countStmt, 0);
        }
        sqlite3_finalize(countStmt);
    } else {
        total = 0;
    }
    // 批量查询文件信息
    if (!records.empty()) {
        std::string inClause = "(";
        for (size_t i = 0; i < records.size(); ++i) {
            inClause += std::to_string(records[i].id);
            if (i != records.size() - 1) inClause += ",";
        }
        inClause += ")";
        std::string fileSql = "SELECT wf.work_record_id, f.id, f.file_name, f.file_path, f.create_time FROM work_record_files wf JOIN file_record f ON wf.file_record_id = f.id WHERE wf.work_record_id IN " + inClause + ";";
        sqlite3_stmt* fileStmt = nullptr;
        if (sqlite3_prepare_v2(db, fileSql.c_str(), -1, &fileStmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(fileStmt) == SQLITE_ROW) {
                int workId = sqlite3_column_int(fileStmt, 0);
                int fileId = sqlite3_column_int(fileStmt, 1);
                nlohmann::json fileJson = {
                    {"id", fileId},
                    {"name", reinterpret_cast<const char*>(sqlite3_column_text(fileStmt, 2))},
                    {"path", reinterpret_cast<const char*>(sqlite3_column_text(fileStmt, 3))},
                    {"upload_time", reinterpret_cast<const char*>(sqlite3_column_text(fileStmt, 4))}
                };
                auto it = std::find_if(records.begin(), records.end(), [workId](const WorkRecord& r){ return r.id == workId; });
                if (it != records.end()) {
                    it->file_info[fileId] = fileJson.dump();
                }
            }
            sqlite3_finalize(fileStmt);
        }
    }
    return records;
}

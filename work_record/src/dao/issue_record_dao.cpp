#include"dao/issue_record_dao.h"
#include <string>
#include <numeric>
#include "constants/constants_sql.h"
#include "util/db_util.h"
using namespace db_util;
// 查询所有问题记录
std::vector<IssueRecord> queryAllIssuesDao(sqlite3* db) {
    std::vector<IssueRecord> records;
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_ISSUE_RECORD, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        IssueRecord r;
        r.id = sqlite3_column_int(s, 0);
        r.issue_title = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        r.reported_by = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        r.description = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));

        r.progress_id = sqlite3_column_int(s, 4);
        r.progress_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 5));
        r.progress_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
        r.responsible_person = reinterpret_cast<const char*>(sqlite3_column_text(s, 7));
        r.affected_id = sqlite3_column_int(s, 8);
        r.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 9));
        r.source_type_id = sqlite3_column_int(s, 10);
        r.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 11));
        r.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 12));
        r.update_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 13));
        r.employee_id = sqlite3_column_type(s, 14) != SQLITE_NULL ? sqlite3_column_int(s, 14) : 0;
        r.employee_name = sqlite3_column_type(s, 15) != SQLITE_NULL ? reinterpret_cast<const char*>(sqlite3_column_text(s, 15)) : "";
        r.department_id = sqlite3_column_type(s, 16) != SQLITE_NULL ? sqlite3_column_int(s, 16) : 0;
        records.push_back(r);
    });
    return records;
}

// 新增问题记录
void insertIssueDao(sqlite3* db, const IssueRecord& r, int& newId) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_ISSUE_RECORD, &stmt);
    sqlite3_bind_text(stmt, 1, r.issue_title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, r.employee_id);
    sqlite3_bind_text(stmt, 3, r.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, r.progress_id);
    sqlite3_bind_text(stmt, 5, r.responsible_person.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, r.affected_id);
    sqlite3_bind_int(stmt, 7, r.source_type_id);
    sqlite3_bind_text(stmt, 8, r.reported_by.c_str(), -1, SQLITE_TRANSIENT);
    db_util::exec_stmt_done(db, stmt);
    newId = (int)sqlite3_last_insert_rowid(db);
}

// 修改问题记录
void updateIssueDao(sqlite3* db, const IssueRecord& r) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_ISSUE_RECORD, &stmt);
    sqlite3_bind_text(stmt, 1, r.issue_title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, r.employee_id);
    sqlite3_bind_text(stmt, 3, r.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, r.progress_id);
    sqlite3_bind_text(stmt, 5, r.responsible_person.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, r.affected_id);
    sqlite3_bind_int(stmt, 7, r.source_type_id);
    sqlite3_bind_text(stmt, 8, r.reported_by.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, r.id);
    db_util::exec_stmt_done(db, stmt);
}

// 删除问题记录
bool deleteIssueDao(sqlite3* db, int id) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_ISSUE_RECORD, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}

// 分页查询问题记录
std::vector<IssueRecord> queryIssuesPagedDao(sqlite3* db, int page, int page_size, int& total, const std::string& source_type_id, const std::string& affected_id) {
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
    std::string countSql = std::string(constants_sql::SQL_COUNT_ISSUE_RECORD_BASE) + whereClause + ";";
    sqlite3_stmt* countStmt = nullptr;
    db_util::prepare_throw(db, countSql.c_str(), &countStmt);
    if (sqlite3_step(countStmt) == SQLITE_ROW) {
        total = sqlite3_column_int(countStmt, 0);
    }
    sqlite3_finalize(countStmt);
    // 查询分页数据
    std::string sql = std::string(constants_sql::SQL_SELECT_ISSUE_RECORD_PAGED_BASE) + whereClause + " ORDER BY i.id DESC LIMIT ? OFFSET ?;";
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, sql.c_str(), &stmt);
    int offset = (page - 1) * page_size;
    sqlite3_bind_int(stmt, 1, page_size);
    sqlite3_bind_int(stmt, 2, offset);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        IssueRecord r;
        r.id = sqlite3_column_int(s, 0);
        r.issue_title = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        r.reported_by = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        r.description = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        r.progress_id = sqlite3_column_int(s, 4);
        r.progress_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 5));
        r.progress_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 6));
        r.responsible_person = reinterpret_cast<const char*>(sqlite3_column_text(s, 7));
        r.affected_id = sqlite3_column_int(s, 8);
        r.affected_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 9));
        r.source_type_id = sqlite3_column_int(s, 10);
        r.source_type_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 11));
        r.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 12));
        r.update_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 13));
        r.employee_id = sqlite3_column_type(s, 14) != SQLITE_NULL ? sqlite3_column_int(s, 14) : 0;
        r.employee_name = sqlite3_column_type(s, 15) != SQLITE_NULL ? reinterpret_cast<const char*>(sqlite3_column_text(s, 15)) : "";
        r.department_id = sqlite3_column_type(s, 16) != SQLITE_NULL ? sqlite3_column_int(s, 16) : 0;
        records.push_back(r);
    });
    return records;
}

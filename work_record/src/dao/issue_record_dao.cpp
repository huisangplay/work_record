#include "dao/issue_record_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"
#include <string>
#include <numeric>

using namespace dao_util;

// 查询所有问题记录
DaoResult queryAllIssues(sqlite3 *db, std::vector<IssueRecord> &records) {
    records.clear();
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_ISSUE_RECORD, stmt, "queryAllIssues");

    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
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
                         r.employee_name = sqlite3_column_type(s, 15) != SQLITE_NULL ? reinterpret_cast<const char*>(
                             sqlite3_column_text(s, 15)) : "";
                         r.department_id = sqlite3_column_type(s, 16) != SQLITE_NULL ? sqlite3_column_int(s, 16) : 0;
                         records.push_back(r);
                         }, "queryAllIssues");

    return DaoResult::SUCCESS;
}

// 新增问题记录
DaoResult insertIssue(sqlite3 *db, const IssueRecord &r, int64_t &newId) {
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_ISSUE_RECORD, stmt, "insertIssue");

    sqlite3_bind_text(stmt, 1, r.issue_title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, r.employee_id);
    sqlite3_bind_text(stmt, 3, r.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, r.progress_id);
    sqlite3_bind_text(stmt, 5, r.responsible_person.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 6, r.affected_id);
    sqlite3_bind_int64(stmt, 7, r.source_type_id);
    sqlite3_bind_text(stmt, 8, r.reported_by.c_str(), -1, SQLITE_TRANSIENT);

    DAO_SAFE_EXEC_DONE(db, stmt, "insertIssue");

    newId = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 修改问题记录
DaoResult updateIssue(sqlite3 *db, const IssueRecord &r) {
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_ISSUE_RECORD, stmt, "updateIssue");

    sqlite3_bind_text(stmt, 1, r.issue_title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, r.employee_id);
    sqlite3_bind_text(stmt, 3, r.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, r.progress_id);
    sqlite3_bind_text(stmt, 5, r.responsible_person.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 6, r.affected_id);
    sqlite3_bind_int64(stmt, 7, r.source_type_id);
    sqlite3_bind_text(stmt, 8, r.reported_by.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 9, r.id);

    DAO_SAFE_EXEC_DONE(db, stmt, "updateIssue");

    return DaoResult::SUCCESS;
}

// 删除问题记录
DaoResult deleteIssue(sqlite3 *db, const int64_t id) {
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_ISSUE_RECORD, stmt, "deleteIssue");

    sqlite3_bind_int64(stmt, 1, id);

    DAO_SAFE_EXEC_DONE(db, stmt, "deleteIssue");

    return DaoResult::SUCCESS;
}

DaoResult queryIssuesPaged(sqlite3 *db, const int page, const int page_size, int &total,
                           const std::string &source_type_id, const std::string &affected_id,
                           std::vector<IssueRecord> &records) {
    records.clear();

    // 构建WHERE条件
    std::string whereClause;
    std::vector<std::string> conditions;
    if (!source_type_id.empty()) {
        conditions.push_back("i.source_type_id = " + source_type_id);
    }
    if (!affected_id.empty()) {
        conditions.push_back("i.affected_id = " + affected_id);
    }
    if (!conditions.empty()) {
        whereClause = "WHERE " + std::accumulate(conditions.begin(), conditions.end(), std::string(),
                                                 [](const std::string &a, const std::string &b) {
                                                     return a + (a.empty() ? "" : " AND ") + b;
                                                 });
    }

    // 先查询总数
    std::string countSql = std::string(constants_sql::SQL_COUNT_ISSUE_RECORD_BASE) + whereClause + ";";
    sqlite3_stmt *countStmt = nullptr;

    DAO_SAFE_PREPARE(db, countSql.c_str(), countStmt, "queryIssuesPaged_count");

    DAO_SAFE_EXEC_SELECT(db, countStmt, [&](sqlite3_stmt* s) {
                         total = sqlite3_column_int(s, 0);
                         }, "queryIssuesPaged_count");

    // 查询分页数据
    std::string sql = std::string(constants_sql::SQL_SELECT_ISSUE_RECORD_PAGED_BASE) + whereClause +
                      " ORDER BY i.id DESC LIMIT ? OFFSET ?;";
    sqlite3_stmt *stmt = nullptr;

    DAO_SAFE_PREPARE(db, sql.c_str(), stmt, "queryIssuesPaged");

    int offset = (page - 1) * page_size;
    sqlite3_bind_int(stmt, 1, page_size);
    sqlite3_bind_int(stmt, 2, offset);

    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
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
                         r.employee_name = sqlite3_column_type(s, 15) != SQLITE_NULL ? reinterpret_cast<const char*>(
                             sqlite3_column_text(s, 15)) : "";
                         r.department_id = sqlite3_column_type(s, 16) != SQLITE_NULL ? sqlite3_column_int(s, 16) : 0;
                         records.push_back(r);
                         }, "queryIssuesPaged");

    return DaoResult::SUCCESS;
}

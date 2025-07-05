#include "dao/issue_progress_dict_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 查询所有问题进展字典
DaoResult queryAllIssueProgressDict(sqlite3* db, std::vector<IssueProgressDict>& list) {
    list.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_ISSUE_PROGRESS_DICT, stmt, "queryAllIssueProgressDict");
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        IssueProgressDict d;
        d.id = sqlite3_column_int(s, 0);
        d.progress = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.progress_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    }, "queryAllIssueProgressDict");
    
    return DaoResult::SUCCESS;
}

// 插入问题进展字典
DaoResult insertIssueProgressDict(sqlite3* db, IssueProgressDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_ISSUE_PROGRESS_DICT, stmt, "insertIssueProgressDict");
    
    sqlite3_bind_text(stmt, 1, item.progress.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.progress_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.comment.c_str(), -1, SQLITE_STATIC);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertIssueProgressDict");
    
    item.id = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 更新问题进展字典
DaoResult updateIssueProgressDict(sqlite3* db, const IssueProgressDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_ISSUE_PROGRESS_DICT, stmt, "updateIssueProgressDict");
    
    sqlite3_bind_text(stmt, 1, item.progress.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.progress_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, item.id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "updateIssueProgressDict");
    
    return DaoResult::SUCCESS;
}

// 删除问题进展字典
DaoResult deleteIssueProgressDict(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_ISSUE_PROGRESS_DICT, stmt, "deleteIssueProgressDict");
    
    sqlite3_bind_int(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteIssueProgressDict");
    
    return DaoResult::SUCCESS;
} 
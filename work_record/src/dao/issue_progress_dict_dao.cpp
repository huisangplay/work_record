#include"dao/issue_progress_dict_dao.h"
#include "util/db_util.h"
#include "constants/constants_sql.h"
using namespace db_util;

std::vector<IssueProgressDict> queryAllIssueProgressDict(sqlite3* db) {
    std::vector<IssueProgressDict> result;
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_ISSUE_PROGRESS_DICT, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        IssueProgressDict item;
        item.id = sqlite3_column_int(s, 0);
        item.progress = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        item.progress_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        item.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        result.push_back(item);
    });
    return result;
}

bool insertIssueProgressDict(sqlite3* db, IssueProgressDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_ISSUE_PROGRESS_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.progress.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.progress_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.comment.c_str(), -1, SQLITE_STATIC);
    if (!db_util::exec_stmt_done(db, stmt)) return false;
    item.id = sqlite3_last_insert_rowid(db);
    return true;
}

bool updateIssueProgressDict(sqlite3* db, const IssueProgressDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_ISSUE_PROGRESS_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.progress.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.progress_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, item.id);
    return db_util::exec_stmt_done(db, stmt);
}

bool deleteIssueProgressDict(sqlite3* db, int id) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_ISSUE_PROGRESS_DICT, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}
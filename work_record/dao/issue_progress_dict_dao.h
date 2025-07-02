#pragma once
#include "../model/issue_progress_dict_model.h"
#include "../db/sqlite3.h"
#include <vector>

inline std::vector<IssueProgressDict> queryAllIssueProgressDict(sqlite3* db) {
    std::vector<IssueProgressDict> result;
    const char* sql = constants_sql::SQL_SELECT_ALL_ISSUE_PROGRESS_DICT;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            IssueProgressDict item;
            item.id = sqlite3_column_int(stmt, 0);
            item.progress = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            item.progress_class = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            item.comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            result.push_back(item);
        }
        sqlite3_finalize(stmt);
    }
    return result;
}

inline bool insertIssueProgressDict(sqlite3* db, IssueProgressDict& item) {
    const char* sql = constants_sql::SQL_INSERT_ISSUE_PROGRESS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.progress.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.progress_class.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, item.comment.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            item.id = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

inline bool updateIssueProgressDict(sqlite3* db, const IssueProgressDict& item) {
    const char* sql = constants_sql::SQL_UPDATE_ISSUE_PROGRESS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.progress.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.progress_class.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, item.comment.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, item.id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

inline bool deleteIssueProgressDict(sqlite3* db, int id) {
    const char* sql = constants_sql::SQL_DELETE_ISSUE_PROGRESS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}
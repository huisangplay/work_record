#pragma once
#include "../model/work_type_dict_model.h"
#include "../db/sqlite3.h"
#include <vector>

inline std::vector<WorkTypeDict> queryAllWorkTypeDict(sqlite3* db) {
    std::vector<WorkTypeDict> list;
    const char* sql = constants_sql::SQL_SELECT_ALL_WORK_TYPE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            WorkTypeDict d;
            d.id = sqlite3_column_int(stmt, 0);
            d.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            d.comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            list.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return list;
}

inline bool insertWorkTypeDict(sqlite3* db, WorkTypeDict& item) {
    const char* sql = constants_sql::SQL_INSERT_WORK_TYPE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            item.id = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

inline bool updateWorkTypeDict(sqlite3* db, const WorkTypeDict& item) {
    const char* sql = constants_sql::SQL_UPDATE_WORK_TYPE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, item.id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

inline bool deleteWorkTypeDict(sqlite3* db, int id) {
    const char* sql = constants_sql::SQL_DELETE_WORK_TYPE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

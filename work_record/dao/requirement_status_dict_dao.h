#pragma once
#include "../model/requirement_status_dict_model.h"
#include "../db/sqlite3.h"
#include "../constants/constants_sql.h"
#include <vector>

inline std::vector<RequirementStatusDict> queryAllRequirementStatusDict(sqlite3* db) {
    std::vector<RequirementStatusDict> list;
    const char* sql = constants_sql::SQL_SELECT_ALL_REQUIREMENT_STATUS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            RequirementStatusDict d;
            d.id = sqlite3_column_int(stmt, 0);
            d.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            d.comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            d.requirement_status_class = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            list.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return list;    
}

inline bool insertRequirementStatusDict(sqlite3* db, RequirementStatusDict& item) {
    const char* sql = constants_sql::SQL_INSERT_REQUIREMENT_STATUS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.status.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, item.requirement_status_class.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            item.id = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

inline bool updateRequirementStatusDict(sqlite3* db, const RequirementStatusDict& item) {
    const char* sql = constants_sql::SQL_UPDATE_REQUIREMENT_STATUS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.status.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, item.requirement_status_class.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 4, item.id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

inline bool deleteRequirementStatusDict(sqlite3* db, int id) {
    const char* sql = constants_sql::SQL_DELETE_REQUIREMENT_STATUS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

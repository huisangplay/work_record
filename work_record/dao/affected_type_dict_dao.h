#pragma once
#include "../model/affected_type_dict_model.h"
#include "../db/sqlite3.h"
#include <vector>
#include "../constants/constants_sql.h"

inline std::vector<AffectedTypeDict> queryAllAffectedTypeDict(sqlite3* db) {
    std::vector<AffectedTypeDict> list;
    const char* sql = constants_sql::SQL_SELECT_ALL_AFFECTED_TYPE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            AffectedTypeDict d;
            d.id = sqlite3_column_int(stmt, 0);
            d.affected = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            d.comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            list.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return list;
}

inline bool insertAffectedTypeDict(sqlite3* db, AffectedTypeDict& item) {
    const char* sql = constants_sql::SQL_INSERT_AFFECTED_TYPE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.affected.c_str(), -1, SQLITE_STATIC);
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

inline bool updateAffectedTypeDict(sqlite3* db, const AffectedTypeDict& item) {
    const char* sql = constants_sql::SQL_UPDATE_AFFECTED_TYPE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.affected.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, item.id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

inline bool deleteAffectedTypeDict(sqlite3* db, int id) {
    const char* sql = constants_sql::SQL_DELETE_AFFECTED_TYPE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

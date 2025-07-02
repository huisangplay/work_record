#pragma once
#include "../model/work_record_status_dict_model.h"
#include "../db/sqlite3.h"
#include <vector>
#include "../constants/constants_sql.h"

inline std::vector<WorkRecordStatusDict> queryAllWorkRecordStatusDict(sqlite3* db) {
    std::vector<WorkRecordStatusDict> list;
    const char* sql = constants_sql::SQL_SELECT_ALL_WORK_RECORD_STATUS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            WorkRecordStatusDict d;
            d.id = sqlite3_column_int(stmt, 0);
            d.status_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            d.status_class = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            list.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return list;
}

inline bool insertWorkRecordStatusDict(sqlite3* db, WorkRecordStatusDict& item) {
    const char* sql = constants_sql::SQL_INSERT_WORK_RECORD_STATUS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.status_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.status_class.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            item.id = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

inline bool updateWorkRecordStatusDict(sqlite3* db, const WorkRecordStatusDict& item) {
    const char* sql = constants_sql::SQL_UPDATE_WORK_RECORD_STATUS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.status_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.status_class.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, item.id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

inline bool deleteWorkRecordStatusDict(sqlite3* db, int id) {
    const char* sql = constants_sql::SQL_DELETE_WORK_RECORD_STATUS_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

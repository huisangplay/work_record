#pragma once
#include "../model/department_dict_model.h"
#include "../db/sqlite3.h"
#include <vector>

inline std::vector<DepartmentDict> queryAllDepartmentDict(sqlite3* db) {
    std::vector<DepartmentDict> list;
    const char* sql = constants_sql::SQL_SELECT_ALL_DEPARTMENT_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            DepartmentDict d;
            d.id = sqlite3_column_int(stmt, 0);
            d.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            d.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            d.create_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            list.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return list;
}

inline bool insertDepartmentDict(sqlite3* db, DepartmentDict& item) {
    const char* sql = constants_sql::SQL_INSERT_DEPARTMENT_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.description.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            item.id = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

inline bool updateDepartmentDict(sqlite3* db, const DepartmentDict& item) {
    const char* sql = constants_sql::SQL_UPDATE_DEPARTMENT_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, item.id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

inline bool deleteDepartmentDict(sqlite3* db, int id) {
    const char* sql = constants_sql::SQL_DELETE_DEPARTMENT_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
} 
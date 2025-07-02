#pragma once
#include "../model/employee_dict_model.h"
#include "../db/sqlite3.h"
#include <vector>

inline std::vector<EmployeeDict> queryAllEmployeeDict(sqlite3* db) {
    std::vector<EmployeeDict> list;
    const char* sql = constants_sql::SQL_SELECT_ALL_EMPLOYEE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            EmployeeDict d;
            d.id = sqlite3_column_int(stmt, 0);
            d.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            d.employee_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            d.department_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            list.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return list;
}

inline bool insertEmployeeDict(sqlite3* db, EmployeeDict& item) {
    // 根据部门名称查找部门ID
    int department_id = 0;
    if (!item.department_name.empty()) {
        const char* deptSql = constants_sql::SQL_SELECT_EMPLOYEE_DEPT_ID_BY_NAME;
        sqlite3_stmt* deptStmt;
        if (sqlite3_prepare_v2(db, deptSql, -1, &deptStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(deptStmt, 1, item.department_name.c_str(), -1, SQLITE_STATIC);
            if (sqlite3_step(deptStmt) == SQLITE_ROW) {
                department_id = sqlite3_column_int(deptStmt, 0);
            }
            sqlite3_finalize(deptStmt);
        }
    }
    const char* sql = constants_sql::SQL_INSERT_EMPLOYEE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.employee_number.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, department_id);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            item.id = sqlite3_last_insert_rowid(db);
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

inline bool updateEmployeeDict(sqlite3* db, const EmployeeDict& item) {
    int department_id = 0;
    if (!item.department_name.empty()) {
        const char* deptSql = constants_sql::SQL_SELECT_EMPLOYEE_DEPT_ID_BY_NAME;
        sqlite3_stmt* deptStmt;
        if (sqlite3_prepare_v2(db, deptSql, -1, &deptStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(deptStmt, 1, item.department_name.c_str(), -1, SQLITE_STATIC);
            if (sqlite3_step(deptStmt) == SQLITE_ROW) {
                department_id = sqlite3_column_int(deptStmt, 0);
            }
            sqlite3_finalize(deptStmt);
        }
    }
    const char* sql = constants_sql::SQL_UPDATE_EMPLOYEE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, item.employee_number.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, department_id);
        sqlite3_bind_int(stmt, 4, item.id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

inline bool deleteEmployeeDict(sqlite3* db, int id) {
    const char* sql = constants_sql::SQL_DELETE_EMPLOYEE_DICT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }
    return false;
}

inline std::vector<EmployeeDict> queryEmployeeByDepartment(sqlite3* db, int department_id) {
    std::vector<EmployeeDict> list;
    const char* sql = constants_sql::SQL_SELECT_EMPLOYEE_BY_DEPARTMENT;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, department_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            EmployeeDict d;
            d.id = sqlite3_column_int(stmt, 0);
            d.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            d.employee_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            d.department_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            list.push_back(d);
        }
        sqlite3_finalize(stmt);
    }
    return list;
} 
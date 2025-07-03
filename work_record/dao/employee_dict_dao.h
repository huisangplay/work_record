#pragma once
#include "../model/employee_dict_model.h"
#include "../db/sqlite3.h"
#include "../db/db_util.h"
#include <vector>
using namespace db_util;
inline std::vector<EmployeeDict> queryAllEmployeeDict(sqlite3* db) {
    std::vector<EmployeeDict> list;
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_EMPLOYEE_DICT, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        EmployeeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.employee_number = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.department_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    });
    return list;
}

inline bool insertEmployeeDict(sqlite3* db, EmployeeDict& item) {
    int department_id = 0;
    if (!item.department_name.empty()) {
        sqlite3_stmt* deptStmt;
        db_util::prepare_throw(db, constants_sql::SQL_SELECT_EMPLOYEE_DEPT_ID_BY_NAME, &deptStmt);
        sqlite3_bind_text(deptStmt, 1, item.department_name.c_str(), -1, SQLITE_STATIC);
        db_util::exec_select(db, deptStmt, [&](sqlite3_stmt* s){
            department_id = sqlite3_column_int(s, 0);
        });
    }
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_EMPLOYEE_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.employee_number.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, department_id);
    if (!db_util::exec_stmt_done(db, stmt)) return false;
    item.id = sqlite3_last_insert_rowid(db);
    return true;
}

inline bool updateEmployeeDict(sqlite3* db, const EmployeeDict& item) {
    int department_id = 0;
    if (!item.department_name.empty()) {
        sqlite3_stmt* deptStmt;
        db_util::prepare_throw(db, constants_sql::SQL_SELECT_EMPLOYEE_DEPT_ID_BY_NAME, &deptStmt);
        sqlite3_bind_text(deptStmt, 1, item.department_name.c_str(), -1, SQLITE_STATIC);
        db_util::exec_select(db, deptStmt, [&](sqlite3_stmt* s){
            department_id = sqlite3_column_int(s, 0);
        });
    }
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_EMPLOYEE_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.employee_number.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, department_id);
    sqlite3_bind_int(stmt, 4, item.id);
    return db_util::exec_stmt_done(db, stmt);
}

inline bool deleteEmployeeDict(sqlite3* db, int id) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_EMPLOYEE_DICT, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}

inline std::vector<EmployeeDict> queryEmployeeByDepartment(sqlite3* db, int department_id) {
    std::vector<EmployeeDict> list;
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_EMPLOYEE_BY_DEPARTMENT, &stmt);
    sqlite3_bind_int(stmt, 1, department_id);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        EmployeeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.employee_number = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.department_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    });
    return list;
} 
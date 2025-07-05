#include "dao/employee_dict_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 查询所有员工字典
DaoResult queryAllEmployeeDict(sqlite3* db, std::vector<EmployeeDict>& list) {
    list.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_EMPLOYEE_DICT, stmt, "queryAllEmployeeDict");
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        EmployeeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.employee_number = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.department_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    }, "queryAllEmployeeDict");
    
    return DaoResult::SUCCESS;
}

// 插入员工字典
DaoResult insertEmployeeDict(sqlite3* db, EmployeeDict& item) {
    int department_id = 0;
    
    // 查询部门ID
    if (!item.department_name.empty()) {
        sqlite3_stmt* deptStmt = nullptr;
        DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_EMPLOYEE_DEPT_ID_BY_NAME, deptStmt, "insertEmployeeDict_dept");
        
        sqlite3_bind_text(deptStmt, 1, item.department_name.c_str(), -1, SQLITE_STATIC);
        
        DAO_SAFE_EXEC_SELECT(db, deptStmt, [&](sqlite3_stmt* s) {
            department_id = sqlite3_column_int(s, 0);
        }, "insertEmployeeDict_dept");
    }
    
    // 插入员工记录
    sqlite3_stmt* stmt = nullptr;
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_EMPLOYEE_DICT, stmt, "insertEmployeeDict");
    
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.employee_number.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, department_id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertEmployeeDict");
    
    item.id = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 更新员工字典
DaoResult updateEmployeeDict(sqlite3* db, const EmployeeDict& item) {
    int department_id = 0;
    
    // 查询部门ID
    if (!item.department_name.empty()) {
        sqlite3_stmt* deptStmt = nullptr;
        DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_EMPLOYEE_DEPT_ID_BY_NAME, deptStmt, "updateEmployeeDict_dept");
        
        sqlite3_bind_text(deptStmt, 1, item.department_name.c_str(), -1, SQLITE_STATIC);
        
        DAO_SAFE_EXEC_SELECT(db, deptStmt, [&](sqlite3_stmt* s) {
            department_id = sqlite3_column_int(s, 0);
        }, "updateEmployeeDict_dept");
    }
    
    // 更新员工记录
    sqlite3_stmt* stmt = nullptr;
    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_EMPLOYEE_DICT, stmt, "updateEmployeeDict");
    
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.employee_number.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, department_id);
    sqlite3_bind_int(stmt, 4, item.id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "updateEmployeeDict");
    
    return DaoResult::SUCCESS;
}

// 删除员工字典
DaoResult deleteEmployeeDict(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_EMPLOYEE_DICT, stmt, "deleteEmployeeDict");
    
    sqlite3_bind_int(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteEmployeeDict");
    
    return DaoResult::SUCCESS;
}

// 根据部门查询员工
DaoResult queryEmployeeByDepartment(sqlite3* db, int department_id, std::vector<EmployeeDict>& list) {
    list.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_EMPLOYEE_BY_DEPARTMENT, stmt, "queryEmployeeByDepartment");
    
    sqlite3_bind_int(stmt, 1, department_id);
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        EmployeeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.employee_number = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.department_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    }, "queryEmployeeByDepartment");
    
    return DaoResult::SUCCESS;
} 
#include "dao/department_dict_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 查询所有部门字典
DaoResult queryAllDepartmentDict(sqlite3* db, std::vector<DepartmentDict>& list) {
    list.clear();
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_ALL_DEPARTMENT_DICT, stmt, "queryAllDepartmentDict");
    
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        DepartmentDict d;
        d.id = sqlite3_column_int(s, 0);
        d.name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.description = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    }, "queryAllDepartmentDict");
    
    return DaoResult::SUCCESS;
}

// 插入部门字典
DaoResult insertDepartmentDict(sqlite3* db, DepartmentDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_DEPARTMENT_DICT, stmt, "insertDepartmentDict");
    
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.description.c_str(), -1, SQLITE_STATIC);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertDepartmentDict");
    
    item.id = sqlite3_last_insert_rowid(db);
    return DaoResult::SUCCESS;
}

// 更新部门字典
DaoResult updateDepartmentDict(sqlite3* db, const DepartmentDict& item) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_UPDATE_DEPARTMENT_DICT, stmt, "updateDepartmentDict");
    
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "updateDepartmentDict");
    
    return DaoResult::SUCCESS;
}

// 删除部门字典
DaoResult deleteDepartmentDict(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_DEPARTMENT_DICT, stmt, "deleteDepartmentDict");
    
    sqlite3_bind_int(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteDepartmentDict");
    
    return DaoResult::SUCCESS;
}
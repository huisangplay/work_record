#include"dao/department_dict_dao.h"
#include "util/db_util.h"
#include "constants/constants_sql.h"
using namespace db_util;

std::vector<DepartmentDict> queryAllDepartmentDict(sqlite3* db) {
    std::vector<DepartmentDict> list;
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_DEPARTMENT_DICT, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        DepartmentDict d;
        d.id = sqlite3_column_int(s, 0);
        d.name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.description = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    });
    return list;
}

bool insertDepartmentDict(sqlite3* db, DepartmentDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_DEPARTMENT_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.description.c_str(), -1, SQLITE_STATIC);
    if (!db_util::exec_stmt_done(db, stmt)) return false;
    item.id = sqlite3_last_insert_rowid(db);
    return true;
}

bool updateDepartmentDict(sqlite3* db, const DepartmentDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_DEPARTMENT_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    return db_util::exec_stmt_done(db, stmt);
}

bool deleteDepartmentDict(sqlite3* db, int id) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_DEPARTMENT_DICT, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
} 
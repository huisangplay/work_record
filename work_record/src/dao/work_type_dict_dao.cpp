#include"dao/work_type_dict_dao.h"
#include "constants/constants_sql.h"
#include "util/db_util.h"
using namespace db_util;

std::vector<WorkTypeDict> queryAllWorkTypeDict(sqlite3* db) {
    std::vector<WorkTypeDict> list;
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_WORK_TYPE_DICT, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        WorkTypeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.type = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        list.push_back(d);
    });
    return list;
}

bool insertWorkTypeDict(sqlite3* db, WorkTypeDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_WORK_TYPE_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    bool ok = db_util::exec_stmt_done(db, stmt);
    if (ok) item.id = sqlite3_last_insert_rowid(db);
    return ok;
}

bool updateWorkTypeDict(sqlite3* db, const WorkTypeDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_WORK_TYPE_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    return db_util::exec_stmt_done(db, stmt);
}

bool deleteWorkTypeDict(sqlite3* db, int id) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_WORK_TYPE_DICT, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}

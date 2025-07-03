#pragma once
#include "../model/work_record_status_dict_model.h"
#include "../db/sqlite3.h"
#include <vector>
#include "../constants/constants_sql.h"
#include "../db/db_util.h"
using namespace db_util;

inline std::vector<WorkRecordStatusDict> queryAllWorkRecordStatusDict(sqlite3* db) {
    std::vector<WorkRecordStatusDict> list;
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_WORK_RECORD_STATUS_DICT, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        WorkRecordStatusDict d;
        d.id = sqlite3_column_int(s, 0);
        d.status_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.status_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        list.push_back(d);
    });
    return list;
}

inline bool insertWorkRecordStatusDict(sqlite3* db, WorkRecordStatusDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_WORK_RECORD_STATUS_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.status_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.status_class.c_str(), -1, SQLITE_STATIC);
    bool ok = db_util::exec_stmt_done(db, stmt);
    if (ok) item.id = sqlite3_last_insert_rowid(db);
    return ok;
}

inline bool updateWorkRecordStatusDict(sqlite3* db, const WorkRecordStatusDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_WORK_RECORD_STATUS_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.status_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.status_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    return db_util::exec_stmt_done(db, stmt);
}

inline bool deleteWorkRecordStatusDict(sqlite3* db, int id) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_WORK_RECORD_STATUS_DICT, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}

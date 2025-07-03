#pragma once
#include "../model/requirement_status_dict_model.h"
#include "../db/sqlite3.h"
#include "../constants/constants_sql.h"
#include <vector>
#include "../db/db_util.h"
using namespace db_util;

inline std::vector<RequirementStatusDict> queryAllRequirementStatusDict(sqlite3* db) {
    std::vector<RequirementStatusDict> list;
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_REQUIREMENT_STATUS_DICT, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        RequirementStatusDict d;
        d.id = sqlite3_column_int(s, 0);
        d.status = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        d.requirement_status_class = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        list.push_back(d);
    });
    return list;
}

inline bool insertRequirementStatusDict(sqlite3* db, RequirementStatusDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_REQUIREMENT_STATUS_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.requirement_status_class.c_str(), -1, SQLITE_STATIC);
    if (!db_util::exec_stmt_done(db, stmt)) return false;
    item.id = sqlite3_last_insert_rowid(db);
    return true;
}

inline bool updateRequirementStatusDict(sqlite3* db, const RequirementStatusDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_REQUIREMENT_STATUS_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, item.requirement_status_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, item.id);
    return db_util::exec_stmt_done(db, stmt);
}

inline bool deleteRequirementStatusDict(sqlite3* db, int id) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_REQUIREMENT_STATUS_DICT, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}

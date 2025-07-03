#pragma once
#include "../model/source_type_dict_model.h"
#include "../db/sqlite3.h"
#include "../db/db_util.h"
#include <vector>
using namespace db_util;

inline std::vector<SourceTypeDict> queryAllSourceTypeDict(sqlite3* db) {
    std::vector<SourceTypeDict> list;
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_SOURCE_TYPE_DICT, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        SourceTypeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.type = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        list.push_back(d);
    });
    return list;
}

inline bool insertSourceTypeDict(sqlite3* db, SourceTypeDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_SOURCE_TYPE_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    if (!db_util::exec_stmt_done(db, stmt)) return false;
    item.id = sqlite3_last_insert_rowid(db);
    return true;
}

inline bool updateSourceTypeDict(sqlite3* db, const SourceTypeDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_SOURCE_TYPE_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    return db_util::exec_stmt_done(db, stmt);
}

inline bool deleteSourceTypeDict(sqlite3* db, int id) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_SOURCE_TYPE_DICT, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}

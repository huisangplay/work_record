#pragma once
#include "../model/affected_type_dict_model.h"
#include "../db/sqlite3.h"
#include <vector>
#include "../constants/constants_sql.h"
#include "../db/db_util.h"
using namespace db_util;
inline std::vector<AffectedTypeDict> queryAllAffectedTypeDict(sqlite3* db) {
    std::vector<AffectedTypeDict> list;
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_SELECT_ALL_AFFECTED_TYPE_DICT, &stmt);
    db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){
        AffectedTypeDict d;
        d.id = sqlite3_column_int(s, 0);
        d.affected = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        d.comment = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        list.push_back(d);
    });
    return list;
}

inline bool insertAffectedTypeDict(sqlite3* db, AffectedTypeDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_AFFECTED_TYPE_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.affected.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    if (!db_util::exec_stmt_done(db, stmt)) return false;
    item.id = sqlite3_last_insert_rowid(db);
    return true;
}

inline bool updateAffectedTypeDict(sqlite3* db, const AffectedTypeDict& item) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_UPDATE_AFFECTED_TYPE_DICT, &stmt);
    sqlite3_bind_text(stmt, 1, item.affected.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, item.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, item.id);
    return db_util::exec_stmt_done(db, stmt);
}

inline bool deleteAffectedTypeDict(sqlite3* db, int id) {
    sqlite3_stmt* stmt;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_AFFECTED_TYPE_DICT, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    return db_util::exec_stmt_done(db, stmt);
}

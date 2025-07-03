#pragma once
#include "../constants/constants_sql.h"
#include "../db/sqlite3.h"
#include "../db/db_util.h"
using namespace db_util;

inline bool insertWorkFileRel(sqlite3* db, int workId, int fileId) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_WORK_FILE_REL, &stmt);
    sqlite3_bind_int(stmt, 1, workId);
    sqlite3_bind_int(stmt, 2, fileId);
    return db_util::exec_stmt_done(db, stmt);
}

inline bool deleteWorkFileRelByWork(sqlite3* db, int workId) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_WORK_FILE_REL, &stmt);
    sqlite3_bind_int(stmt, 1, workId);
    return db_util::exec_stmt_done(db, stmt);
}

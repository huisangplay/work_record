#pragma once
#include "../constants/constants_sql.h"
#include "../db/sqlite3.h"

inline bool insertWorkFileRel(sqlite3* db, int workId, int fileId) {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, constants_sql::SQL_INSERT_WORK_FILE_REL, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, workId);
    sqlite3_bind_int(stmt, 2, fileId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

inline bool deleteWorkFileRelByWork(sqlite3* db, int workId) {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, constants_sql::SQL_DELETE_WORK_FILE_REL, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, workId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

#include"dao/work_record_file_dao.h"
#include "constants/constants_sql.h"
#include "util/db_util.h"
using namespace db_util;

bool insertWorkFileRel(sqlite3* db, int workId, int fileId) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_INSERT_WORK_FILE_REL, &stmt);
    sqlite3_bind_int(stmt, 1, workId);
    sqlite3_bind_int(stmt, 2, fileId);
    return db_util::exec_stmt_done(db, stmt);
}

bool deleteWorkFileRelByWork(sqlite3* db, int workId) {
    sqlite3_stmt* stmt = nullptr;
    db_util::prepare_throw(db, constants_sql::SQL_DELETE_WORK_FILE_REL, &stmt);
    sqlite3_bind_int(stmt, 1, workId);
    return db_util::exec_stmt_done(db, stmt);
}

#include "dao/work_record_file_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 插入工单文件关联
DaoResult insertWorkFileRel(sqlite3* db, int workId, int fileId) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_WORK_FILE_REL, stmt, "insertWorkFileRel");
    
    sqlite3_bind_int(stmt, 1, workId);
    sqlite3_bind_int(stmt, 2, fileId);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "insertWorkFileRel");
    
    return DaoResult::SUCCESS;
}

DaoResult deleteWorkFileRelByWork(sqlite3* db, int workId) {
    sqlite3_stmt* stmt = nullptr;
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_WORK_FILE_REL, stmt,"deleteWorkFileRelByWork");
    sqlite3_bind_int(stmt, 1, workId);
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteWorkFileRelByWork");
    return DaoResult::SUCCESS;
}

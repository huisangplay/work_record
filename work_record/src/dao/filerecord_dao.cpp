#include "dao/filerecord_dao.h"
#include "util/dao_util.h"
#include "constants/constants_sql.h"

using namespace dao_util;

// 插入文件记录
DaoResult insertFileRecord(sqlite3* db, const FileRecord& record, int64_t& newId) {
    sqlite3_stmt* stmt = nullptr;

    DAO_SAFE_PREPARE(db, constants_sql::SQL_INSERT_FILE_RECORD, stmt, "insertFileRecord");

    sqlite3_bind_text(stmt, 1, record.file_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, record.file_path.c_str(), -1, SQLITE_TRANSIENT);

    DAO_SAFE_EXEC_DONE(db, stmt, "insertFileRecord");

    newId = static_cast<int64_t>(sqlite3_last_insert_rowid(db));
    return DaoResult::SUCCESS;
}

// 根据ID获取文件记录
DaoResult getFileRecordById(sqlite3* db, int64_t id, FileRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_SELECT_FILE_RECORD_BY_ID, stmt, "getFileRecordById");
    
    sqlite3_bind_int64(stmt, 1, id);
    
    bool found = false;
    DAO_SAFE_EXEC_SELECT(db, stmt, [&](sqlite3_stmt* s) {
        record.id = sqlite3_column_int(s, 0);
        record.file_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        record.file_path = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        record.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        found = true;
    }, "getFileRecordById");
    
    return found ? DaoResult::SUCCESS : DaoResult::NOT_FOUND;
}

// 删除文件记录
DaoResult deleteFileRecord(sqlite3* db, int64_t id) {
    sqlite3_stmt* stmt = nullptr;
    
    DAO_SAFE_PREPARE(db, constants_sql::SQL_DELETE_FILE_RECORD, stmt, "deleteFileRecord");
    
    sqlite3_bind_int64(stmt, 1, id);
    
    DAO_SAFE_EXEC_DONE(db, stmt, "deleteFileRecord");
    
    return DaoResult::SUCCESS;
} 
#pragma once
#include "../model/file_record_model.h"
#include "../constants/constants_sql.h"
#include "../db/sqlite3.h"
#include "../db/db_util.h"
#include <string>
using namespace db_util;

inline bool insertFileRecord(sqlite3* db, const FileRecord& fileRec, int& newId) {
    sqlite3_stmt* stmt = nullptr;
    prepare_throw(db, constants_sql::SQL_INSERT_FILE_RECORD, &stmt);
    sqlite3_bind_text(stmt, 1, fileRec.file_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, fileRec.file_path.c_str(), -1, SQLITE_TRANSIENT);
    if (!exec_stmt_done(db, stmt)) return false;
    newId = (int)sqlite3_last_insert_rowid(db);
    return true;
}

inline bool deleteFileRecord(sqlite3* db, int fileId) {
    sqlite3_stmt* stmt = nullptr;
    prepare_throw(db, constants_sql::SQL_DELETE_FILE_RECORD, &stmt);
    sqlite3_bind_int(stmt, 1, fileId);
    return exec_stmt_done(db, stmt);
}

// 根据ID获取FileRecord
inline bool getFileRecordById(sqlite3* db, int id, FileRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    prepare_throw(db, constants_sql::SQL_SELECT_FILE_RECORD_BY_ID, &stmt);
    sqlite3_bind_int(stmt, 1, id);
    bool found = false;
    exec_select(db, stmt, [&](sqlite3_stmt* s){
        record.id = sqlite3_column_int(s, 0);
        record.file_name = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        record.file_path = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        record.create_time = reinterpret_cast<const char*>(sqlite3_column_text(s, 3));
        found = true;
    });
    return found;
}

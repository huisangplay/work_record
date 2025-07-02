#pragma once
#include "../model/file_record_model.h"
#include "../constants/constants_sql.h"
#include "../db/sqlite3.h"
#include <string>

inline bool insertFileRecord(sqlite3* db, const FileRecord& fileRec, int& newId) {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, constants_sql::SQL_INSERT_FILE_RECORD, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, fileRec.file_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, fileRec.file_path.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) { sqlite3_finalize(stmt); return false; }
    newId = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return true;
}

inline bool deleteFileRecord(sqlite3* db, int fileId) {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, constants_sql::SQL_DELETE_FILE_RECORD, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, fileId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// 根据ID获取FileRecord
inline bool getFileRecordById(sqlite3* db, int id, FileRecord& record) {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, file_name, file_path, create_time FROM file_record WHERE id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_bind_int(stmt, 1, id);
    bool found = false;
    int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW) {
        record.id = sqlite3_column_int(stmt, 0);
        record.file_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        record.file_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        record.create_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        found = true;
    } else if (step != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error(std::string("SQL step failed: ") + sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    return found;
}

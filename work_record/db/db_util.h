#pragma once
#include <string>
#include <iostream>
#include "sqlite3.h"
#include <stdexcept>

constexpr const char* DB_PATH = "db/work_record.db";
extern sqlite3* db;
inline sqlite3* openDB(const std::string& db_name = DB_PATH) {
    if (sqlite3_open(db_name.c_str(), &db) != SQLITE_OK) {
        std::cerr << "数据库连接失败: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
    return db;
}

// 通用SQL执行函数，异常时抛出 std::runtime_error
inline void exec_sql(sqlite3* db, const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string msg = errMsg ? errMsg : "Unknown SQL error";
        sqlite3_free(errMsg);
        throw std::runtime_error("SQL执行失败: " + msg + "\nSQL: " + sql);
    }
}

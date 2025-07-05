#pragma once
#include <string>

#include <iostream>

#include <sqlite3/sqlite3.h>

#include <stdexcept>

namespace db_util {

    inline sqlite3 * openDB(const std::string & db_name) {
        sqlite3 * db = nullptr;
        if (sqlite3_open(db_name.c_str(), &db) != SQLITE_OK) {
            std::cerr << "数据库连接失败: " << sqlite3_errmsg(db) << std::endl;
            return nullptr;
        }
        sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
        return db;
    }

    // 通用SQL执行函数，异常时抛出 std::runtime_error
    inline void exec_sql(sqlite3 * db,
        const std::string & sql) {
        char * errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, & errMsg);
        if (rc != SQLITE_OK) {
            std::string msg = errMsg ? errMsg : "Unknown SQL error";
            sqlite3_free(errMsg);
            throw std::runtime_error("SQL执行失败: " + msg + "\nSQL: " + sql);
        }
    }

    // 通用sqlite3_step执行（适用于insert/update/delete等），自动finalize，失败抛异常
    inline bool exec_stmt_done(sqlite3 * db, sqlite3_stmt * stmt) {
        int rc = sqlite3_step(stmt);
        bool ok = (rc == SQLITE_DONE);
        if (!ok) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw std::runtime_error("SQL step failed: " + err);
        }
        sqlite3_finalize(stmt);
        return ok;
    }

    // 通用select查询，自动step、finalize、错误处理，lambda处理每一行
    // 用法：db_util::exec_select(db, stmt, [&](sqlite3_stmt* s){ ... });
    template < typename RowFunc >
        inline void exec_select(sqlite3 * db, sqlite3_stmt * stmt, RowFunc rowFunc) {
            while (true) {
                int step = sqlite3_step(stmt);
                if (step == SQLITE_ROW) {
                    rowFunc(stmt);
                } else if (step == SQLITE_DONE) {
                    break;
                } else {
                    sqlite3_finalize(stmt);
                    throw std::runtime_error(std::string("SQL step failed: ") + sqlite3_errmsg(db));
                }
            }
            sqlite3_finalize(stmt);
        }

    // 统一prepare+错误处理
    inline void prepare_throw(sqlite3 * db,
        const char * sql, sqlite3_stmt ** stmt) {
        int rc = sqlite3_prepare_v2(db, sql, -1, stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error(std::string("SQL prepare failed: ") + sqlite3_errmsg(db));
        }
    }

    // 通用事务包装，lambda里抛异常自动rollback，否则commit
    template<typename Func>
    inline void with_transaction(sqlite3* db, Func func) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, "BEGIN;", 0, 0, &errMsg) != SQLITE_OK) {
            std::string msg = errMsg ? errMsg : "Unknown SQL error";
            sqlite3_free(errMsg);
            throw std::runtime_error("BEGIN TRANSACTION failed: " + msg);
        }
        try {
            func();
            if (sqlite3_exec(db, "COMMIT;", 0, 0, &errMsg) != SQLITE_OK) {
                std::string msg = errMsg ? errMsg : "Unknown SQL error";
                sqlite3_free(errMsg);
                throw std::runtime_error("COMMIT failed: " + msg);
            }
        } catch (...) {
            sqlite3_exec(db, "ROLLBACK;", 0, 0, nullptr);
            throw;
        }
    }

    // step 并自动 finalize，失败抛异常
    inline void step_throw(sqlite3* db, sqlite3_stmt* stmt) {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw std::runtime_error("SQL step failed: " + err);
        }
        sqlite3_finalize(stmt);
    }
}
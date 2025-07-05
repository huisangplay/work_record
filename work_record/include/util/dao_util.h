#pragma once
#include <sqlite3/sqlite3.h>
#include <string>
#include "util/log_util.h"

namespace dao_util {

    // 数据库操作结果枚举
    enum class DaoResult {
        SUCCESS,
        FAILED,
        NOT_FOUND
    };

    // 通用SQL执行函数，异常时记录日志而不是抛出
    inline DaoResult exec_sql_safe(sqlite3* db, const std::string& sql, const std::string& context = "") {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string msg = errMsg ? errMsg : "Unknown SQL error";
            sqlite3_free(errMsg);
            std::runtime_error e("SQL执行失败: " + msg + "\nSQL: " + sql);
            log_util::log_exception(e, context);
            return DaoResult::FAILED;
        }
        return DaoResult::SUCCESS;
    }

    // 通用sqlite3_step执行（适用于insert/update/delete等），自动finalize，失败记录日志
    inline DaoResult exec_stmt_done_safe(sqlite3* db, sqlite3_stmt* stmt, const std::string& context = "") {
        int rc = sqlite3_step(stmt);
        bool ok = (rc == SQLITE_DONE);
        if (!ok) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            std::runtime_error e("SQL step failed: " + err);
            log_util::log_exception(e, context);
            return DaoResult::FAILED;
        }
        sqlite3_finalize(stmt);
        return DaoResult::SUCCESS;
    }

    // 通用select查询，自动step、finalize、错误处理，lambda处理每一行
    template<typename RowFunc>
    inline DaoResult exec_select_safe(sqlite3* db, sqlite3_stmt* stmt, RowFunc rowFunc, const std::string& context = "") {
        try {
            while (true) {
                int step = sqlite3_step(stmt);
                if (step == SQLITE_ROW) {
                    rowFunc(stmt);
                } else if (step == SQLITE_DONE) {
                    break;
                } else {
                    sqlite3_finalize(stmt);
                    std::runtime_error e(std::string("SQL step failed: ") + sqlite3_errmsg(db));
                    log_util::log_exception(e, context);
                    return DaoResult::FAILED;
                }
            }
            sqlite3_finalize(stmt);
            return DaoResult::SUCCESS;
        } catch (const std::exception& e) {
            sqlite3_finalize(stmt);
            log_util::log_exception(e, context);
            return DaoResult::FAILED;
        }
    }

    // 统一prepare+错误处理，失败记录日志
    inline DaoResult prepare_safe(sqlite3* db, const char* sql, sqlite3_stmt** stmt, const std::string& context = "") {
        int rc = sqlite3_prepare_v2(db, sql, -1, stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::runtime_error e(std::string("SQL prepare failed: ") + sqlite3_errmsg(db) + "\nSQL: " + sql);
            log_util::log_exception(e, context);
            return DaoResult::FAILED;
        }
        return DaoResult::SUCCESS;
    }

    // 通用事务包装，lambda里抛异常自动rollback并记录日志
    template<typename Func>
    inline DaoResult with_transaction_safe(sqlite3* db, Func func, const std::string& context = "") {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, "BEGIN;", 0, 0, &errMsg) != SQLITE_OK) {
            std::string msg = errMsg ? errMsg : "Unknown SQL error";
            sqlite3_free(errMsg);
            std::runtime_error e("BEGIN TRANSACTION failed: " + msg);
            log_util::log_exception(e, context);
            return DaoResult::FAILED;
        }
        
        try {
            func();
            if (sqlite3_exec(db, "COMMIT;", 0, 0, &errMsg) != SQLITE_OK) {
                std::string msg = errMsg ? errMsg : "Unknown SQL error";
                sqlite3_free(errMsg);
                std::runtime_error e("COMMIT failed: " + msg);
                log_util::log_exception(e, context);
                return DaoResult::FAILED;
            }
            return DaoResult::SUCCESS;
        } catch (const std::exception& e) {
            sqlite3_exec(db, "ROLLBACK;", 0, 0, nullptr);
            log_util::log_exception(e, context);
            return DaoResult::FAILED;
        }
    }

    // step 并自动 finalize，失败记录日志
    inline DaoResult step_safe(sqlite3* db, sqlite3_stmt* stmt, const std::string& context = "") {
        int rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::string err = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            std::runtime_error e("SQL step failed: " + err);
            log_util::log_exception(e, context);
            return DaoResult::FAILED;
        }
        sqlite3_finalize(stmt);
        return DaoResult::SUCCESS;
    }

    // 辅助函数：将DaoResult转换为bool
    inline bool is_success(DaoResult result) {
        return result == DaoResult::SUCCESS;
    }

    // 辅助函数：将DaoResult转换为bool（包含NOT_FOUND）
    inline bool is_success_or_not_found(DaoResult result) {
        return result == DaoResult::SUCCESS || result == DaoResult::NOT_FOUND;
    }

    // 辅助宏：简化DAO操作的错误处理
    #define DAO_SAFE_PREPARE(db, sql, stmt, context) \
        if (dao_util::prepare_safe(db, sql, &stmt, context) != dao_util::DaoResult::SUCCESS) { \
            return dao_util::DaoResult::FAILED; \
        }

    #define DAO_SAFE_EXEC_DONE(db, stmt, context) \
        if (dao_util::exec_stmt_done_safe(db, stmt, context) != dao_util::DaoResult::SUCCESS) { \
            return dao_util::DaoResult::FAILED; \
        }

    #define DAO_SAFE_EXEC_SELECT(db, stmt, rowFunc, context) \
        if (dao_util::exec_select_safe(db, stmt, rowFunc, context) != dao_util::DaoResult::SUCCESS) { \
            return dao_util::DaoResult::FAILED; \
        }
} 
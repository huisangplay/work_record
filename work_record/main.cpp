#ifdef _WIN32
#include <winsock2.h>
#endif
#include "service/work_service.h"
#include "service/department_service.h"
#include "service/employee_service.h"
#include "util/file_util.h"
#include <iostream>
#include <httplib/httplib.h>
#include <nlohmann/json.hpp>
#include <sqlite3/sqlite3.h>
#include "service/issue_service.h"
#include "service/dict_service.h"
#include "util/db_util.h"
#include "service/requirement_service.h"
#include "util/log_util.h"
#include "util/dao_util.h"

using namespace httplib;
using namespace dao_util;
using json = nlohmann::json;

constexpr const char* DB_PATH = "db/work_record.db";
sqlite3* db = nullptr;

// 安全的数据库初始化
bool initialize_database() {
    spdlog::info("正在初始化数据库连接...");
    
    db = db_util::openDB(DB_PATH);
    if (!db) {
        spdlog::error("数据库连接失败: {}", DB_PATH);
        return false;
    }
    
    // 测试数据库连接
    auto result = exec_sql_safe(db, "SELECT 1", "database_connection_test");
    if (result != DaoResult::SUCCESS) {
        spdlog::error("数据库连接测试失败");
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    
    spdlog::info("数据库连接成功: {}", DB_PATH);
    return true;
}

// 安全的服务启动
bool start_server() {
    try {
        Server svr;
        
        // 设置静态文件目录
        svr.set_base_dir("./static");
        
        // 设置路由
        // [GET] 获取工作记录
        svr.Get("/api/work_records", get_all_work_records);
        
        // [GET] 根据ID获取单条工作记录
        svr.Get("/api/work_record", get_work_record_by_id);

        // [GET] 获取需求记录
        svr.Get("/api/requirement_records", get_all_requirement_records);

        // [GET] 获取问题记录（分页）
        svr.Get("/api/issues", get_issues_paged);

        // [POST] 创建工作记录（含文件上传）
        svr.Post("/api/get_work_records", get_work_records);

        // [GET] 工作记录状态字典
        svr.Get("/api/work_record_status_dict", get_work_record_status_dict);
        
        // [DELETE] 删除工作记录
        svr.Delete(R"(/api/work_record/(\d+))", delete_work_record);
        
        // [POST] 工作记录上传文件
        svr.Post(R"(/api/work_record/(\d+)/upload)", upload_work_record_file);
        
        // [GET] 文件下载
        svr.Get("/api/download", download_file);
        
        // [DELETE] 删除单个文件
        svr.Delete(R"(/api/file/(\d+))", delete_file);
        
        // [GET] 通用字典接口
        svr.Get("/api/dict", get_dict);
        svr.Post("/api/dict", add_dict);
        svr.Put("/api/dict", update_dict);
        svr.Delete("/api/dict", delete_dict);
        
        // [POST] 更新工作记录
        svr.Post("/api/update_work_record", update_work_record);
        
        // [POST] 需求进展新增/修改（自动分发）
        svr.Post("/api/requirement_record", add_or_update_requirement_record);
        
        // [DELETE] 需求进展删除（只支持 GET 参数 id）
        svr.Delete("/api/delete_requirement_record", delete_requirement_record);
        
        // [POST] 新增问题
        svr.Post("/api/add_issue", add_issue);
        
        // [POST] 修改问题
        svr.Post("/api/update_issue", update_issue);
        
        // [POST] 删除问题
        svr.Post("/api/delete_issue", delete_issue);

        // [GET] 获取所有部门
        svr.Get("/api/departments", get_all_departments);
        
        // [POST] 新增部门
        svr.Post("/api/department", add_department);
        
        // [POST] 更新部门
        svr.Post("/api/update_department", update_department);
        
        // [DELETE] 删除部门
        svr.Delete("/api/department", delete_department);
        
        // [GET] 获取所有人员
        svr.Get("/api/employees", get_all_employees);
        
        // [POST] 新增人员
        svr.Post("/api/employee", add_employee);
        
        // [POST] 更新人员
        svr.Post("/api/update_employee", update_employee);
        
        // [DELETE] 删除人员
        svr.Delete("/api/employee", delete_employee);
        
        // 添加错误处理中间件
        svr.set_exception_handler([](const auto& req, auto& res, const std::exception_ptr &ep) {
            try {
                std::rethrow_exception(ep);
            } catch (const std::exception& e) {
                spdlog::error("HTTP请求异常: {} - {}", req.path, e.what());
                json error_response = {
                    {"success", false},
                    {"message", "服务器内部错误"},
                    {"error", e.what()}
                };
                res.set_content(error_response.dump(), "application/json");
                res.status = 500;
            }
        });
        
        // 添加日志中间件
        svr.set_logger([](const auto& req, const auto& res) {
            spdlog::info("HTTP {} {} - {}", req.method, req.path, res.status);
        });
        
        std::cout << "🚀 服务已启动：http://localhost:8080\n";
        spdlog::info("HTTP服务器启动成功，监听端口: 8080");
        
        svr.listen("0.0.0.0", 8080);
        return true;
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "start_server");
        return false;
    }
}

// 安全的清理函数
void cleanup() {
    spdlog::info("正在清理资源...");
    
    if (db) {
        sqlite3_close(db);
        db = nullptr;
        spdlog::info("数据库连接已关闭");
    }
    
    spdlog::info("资源清理完成");
    
    // 最后清理日志系统
    log_util::shutdown();
}

// ==== 主程序入口 ====
int main() {
    int exit_code = 0;
    
    try {
        // 设置控制台输出代码页为 UTF-8
        SetConsoleOutputCP(CP_UTF8);

        // 初始化日志器
        log_util::init("app_logger", "logs/app.log", spdlog::level::info, spdlog::level::info);
        spdlog::info("=== 应用程序启动 ===");

        // 初始化数据库
        if (!initialize_database()) {
            spdlog::error("数据库初始化失败，程序退出");
            exit_code = 1;
            goto cleanup_and_exit;
        }
        
        // 启动服务器
        if (!start_server()) {
            spdlog::error("服务器启动失败，程序退出");
            exit_code = 1;
            goto cleanup_and_exit;
        }
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "main");
        spdlog::error("主程序发生未捕获异常，程序退出");
        exit_code = 1;
    } catch (...) {
        spdlog::error("主程序发生未知异常，程序退出");
        exit_code = 1;
    }
    
cleanup_and_exit:
    cleanup();
    return exit_code;
}



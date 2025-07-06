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
#include "config/ConfigInitializer.h"
#include "config/ConfigManager.h"

using namespace httplib;
using namespace dao_util;
using json = nlohmann::json;

sqlite3* db = nullptr;

// RAII资源管理类
class ApplicationResources {
private:
    bool db_initialized = false;
    
public:
    ApplicationResources() = default;
    
    ~ApplicationResources() {
        cleanup();
    }
    
    // 禁用拷贝构造和赋值
    ApplicationResources(const ApplicationResources&) = delete;
    ApplicationResources& operator=(const ApplicationResources&) = delete;
    
    // 初始化数据库
    bool initializeDatabase() {
        spdlog::info("正在初始化数据库连接...");
        
        ConfigManager& config = ConfigManager::getInstance();
        std::string dbPath = config.getString("database.path", "db/work_record.db");
        
        db = db_util::openDB(dbPath.c_str());
        if (!db) {
            spdlog::error("数据库连接失败: {}", dbPath);
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
        
        db_initialized = true;
        spdlog::info("数据库连接成功: {}", dbPath);
        return true;
    }
    
    // 清理资源
    void cleanup() {
        spdlog::info("正在清理资源...");
        
        if (db && db_initialized) {
            sqlite3_close(db);
            db = nullptr;
            db_initialized = false;
            spdlog::info("数据库连接已关闭");
        }
        
        spdlog::info("资源清理完成");
        
        // 最后清理日志系统
        log_util::shutdown();
    }
};

// 安全的服务启动
bool start_server() {
    try {
        Server svr;
        
        ConfigManager& config = ConfigManager::getInstance();
        
        // 设置静态文件目录
        std::string staticDir = config.getString("server.static_dir", "./static");
        svr.set_base_dir(staticDir.c_str());
        
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
        
        std::string host = config.getString("server.host", "0.0.0.0");
        int port = config.getInt("server.port", 8080);
        
        std::cout << "🚀 服务已启动：http://" << host << ":" << port << "\n";
        spdlog::info("HTTP服务器启动成功，监听地址: {}:{}", host, port);
        
        svr.listen(host.c_str(), port);
        return true;
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "start_server");
        return false;
    }
}

// ==== 主程序入口 ====
int main() {
    try {
        // 设置控制台输出代码页为 UTF-8
        SetConsoleOutputCP(CP_UTF8);

        // 初始化配置系统
        spdlog::info("开始初始化配置系统...");
        if (!config::initialize("config", "development")) {
            std::cerr << "配置系统初始化失败" << std::endl;
            spdlog::error("配置系统初始化失败，程序退出");
            return 1;
        }
        spdlog::info("配置系统初始化完成");
        
        ConfigManager& config = ConfigManager::getInstance();
        
        // 初始化日志器
        std::string logFile = config.getString("logging.file", "logs/app.log");
        std::string logLevel = config.getString("logging.level", "info");
        
        // 转换日志级别
        spdlog::level::level_enum level = spdlog::level::info;
        if (logLevel == "debug") level = spdlog::level::debug;
        else if (logLevel == "warn") level = spdlog::level::warn;
        else if (logLevel == "error") level = spdlog::level::err;
        
        log_util::init("app_logger", logFile, level, level);
        spdlog::info("=== 应用程序启动 ===");
        spdlog::info("环境: {}", config::getEnvironment());
        spdlog::info("配置目录: {}", config::getConfigDir());
        spdlog::info("日志系统初始化完成");

        // 初始化数据库
        spdlog::info("开始初始化数据库...");
        ApplicationResources resources;
        if (!resources.initializeDatabase()) {
            spdlog::error("数据库初始化失败，程序退出");
            return 1;
        }
        
        // 启动服务器
        spdlog::info("开始启动服务器...");
        if (!start_server()) {
            spdlog::error("服务器启动失败，程序退出");
            return 1;
        }
        
        // 正常退出
        return 0;
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "main");
        spdlog::error("主程序发生未捕获异常，程序退出");
        return 1;
    } catch (...) {
        spdlog::error("主程序发生未知异常，程序退出");
        return 1;
    }
}



#include "service/department_service.h"
#include "dao/department_dict_dao.h"
#include "model/department_dict_model.h"
#include "util/dao_util.h"
#include "util/log_util.h"
#include "util/response_util.h"
#include <nlohmann/json.hpp>
#include <sqlite3/sqlite3.h>

using namespace httplib;
using namespace dao_util;
using namespace response_util;
using json = nlohmann::json;

extern sqlite3* db;

// 获取所有部门
void get_all_departments(const Request& req, Response& res) {
    try {
        std::vector<DepartmentDict> departments;
        auto result = queryAllDepartmentDict(db, departments);
        
        if (result == DaoResult::SUCCESS) {
            json j = json::array();
            for (const auto& dept : departments) {
                j.push_back(json{
                    {"id", dept.id},
                    {"name", dept.name},
                    {"description", dept.description}
                });
            }
            send_data_direct(res, j);
            spdlog::info("获取所有部门成功，数量: {}", departments.size());
        } else {
            spdlog::error("获取所有部门失败");
            send_operation_failed(res, "获取", "部门列表");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_all_departments");
        send_internal_error(res);
    }
}

// 新增部门
void add_department(const Request& req, Response& res) {
    try {
        DepartmentDict dept;
        dept.name = req.get_file_value("name").content;
        dept.description = req.get_file_value("description").content;
        
        auto result = insertDepartmentDict(db, dept);
        
        if (result == DaoResult::SUCCESS) {
            send_success(res, json{{"id", dept.id}});
            spdlog::info("新增部门成功，ID: {}, 名称: {}", dept.id, dept.name);
        } else {
            spdlog::error("新增部门失败，名称: {}", dept.name);
            send_operation_failed(res, "插入", "部门");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "add_department");
        send_internal_error(res);
    }
}

// 更新部门
void update_department(const Request& req, Response& res) {
    try {
        DepartmentDict dept;
        dept.id = std::stoi(req.get_file_value("id").content);
        dept.name = req.get_file_value("name").content;
        dept.description = req.get_file_value("description").content;
        
        auto result = updateDepartmentDict(db, dept);
        
        if (result == DaoResult::SUCCESS) {
            send_success(res);
            spdlog::info("更新部门成功，ID: {}, 名称: {}", dept.id, dept.name);
        } else {
            spdlog::error("更新部门失败，ID: {}, 名称: {}", dept.id, dept.name);
            send_operation_failed(res, "更新", "部门");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "update_department");
        send_internal_error(res);
    }
}

// 删除部门
void delete_department(const Request& req, Response& res) {
    try {
        int64_t id = std::stoll(req.get_param_value("id"));
        
        auto result = deleteDepartmentDict(db, id);
        
        if (result == DaoResult::SUCCESS) {
            send_success(res);
            spdlog::info("删除部门成功，ID: {}", id);
        } else {
            spdlog::error("删除部门失败，ID: {}", id);
            send_operation_failed(res, "删除", "部门");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "delete_department");
        send_internal_error(res);
    }
}
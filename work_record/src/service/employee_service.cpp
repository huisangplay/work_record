#include "service/employee_service.h"
#include "dao/employee_dict_dao.h"
#include "model/employee_dict_model.h"
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

// 获取所有员工
void get_all_employees(const Request& req, Response& res) {
    try {
        std::vector<EmployeeDict> employees;
        auto result = queryAllEmployeeDict(db, employees);
        
        if (result == DaoResult::SUCCESS) {
            json j = json::array();
            for (const auto& emp : employees) {
                j.push_back(json{
                    {"id", emp.id},
                    {"name", emp.name},
                    {"employee_number", emp.employee_number},
                    {"department_name", emp.department_name}
                });
            }
            send_data_direct(res, j);
            spdlog::info("获取所有员工成功，数量: {}", employees.size());
        } else {
            spdlog::error("获取所有员工失败");
            send_operation_failed(res, "获取", "员工列表");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_all_employees");
        send_internal_error(res);
    }
}

// 新增员工
void add_employee(const Request& req, Response& res) {
    try {
        EmployeeDict emp;
        emp.name = req.get_file_value("name").content;
        emp.employee_number = req.get_file_value("employee_number").content;
        emp.department_name = req.get_file_value("department_name").content;
        
        auto result = insertEmployeeDict(db, emp);
        
        if (result == DaoResult::SUCCESS) {
            send_success(res, json{{"id", emp.id}});
            spdlog::info("新增员工成功，ID: {}, 姓名: {}", emp.id, emp.name);
        } else {
            spdlog::error("新增员工失败，姓名: {}", emp.name);
            send_operation_failed(res, "插入", "员工");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "add_employee");
        send_internal_error(res);
    }
}

// 更新员工
void update_employee(const Request& req, Response& res) {
    try {
        EmployeeDict emp;
        emp.id = std::stoi(req.get_file_value("id").content);
        emp.name = req.get_file_value("name").content;
        emp.employee_number = req.get_file_value("employee_number").content;
        emp.department_name = req.get_file_value("department_name").content;
        
        auto result = updateEmployeeDict(db, emp);
        
        if (result == DaoResult::SUCCESS) {
            send_success(res);
            spdlog::info("更新员工成功，ID: {}, 姓名: {}", emp.id, emp.name);
        } else {
            spdlog::error("更新员工失败，ID: {}, 姓名: {}", emp.id, emp.name);
            send_operation_failed(res, "更新", "员工");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "update_employee");
        send_internal_error(res);
    }
}

// 删除员工
void delete_employee(const Request& req, Response& res) {
    try {
        int id = std::stoi(req.get_param_value("id"));
        
        auto result = deleteEmployeeDict(db, id);
        
        if (result == DaoResult::SUCCESS) {
            send_success(res);
            spdlog::info("删除员工成功，ID: {}", id);
        } else {
            spdlog::error("删除员工失败，ID: {}", id);
            send_operation_failed(res, "删除", "员工");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "delete_employee");
        send_internal_error(res);
    }
} 
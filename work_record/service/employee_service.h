#pragma once
#include "../dao/employee_dict_dao.h"
#include "../model/employee_dict_model.h"
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"
#include "../db/sqlite3.h"
extern sqlite3* db;
using namespace httplib;
using json = nlohmann::json;

// 获取所有人员，支持 department_id 过滤
inline void get_all_employees(const Request& req, Response& res) {
    try {
        std::vector<EmployeeDict> employees;
        if (req.has_param("department_id")) {
            int deptId = std::stoi(req.get_param_value("department_id"));
            employees = queryEmployeeByDepartment(db, deptId);
        } else {
            employees = queryAllEmployeeDict(db);
        }
        json j = json::array();
        for (const auto& emp : employees) {
            j.push_back(json{
                {"id", emp.id},
                {"name", emp.name},
                {"employee_number", emp.employee_number},
                {"department_name", emp.department_name}
            });
        }
        res.set_content(j.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 新增人员
inline void add_employee(const Request& req, Response& res) {
    try {
        EmployeeDict emp;
        emp.name = req.get_file_value("name").content;
        emp.employee_number = req.get_file_value("employee_number").content;
        emp.department_name = req.get_file_value("department_name").content;
        
        if (!insertEmployeeDict(db, emp)) {
            throw std::runtime_error("人员插入失败");
        }
        
        res.set_content(json{{"id", emp.id}, {"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 更新人员
inline void update_employee(const Request& req, Response& res) {
    try {
        EmployeeDict emp;
        emp.id = std::stoi(req.get_file_value("id").content);
        emp.name = req.get_file_value("name").content;
        emp.employee_number = req.get_file_value("employee_number").content;
        emp.department_name = req.get_file_value("department_name").content;
        
        if (!updateEmployeeDict(db, emp)) {
            throw std::runtime_error("人员更新失败");
        }
        
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 删除人员
inline void delete_employee(const Request& req, Response& res) {
    try {
        int id = std::stoi(req.get_param_value("id"));
        
        if (!deleteEmployeeDict(db, id)) {
            throw std::runtime_error("人员删除失败");
        }
        
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
} 
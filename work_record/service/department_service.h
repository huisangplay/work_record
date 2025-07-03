#pragma once
#include "../dao/department_dict_dao.h"
#include "../model/department_dict_model.h"
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"
#include "../db/sqlite3.h"
using namespace httplib;
using json = nlohmann::json;
extern sqlite3 * db;
// 获取所有部门
inline void get_all_departments(const Request& req, Response& res) {
    try {
        auto departments = queryAllDepartmentDict(db);
        json j = json::array();
        for (const auto& dept : departments) {
            j.push_back(json{
                {"id", dept.id},
                {"name", dept.name},
                {"description", dept.description}
            });
        }
        res.set_content(j.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 新增部门
inline void add_department(const Request& req, Response& res) {
    try {
        DepartmentDict dept;
        dept.name = req.get_file_value("name").content;
        dept.description = req.get_file_value("description").content;
        
        if (!insertDepartmentDict(db, dept)) {
            throw std::runtime_error("部门插入失败");
        }
        
        res.set_content(json{{"id", dept.id}, {"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 更新部门
inline void update_department(const Request& req, Response& res) {
    try {
        DepartmentDict dept;
        dept.id = std::stoi(req.get_file_value("id").content);
        dept.name = req.get_file_value("name").content;
        dept.description = req.get_file_value("description").content;
        
        if (!updateDepartmentDict(db, dept)) {
            throw std::runtime_error("部门更新失败");
        }
        
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 删除部门
inline void delete_department(const Request& req, Response& res) {
    try {
        int id = std::stoi(req.get_param_value("id"));
        
        if (!deleteDepartmentDict(db, id)) {
            throw std::runtime_error("部门删除失败");
        }
        
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
} 
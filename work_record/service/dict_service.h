#pragma once
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"
#include "../db/db_util.h"
#include "../dao/work_type_dict_dao.h"
#include "../dao/affected_type_dict_dao.h"
#include "../dao/source_type_dict_dao.h"
#include "../dao/work_record_status_dict_dao.h"
#include "../dao/requirement_status_dict_dao.h"
#include "../dao/issue_progress_dict_dao.h"
#include "../dao/department_dict_dao.h"
#include "../dao/employee_dict_dao.h"
#include "../service/requirement_service.h"
extern sqlite3 * db;
// 获取所有字典表通用接口重构
inline void get_dict(const Request& req, Response& res) {
    try {
        std::string table = req.get_param_value("table");
        if (table == "work_type_dict") {
            auto list = queryAllWorkTypeDict(db);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : list) {
                arr.push_back(json{{"id", item.id}, {"type", item.type}, {"comment", item.comment}});
            }
            res.set_content(arr.dump(), "application/json");
        } else if (table == "affected_type_dict") {
            auto list = queryAllAffectedTypeDict(db);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : list) {
                arr.push_back(json{{"id", item.id}, {"affected", item.affected}, {"comment", item.comment}});
            }
            res.set_content(arr.dump(), "application/json");
        } else if (table == "source_type_dict") {
            auto list = queryAllSourceTypeDict(db);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : list) {
                arr.push_back(json{{"id", item.id}, {"type", item.type}, {"comment", item.comment}});
            }
            res.set_content(arr.dump(), "application/json");
        } else if (table == "work_record_status_dict") {
            auto list = queryAllWorkRecordStatusDict(db);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : list) {
                arr.push_back(json{{"id", item.id}, {"status_name", item.status_name}, {"status_class", item.status_class}});
            }
            res.set_content(arr.dump(), "application/json");
        } else if (table == "requirement_status_dict") {
            auto list = queryAllRequirementStatusDict(db);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : list) {
                arr.push_back(json{{"id", item.id}, {"status", item.status}, {"comment", item.comment}, {"requirement_status_class", item.requirement_status_class}});
            }
            res.set_content(arr.dump(), "application/json");
        } else if (table == "issue_progress_dict") {
            auto list = queryAllIssueProgressDict(db);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : list) {
                arr.push_back(json{{"id", item.id}, {"progress", item.progress}, {"progress_class", item.progress_class}, {"comment", item.comment}});
            }
            res.set_content(arr.dump(), "application/json");
        } else if (table == "department_dict") {
            auto list = queryAllDepartmentDict(db);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : list) {
                arr.push_back(json{{"id", item.id}, {"name", item.name}, {"description", item.description}});
            }
            res.set_content(arr.dump(), "application/json");
        } else if (table == "employee_dict") {
            auto list = queryAllEmployeeDict(db);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : list) {
                arr.push_back(json{{"id", item.id}, {"name", item.name}, {"employee_number", item.employee_number}, {"department_name", item.department_name}});
            }
            res.set_content(arr.dump(), "application/json");
        } else {
            res.status = 400;
            res.set_content(json{{"error", "invalid dict"}}.dump(), "application/json");
        }
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 新增字典项
inline void add_dict(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        std::string table = j["table"];

        if (table == "work_type_dict") {
            WorkTypeDict item;
            item.type = j["name"];
            item.comment = j.value("comment", "");
            if (insertWorkTypeDict(db, item)) {
                res.set_content(json{{"success", true}, {"id", item.id}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "插入失败"}}.dump(), "application/json");
            }
        } else if (table == "affected_type_dict") {
            AffectedTypeDict item;
            item.affected = j["name"];
            item.comment = j.value("comment", "");
            if (insertAffectedTypeDict(db, item)) {
                res.set_content(json{{"success", true}, {"id", item.id}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "插入失败"}}.dump(), "application/json");
            }
        } else if (table == "source_type_dict") {
            SourceTypeDict item;
            item.type = j["name"];
            item.comment = j.value("comment", "");
            if (insertSourceTypeDict(db, item)) {
                res.set_content(json{{"success", true}, {"id", item.id}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "插入失败"}}.dump(), "application/json");
            }
        } else if (table == "work_record_status_dict") {
            WorkRecordStatusDict item;
            item.status_name = j["name"];
            item.status_class = j.value("status_class", "bg-primary");
            if (insertWorkRecordStatusDict(db, item)) {
                res.set_content(json{{"success", true}, {"id", item.id}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "插入失败"}}.dump(), "application/json");
            }
        } else if (table == "requirement_status_dict") {
            RequirementStatusDict item;
            item.status = j["name"];
            item.requirement_status_class = j.value("status_class", "bg-primary");
            item.comment = j.value("comment", "");
            if (insertRequirementStatusDict(db, item)) {
                res.set_content(json{{"success", true}, {"id", item.id}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "插入失败"}}.dump(), "application/json");
            }
        } else if (table == "issue_progress_dict") {
            IssueProgressDict item;
            item.progress = j["name"];
            item.progress_class = j.value("progress_class", "bg-primary");
            item.comment = j.value("comment", "");
            if (insertIssueProgressDict(db, item)) {
                res.set_content(json{{"success", true}, {"id", item.id}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "插入失败"}}.dump(), "application/json");
            }
        } else if (table == "department_dict") {
            DepartmentDict item;
            item.name = j["name"];
            item.description = j.value("comment", "");
            if (insertDepartmentDict(db, item)) {
                res.set_content(json{{"success", true}, {"id", item.id}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "插入失败"}}.dump(), "application/json");
            }
        } else if (table == "employee_dict") {
            EmployeeDict item;
            item.name = j["name"];
            item.employee_number = j.value("employee_number", "");
            item.department_name = j.value("department_name", "");
            if (insertEmployeeDict(db, item)) {
                res.set_content(json{{"success", true}, {"id", item.id}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "插入失败"}}.dump(), "application/json");
            }
        } else {
            res.status = 400;
            res.set_content(json{{"error", "invalid dict"}}.dump(), "application/json");
        }
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 更新字典项
inline void update_dict(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        std::string table = j["table"];
        int id = j["id"];

        if (table == "work_type_dict") {
            WorkTypeDict item;
            item.id = id;
            item.type = j["name"];
            item.comment = j.value("comment", "");
            if (updateWorkTypeDict(db, item)) {
                res.set_content(json{{"success", true}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "更新失败"}}.dump(), "application/json");
            }
        } else if (table == "affected_type_dict") {
            AffectedTypeDict item;
            item.id = id;
            item.affected = j["name"];
            item.comment = j.value("comment", "");
            if (updateAffectedTypeDict(db, item)) {
                res.set_content(json{{"success", true}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "更新失败"}}.dump(), "application/json");
            }
        } else if (table == "source_type_dict") {
            SourceTypeDict item;
            item.id = id;
            item.type = j["name"];
            item.comment = j.value("comment", "");
            if (updateSourceTypeDict(db, item)) {
                res.set_content(json{{"success", true}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "更新失败"}}.dump(), "application/json");
            }
        } else if (table == "work_record_status_dict") {
            WorkRecordStatusDict item;
            item.id = id;
            item.status_name = j["name"];
            item.status_class = j.value("status_class", "bg-primary");
            if (updateWorkRecordStatusDict(db, item)) {
                res.set_content(json{{"success", true}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "更新失败"}}.dump(), "application/json");
            }
        } else if (table == "requirement_status_dict") {
            RequirementStatusDict item;
            item.id = id;
            item.status = j["name"];
            item.requirement_status_class = j.value("status_class", "bg-primary");
            item.comment = j.value("comment", "");
            if (updateRequirementStatusDict(db, item)) {
                res.set_content(json{{"success", true}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "更新失败"}}.dump(), "application/json");
            }
        } else if (table == "issue_progress_dict") {
            IssueProgressDict item;
            item.id = id;
            item.progress = j["name"];
            item.progress_class = j.value("progress_class", "bg-primary");
            item.comment = j.value("comment", "");
            if (updateIssueProgressDict(db, item)) {
                res.set_content(json{{"success", true}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "更新失败"}}.dump(), "application/json");
            }
        } else if (table == "department_dict") {
            DepartmentDict item;
            item.id = id;
            item.name = j["name"];
            item.description = j.value("comment", "");
            if (updateDepartmentDict(db, item)) {
                res.set_content(json{{"success", true}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "更新失败"}}.dump(), "application/json");
            }
        } else if (table == "employee_dict") {
            EmployeeDict item;
            item.id = id;
            item.name = j["name"];
            item.employee_number = j.value("employee_number", "");
            item.department_name = j.value("department_name", "");
            if (updateEmployeeDict(db, item)) {
                res.set_content(json{{"success", true}}.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(json{{"error", "更新失败"}}.dump(), "application/json");
            }
        } else {
            res.status = 400;
            res.set_content(json{{"error", "invalid dict"}}.dump(), "application/json");
        }
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 删除字典项
inline void delete_dict(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        std::string table = j["table"];
        int id = j["id"];

        bool success = false;
        if (table == "work_type_dict") {
            success = deleteWorkTypeDict(db, id);
        } else if (table == "affected_type_dict") {
            success = deleteAffectedTypeDict(db, id);
        } else if (table == "source_type_dict") {
            success = deleteSourceTypeDict(db, id);
        } else if (table == "work_record_status_dict") {
            success = deleteWorkRecordStatusDict(db, id);
        } else if (table == "requirement_status_dict") {
            success = deleteRequirementStatusDict(db, id);
        } else if (table == "issue_progress_dict") {
            success = deleteIssueProgressDict(db, id);
        } else if (table == "department_dict") {
            success = deleteDepartmentDict(db, id);
        } else if (table == "employee_dict") {
            success = deleteEmployeeDict(db, id);
        } else {
            res.status = 400;
            res.set_content(json{{"error", "invalid dict"}}.dump(), "application/json");
            return;
        }

        if (success) {
            res.set_content(json{{"success", true}}.dump(), "application/json");
        } else {
            res.status = 500;
            res.set_content(json{{"error", "删除失败"}}.dump(), "application/json");
        }
    } catch (const std::exception& e) {
        res.status = 400;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}
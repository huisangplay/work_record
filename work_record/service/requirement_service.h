#pragma once
#include "../dao/requirement_record_dao.h"
#include "../dao/affected_type_dict_dao.h"
#include "../model/requirement_record_model.h"
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"
#include "../db/sqlite3.h"
#include "../db/db_util.h"
#include <vector>

using namespace httplib;
using json = nlohmann::json;
extern sqlite3 * db;
// 错误码
enum RequirementProgressErrorCode {
    REQUIREMENT_PROGRESS_NO_ERROR = 0,
    REQUIREMENT_PROGRESS_HAS_WORK_RECORD = 1
};

// 新增需求进展接口
inline void add_requirement_record(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        RequirementRecord record;
        record.title = j.value("title", "");
        // 强制类型转换，确保为 int 类型
        record.requirement_status_id = j.contains("requirement_status_id") ? std::stoi(j["requirement_status_id"].get<std::string>()) : 0;
        record.source_type_id = j.contains("source_type_id") ? std::stoi(j["source_type_id"].get<std::string>()) : 0;
        record.affected_id = j.contains("affected_id") ? std::stoi(j["affected_id"].get<std::string>()) : 0;
        record.employee_id = j.contains("employee_id") ? std::stoi(j["employee_id"].get<std::string>()) : 0;
        int newId = 0;
        insertRequirementRecord(db, record, newId);
        res.set_content(json{{"id", newId}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 修改需求进展接口
inline void update_requirement_record(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        RequirementRecord record;
        record.id = j.contains("id") ? std::stoi(j["id"].get<std::string>()) : 0;
        record.title = j.value("title", "");
        record.requirement_status_id = j.contains("requirement_status_id") ? std::stoi(j["requirement_status_id"].get<std::string>()) : 0;
        record.source_type_id = j.contains("source_type_id") ? std::stoi(j["source_type_id"].get<std::string>()) : 0;
        record.affected_id = j.contains("affected_id") ? std::stoi(j["affected_id"].get<std::string>()) : 0;
        record.employee_id = j.contains("employee_id") ? std::stoi(j["employee_id"].get<std::string>()) : 0;
        updateRequirementRecord(db, record);
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 删除需求进展接口
inline void delete_requirement_record(const Request& req, Response& res) {
    try {
        if (!req.has_param("id")) throw std::runtime_error("缺少 id 参数");
        int id = std::stoi(req.get_param_value("id"));
        int workCount = countWorkRecordByRequirement(db, id);
        if (workCount > 0) {
            res.status = 400;
            res.set_content(json{{"error", "该需求下存在工作记录，无法删除"}, {"code", REQUIREMENT_PROGRESS_HAS_WORK_RECORD}}.dump(), "application/json");
            return;
        }
        if (!deleteRequirementRecord(db, id)) {
            throw std::runtime_error("需求进展删除失败");
        }
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 获取所有需求接口重构
inline void get_all_requirement_records(const Request& req, Response& res) {
    try {

            // 使用分页查询
            int page = 1, page_size = 20, total = 0;
            try { page = std::stoi(req.get_param_value("page")); } catch (...) {}
            try { page_size = std::stoi(req.get_param_value("page_size")); } catch (...) {}
            if (page < 1) page = 1;
            if (page_size != 10 && page_size != 20 && page_size != 50) page_size = 20;
            
            std::string status_id = req.has_param("requirement_status_id") ? req.get_param_value("requirement_status_id") : "";
            std::string affected_id = req.has_param("affected_id") ? req.get_param_value("affected_id") : "";
            std::string source_type_id = req.has_param("source_type_id") ? req.get_param_value("source_type_id") : "";
            
            auto records = queryRequirementRecordsPagedDao(db, page, page_size, total, status_id, affected_id, source_type_id);
            json j = json::array();
            for (const auto& r : records) {
                j.push_back(json{
                    {"id", r.id},
                    {"title", r.title},
                    {"requirement_status_id", r.requirement_status_id},
                    {"requirement_status_name", r.requirement_status_name},
                    {"requirement_status_class", r.requirement_status_class},
                    {"source_type_id", r.source_type_id},
                    {"source_type_name", r.source_type_name},
                    {"create_time", r.create_time},
                    {"update_time", r.update_time},
                    {"affected_id", r.affected_id},
                    {"affected_name", r.affected_name},
                    {"employee_name",r.employee_name},
                    {"department_id",r.department_id},
                    {"employee_id",r.employee_id}
                });
            }
            json resp = {
                {"total", total},
                {"page", page},
                {"page_size", page_size},
                {"records", j}
            };
            res.set_content(resp.dump(), "application/json");

    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}


// 需求进展新增/修改分发接口
inline void add_or_update_requirement_record(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        int id = 0;
        if (j.contains("id")) {
            if (j["id"].is_number_integer()) id = j["id"].get<int>();
            else if (j["id"].is_string() && !j["id"].get<std::string>().empty()) id = std::stoi(j["id"].get<std::string>());
        }
        if (id > 0) {
            update_requirement_record(req, res);
        } else {
            add_requirement_record(req, res);
        }
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 新建需求记录
inline void create_requirement(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        RequirementRecord record;
        record.title = j.value("title", "");
        record.requirement_status_id = j.value("requirement_status_id", 0);
        record.source_type_id = j.value("source_type_id", 0);
        record.affected_id = j.value("affected_id", 0);
        record.employee_id = j.value("employee_id", 0);
        int newId = 0;
        insertRequirementRecord(db, record, newId);
        res.set_content(json{{"id", newId}, {"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 更新需求记录
inline void update_requirement(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        RequirementRecord record;
        record.id = j.value("id", 0);
        record.title = j.value("title", "");
        record.requirement_status_id = j.value("requirement_status_id", 0);
        record.source_type_id = j.value("source_type_id", 0);
        record.affected_id = j.value("affected_id", 0);
        record.employee_id = j.value("employee_id", 0);
        updateRequirementRecord(db, record);
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

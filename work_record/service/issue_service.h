#pragma once
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"
#include "../db/db_util.h"
#include "../dao/issue_record_dao.h"
#include "../service/requirement_service.h"

// 获取所有问题接口重构（字段补全、结构规范、异常健壮）
inline void get_all_issues(const Request& req, Response& res) {
    try {
        // 检查是否有分页参数
        bool hasPaging = req.has_param("page") || req.has_param("page_size");

            // 使用分页查询
            int page = 1, page_size = 20, total = 0;
            try { page = std::stoi(req.get_param_value("page")); } catch (...) {}
            try { page_size = std::stoi(req.get_param_value("page_size")); } catch (...) {}
            if (page < 1) page = 1;
            if (page_size != 10 && page_size != 20 && page_size != 50) page_size = 20;

            std::string source_type_id = req.has_param("source_type_id") ? req.get_param_value("source_type_id") : "";
            std::string affected_id = req.has_param("affected_id") ? req.get_param_value("affected_id") : "";

            auto records = queryIssuesPagedDao(db, page, page_size, total, source_type_id, affected_id);
            json j = json::array();
            for (const auto& r : records) {
                j.push_back(json{
                    {"id", r.id},
                    {"issue_title", r.issue_title},
                    {"description", r.description},
                    {"progress_id", r.progress_id},
                    {"progress_name", r.progress_name},
                    {"progress_class", r.progress_class},
                    {"responsible_person", r.responsible_person},
                    {"create_time", r.create_time},
                    {"update_time", r.update_time},
                    {"reported_by", r.reported_by},
                    {"source_type_id", r.source_type_id},
                    {"source_type_name", r.source_type_name},
                    {"affected_id", r.affected_id},
                    {"affected_name", r.affected_name},
                    {"employee_id", r.employee_id},
                    {"employee_name", r.employee_name},
                    {"department_id", r.department_id}
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
// 新增问题接口
inline void add_issue(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        IssueRecord r;
        r.issue_title = j.value("issue_title", "");
        r.employee_id = j.contains("employee_id") ? std::stoi(j["employee_id"].get<std::string>()) : 0;
        r.description = j.value("description", "");
        r.progress_id = j.contains("progress_id") ? std::stoi(j["progress_id"].get<std::string>()) : 0;
        r.responsible_person = j.value("responsible_person", "");
        r.affected_id = j.contains("affected_id") ? std::stoi(j["affected_id"].get<std::string>()) : 0;
        r.source_type_id = j.contains("source_type_id") ? std::stoi(j["source_type_id"].get<std::string>()) : 0;
        r.reported_by = j.value("reported_by", "");
        int newId = 0;
        insertIssueDao(db, r, newId); // 失败会直接throw异常，带详细错误
        res.set_content(json{{"id", newId}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 修改问题接口
inline void update_issue(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        IssueRecord r;
        r.id = j.contains("id") ? std::stoi(j["id"].get<std::string>()) : 0;
        r.issue_title = j.value("issue_title", "");
        r.employee_id = j.contains("employee_id") ? std::stoi(j["employee_id"].get<std::string>()) : 0;
        r.description = j.value("description", "");
        r.progress_id = j.contains("progress_id") ? std::stoi(j["progress_id"].get<std::string>()) : 0;
        r.responsible_person = j.value("responsible_person", "");
        r.affected_id = j.contains("affected_id") ? std::stoi(j["affected_id"].get<std::string>()) : 0;
        r.source_type_id = j.contains("source_type_id") ? std::stoi(j["source_type_id"].get<std::string>()) : 0;
        r.reported_by = j.value("reported_by", "");
        updateIssueDao(db, r);
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 删除问题接口
inline void delete_issue(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        int id = j.contains("id") ? j["id"].get<int>() : 0;
        if (!deleteIssueDao(db, id)) throw std::runtime_error("删除失败");
        res.set_content(json{{"success", true}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}
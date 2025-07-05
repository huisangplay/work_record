#include "service/issue_service.h"
#include "dao/issue_record_dao.h"
#include "model/issue_record_model.h"
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

// 获取所有问题记录
void get_all_issues(const Request& req, Response& res) {
    try {
        std::vector<IssueRecord> records;
        auto result = queryAllIssues(db, records);
        
        if (result == DaoResult::SUCCESS) {
            json j = json::array();
            for (const auto& record : records) {
                j.push_back(json{
                    {"id", record.id},
                    {"issue_title", record.issue_title},
                    {"reported_by", record.reported_by},
                    {"description", record.description},
                    {"progress_id", record.progress_id},
                    {"progress_name", record.progress_name},
                    {"progress_class", record.progress_class},
                    {"responsible_person", record.responsible_person},
                    {"affected_id", record.affected_id},
                    {"affected_name", record.affected_name},
                    {"source_type_id", record.source_type_id},
                    {"source_type_name", record.source_type_name},
                    {"create_time", record.create_time},
                    {"update_time", record.update_time},
                    {"employee_id", record.employee_id},
                    {"employee_name", record.employee_name},
                    {"department_id", record.department_id}
                });
            }
            send_data_direct(res, j);
            spdlog::info("获取所有问题记录成功，数量: {}", records.size());
        } else {
            spdlog::error("获取所有问题记录失败");
            send_operation_failed(res, "获取", "问题列表");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_all_issues");
        send_internal_error(res);
    }
}

// 新增问题记录
void add_issue(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        IssueRecord record;
        
        // 安全地处理字符串字段，处理 null 值
        record.issue_title = j.value("issue_title", "");
        
        // 安全地转换数字字段，处理字符串形式的数字
        if (j.contains("employee_id") && !j["employee_id"].is_null()) {
            if (j["employee_id"].is_string()) {
                record.employee_id = std::stoll(j["employee_id"].get<std::string>());
            } else {
        record.employee_id = j["employee_id"];
            }
        } else {
            record.employee_id = 0;
        }
        
        record.description = j.value("description", "");
        
        if (j.contains("progress_id") && !j["progress_id"].is_null()) {
            if (j["progress_id"].is_string()) {
                record.progress_id = std::stoll(j["progress_id"].get<std::string>());
            } else {
        record.progress_id = j["progress_id"];
            }
        } else {
            record.progress_id = 0;
        }
        
        record.responsible_person = j.value("responsible_person", "");
        
        if (j.contains("affected_id") && !j["affected_id"].is_null()) {
            if (j["affected_id"].is_string()) {
                record.affected_id = std::stoll(j["affected_id"].get<std::string>());
            } else {
        record.affected_id = j["affected_id"];
            }
        } else {
            record.affected_id = 0;
        }
        
        if (j.contains("source_type_id") && !j["source_type_id"].is_null()) {
            if (j["source_type_id"].is_string()) {
                record.source_type_id = std::stoll(j["source_type_id"].get<std::string>());
            } else {
        record.source_type_id = j["source_type_id"];
            }
        } else {
            record.source_type_id = 0;
        }
        
        record.reported_by = j.value("reported_by", "");
        
        int64_t newId;

        if (const auto result = insertIssue(db, record, newId); result == DaoResult::SUCCESS) {
            send_success(res, json{{"id", newId}});
            spdlog::info("新增问题记录成功，ID: {}, 标题: {}", newId, record.issue_title);
        } else {
            spdlog::error("新增问题记录失败，标题: {}", record.issue_title);
            send_operation_failed(res, "插入", "问题");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "add_issue");
        send_internal_error(res);
    }
}

// 更新问题记录
void update_issue(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        IssueRecord record;
        
        // 安全地转换ID字段
        if (j.contains("id") && !j["id"].is_null()) {
            if (j["id"].is_string()) {
                record.id = std::stoll(j["id"].get<std::string>());
            } else {
        record.id = j["id"];
            }
        } else {
            record.id = 0;
        }
        
        // 安全地处理字符串字段，处理 null 值
        record.issue_title = j.value("issue_title", "");
        
        // 安全地转换数字字段，处理字符串形式的数字
        if (j.contains("employee_id") && !j["employee_id"].is_null()) {
            if (j["employee_id"].is_string()) {
                record.employee_id = std::stoll(j["employee_id"].get<std::string>());
            } else {
        record.employee_id = j["employee_id"];
            }
        } else {
            record.employee_id = 0;
        }
        
        record.description = j.value("description", "");
        
        if (j.contains("progress_id") && !j["progress_id"].is_null()) {
            if (j["progress_id"].is_string()) {
                record.progress_id = std::stoll(j["progress_id"].get<std::string>());
            } else {
        record.progress_id = j["progress_id"];
            }
        } else {
            record.progress_id = 0;
        }
        
        record.responsible_person = j.value("responsible_person", "");
        
        if (j.contains("affected_id") && !j["affected_id"].is_null()) {
            if (j["affected_id"].is_string()) {
                record.affected_id = std::stoll(j["affected_id"].get<std::string>());
            } else {
        record.affected_id = j["affected_id"];
            }
        } else {
            record.affected_id = 0;
        }
        
        if (j.contains("source_type_id") && !j["source_type_id"].is_null()) {
            if (j["source_type_id"].is_string()) {
                record.source_type_id = std::stoll(j["source_type_id"].get<std::string>());
            } else {
        record.source_type_id = j["source_type_id"];
            }
        } else {
            record.source_type_id = 0;
        }
        
        record.reported_by = j.value("reported_by", "");

        if (const auto result = updateIssue(db, record); result == DaoResult::SUCCESS) {
            send_success(res);
            spdlog::info("更新问题记录成功，ID: {}, 标题: {}", record.id, record.issue_title);
        } else {
            spdlog::error("更新问题记录失败，ID: {}, 标题: {}", record.id, record.issue_title);
            send_operation_failed(res, "更新", "问题");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "update_issue");
        send_internal_error(res);
    }
}

// 删除问题记录
void delete_issue(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        int id;
        
        // 安全地转换ID字段
        if (j.contains("id") && !j["id"].is_null()) {
            if (j["id"].is_string()) {
                id = std::stoi(j["id"].get<std::string>());
            } else {
                id = j["id"];
            }
        } else {
            spdlog::error("删除问题记录失败：缺少ID参数");
            send_bad_request(res, "缺少ID参数");
            return;
        }

        if (auto result = deleteIssue(db, id); result == DaoResult::SUCCESS) {
            send_success(res);
            spdlog::info("删除问题记录成功，ID: {}", id);
        } else {
            spdlog::error("删除问题记录失败，ID: {}", id);
            send_operation_failed(res, "删除", "问题");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "delete_issue");
        send_internal_error(res);
    }
}

// 分页查询问题记录
void get_issues_paged(const Request& req, Response& res) {
    try {
        int page = std::stoi(req.get_param_value("page"));
        int page_size = std::stoi(req.get_param_value("page_size"));
        std::string source_type_id = req.get_param_value("source_type_id");
        std::string affected_id = req.get_param_value("affected_id");
        
        int total;
        std::vector<IssueRecord> records;
        auto result = queryIssuesPaged(db, page, page_size, total, source_type_id, affected_id, records);
        
        if (result == DaoResult::SUCCESS) {
            json j = json::array();
            for (const auto& record : records) {
                j.push_back(json{
                    {"id", record.id},
                    {"issue_title", record.issue_title},
                    {"reported_by", record.reported_by},
                    {"description", record.description},
                    {"progress_id", record.progress_id},
                    {"progress_name", record.progress_name},
                    {"progress_class", record.progress_class},
                    {"responsible_person", record.responsible_person},
                    {"affected_id", record.affected_id},
                    {"affected_name", record.affected_name},
                    {"source_type_id", record.source_type_id},
                    {"source_type_name", record.source_type_name},
                    {"create_time", record.create_time},
                    {"update_time", record.update_time},
                    {"employee_id", record.employee_id},
                    {"employee_name", record.employee_name},
                    {"department_id", record.department_id}
                });
            }
            
            json response = json{
                {"records", j},
                {"total", total},
                {"page", page},
                {"page_size", page_size}
            };
            send_success(res, response);
            spdlog::info("分页查询问题记录成功，总数: {}, 当前页: {}, 每页大小: {}", total, page, page_size);
        } else {
            spdlog::error("分页查询问题记录失败");
            send_operation_failed(res, "分页查询", "问题记录");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_issues_paged");
        send_internal_error(res);
    }
} 
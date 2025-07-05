#include "service/requirement_service.h"
#include "dao/requirement_record_dao.h"
#include "model/requirement_record_model.h"
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

// 获取所有需求记录
void get_all_requirements(const Request& req, Response& res) {
    try {
        std::vector<RequirementRecord> records;
        auto result = queryAllRequirementRecords(db, records);
        
        if (result == DaoResult::SUCCESS) {
            json j = json::array();
            for (const auto& record : records) {
                j.push_back(json{
                    {"id", record.id},
                    {"title", record.title},
                    {"requirement_status_id", record.requirement_status_id},
                    {"requirement_status_name", record.requirement_status_name},
                    {"requirement_status_class", record.requirement_status_class},
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
            res.set_content(j.dump(), "application/json");
            spdlog::info("获取所有需求记录成功，数量: {}", records.size());
        } else {
            spdlog::error("获取所有需求记录失败");
            send_operation_failed(res, "获取", "需求列表");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_all_requirements");
        send_internal_error(res);
    }
}

// 根据ID获取需求记录
void get_requirement_by_id(const Request& req, Response& res) {
    try {
        int id = std::stoi(req.get_param_value("id"));
        RequirementRecord record;
        
        auto result = getRequirementById(db, id, record);
        
        if (result == DaoResult::SUCCESS) {
            json j = json{
                {"id", record.id},
                {"title", record.title},
                {"requirement_status_id", record.requirement_status_id},
                {"affected_id", record.affected_id},
                {"affected_name", record.affected_name},
                {"source_type_id", record.source_type_id},
                {"source_type_name", record.source_type_name},
                {"create_time", record.create_time},
                {"update_time", record.update_time}
            };
            send_success(res, j);
            spdlog::info("获取需求记录成功，ID: {}", id);
        } else if (result == DaoResult::NOT_FOUND) {
            spdlog::warn("需求记录不存在，ID: {}", id);
            send_not_found(res, "需求记录不存在");
        } else {
            spdlog::error("获取需求记录失败，ID: {}", id);
            send_operation_failed(res, "获取", "需求记录");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_requirement_by_id");
        send_internal_error(res);
    }
}

// 新增需求记录
void add_requirement(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        RequirementRecord record;
        record.title = j["title"];
        
        // 安全地转换数字字段，处理字符串形式的数字
        if (j["requirement_status_id"].is_string()) {
            record.requirement_status_id = std::stoll(j["requirement_status_id"].get<std::string>());
        } else {
        record.requirement_status_id = j["requirement_status_id"];
        }
        
        if (j["source_type_id"].is_string()) {
            record.source_type_id = std::stoll(j["source_type_id"].get<std::string>());
        } else {
        record.source_type_id = j["source_type_id"];
        }
        
        if (j["affected_id"].is_string()) {
            record.affected_id = std::stoll(j["affected_id"].get<std::string>());
        } else {
        record.affected_id = j["affected_id"];
        }
        
        // 处理 employee_id，可能为空或字符串
        if (j.contains("employee_id") && !j["employee_id"].is_null()) {
            if (j["employee_id"].is_string()) {
                std::string emp_id_str = j["employee_id"].get<std::string>();
                if (!emp_id_str.empty()) {
                    record.employee_id = std::stoll(emp_id_str);
                } else {
                    record.employee_id = 0;
                }
            } else {
                record.employee_id = j["employee_id"];
            }
        } else {
            record.employee_id = 0;
        }
        
        int64_t newId;

        if (auto result = insertRequirementRecord(db, record, newId); result == DaoResult::SUCCESS) {
            send_success(res, json{{"id", newId}});
            spdlog::info("新增需求记录成功，ID: {}, 标题: {}", newId, record.title);
        } else {
            spdlog::error("新增需求记录失败，标题: {}", record.title);
            send_operation_failed(res, "插入", "需求");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "add_requirement");
        send_internal_error(res);
    }
}

void add_or_update_requirement_record(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        int id = 0;
        if (j.contains("id")) {
            if (j["id"].is_number_integer()) id = j["id"].get<int>();
            else if (j["id"].is_string() && !j["id"].get<std::string>().empty()) id = std::stoi(j["id"].get<std::string>());
        }
        if (id > 0) {
            update_requirement(req, res);
        } else {
            add_requirement(req, res);
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "add_or_update_requirement_record");
        send_internal_error(res);
    }
}

// 更新需求记录
void update_requirement(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        RequirementRecord record;
        
        // 安全地转换ID字段
        if (j["id"].is_string()) {
            record.id = std::stoll(j["id"].get<std::string>());
        } else {
        record.id = j["id"];
        }
        
        record.title = j["title"];
        
        // 安全地转换数字字段，处理字符串形式的数字
        if (j["requirement_status_id"].is_string()) {
            record.requirement_status_id = std::stoll(j["requirement_status_id"].get<std::string>());
        } else {
        record.requirement_status_id = j["requirement_status_id"];
        }
        
        if (j["source_type_id"].is_string()) {
            record.source_type_id = std::stoll(j["source_type_id"].get<std::string>());
        } else {
        record.source_type_id = j["source_type_id"];
        }
        
        if (j["affected_id"].is_string()) {
            record.affected_id = std::stoll(j["affected_id"].get<std::string>());
        } else {
        record.affected_id = j["affected_id"];
        }
        
        // 处理 employee_id，可能为空或字符串
        if (j.contains("employee_id") && !j["employee_id"].is_null()) {
            if (j["employee_id"].is_string()) {
                std::string emp_id_str = j["employee_id"].get<std::string>();
                if (!emp_id_str.empty()) {
                    record.employee_id = std::stoll(emp_id_str);
                } else {
                    record.employee_id = 0;
                }
            } else {
                record.employee_id = j["employee_id"];
            }
        } else {
            record.employee_id = 0;
        }
        
        auto result = updateRequirementRecord(db, record);
        
        if (result == DaoResult::SUCCESS) {
            send_success(res);
            spdlog::info("更新需求记录成功，ID: {}, 标题: {}", record.id, record.title);
        } else {
            spdlog::error("更新需求记录失败，ID: {}, 标题: {}", record.id, record.title);
            send_operation_failed(res, "更新", "需求");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "update_requirement");
        send_internal_error(res);
    }
}

// 删除需求记录
void delete_requirement_record(const Request& req, Response& res) {
    try {
        int64_t id = std::stoll(req.get_param_value("id"));
        
        int workCount = 0;
        auto countResult = countWorkRecordByRequirement(db, id, workCount);
        
        if (countResult != DaoResult::SUCCESS) {
            spdlog::error("检查需求下工作记录数量失败，ID: {}", id);
            send_operation_failed(res, "检查", "工作记录");
            return;
        }

        if (workCount > 0) {
            spdlog::info("删除需求被拒绝，ID: {}，存在 {} 个工作记录", id, workCount);
            send_bad_request(res, "该需求下存在工作记录，无法删除");
            return;
        }
        
        auto result = deleteRequirementRecord(db, id);
        
        if (result == DaoResult::SUCCESS) {
            send_success(res);
            spdlog::info("删除需求记录成功，ID: {}", id);
        } else {
            spdlog::error("删除需求记录失败，ID: {}", id);
            send_operation_failed(res, "删除", "需求");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "delete_requirement");
        send_internal_error(res);
    }
}

// 分页查询需求记录
void get_requirements_paged(const Request& req, Response& res) {
    try {
        int page = std::stoi(req.get_param_value("page"));
        int page_size = std::stoi(req.get_param_value("page_size"));
        std::string status_id = req.get_param_value("status_id");
        std::string affected_id = req.get_param_value("affected_id");
        std::string source_type_id = req.get_param_value("source_type_id");
        
        int total;
        std::vector<RequirementRecord> records;
        auto result = queryRequirementRecordsPaged(db, page, page_size, total, status_id, affected_id, source_type_id, records);
        
        if (result == DaoResult::SUCCESS) {
            json j = json::array();
            for (const auto& record : records) {
                j.push_back(json{
                    {"id", record.id},
                    {"title", record.title},
                    {"requirement_status_id", record.requirement_status_id},
                    {"requirement_status_name", record.requirement_status_name},
                    {"requirement_status_class", record.requirement_status_class},
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
                {"data", j},
                {"total", total},
                {"page", page},
                {"page_size", page_size}
            };
            send_success(res, response);
            spdlog::info("分页查询需求记录成功，总数: {}, 当前页: {}, 每页大小: {}", total, page, page_size);
        } else {
            spdlog::error("分页查询需求记录失败");
            send_operation_failed(res, "分页查询", "需求记录");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_requirements_paged");
        send_internal_error(res);
    }
}

// 获取所有需求接口重构
void get_all_requirement_records(const Request& req, Response& res) {
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

        std::vector<RequirementRecord> records;
        auto result = queryRequirementRecordsPaged(db, page, page_size, total, status_id, affected_id, source_type_id, records);
        
        if (result == DaoResult::SUCCESS) {
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
                    {"employee_name", r.employee_name},
                    {"department_id", r.department_id},
                    {"employee_id", r.employee_id}
                });
            }
            json resp = {
                {"total", total},
                {"page", page},
                {"page_size", page_size},
                {"records", j}
            };
            send_success(res, resp);
            spdlog::info("获取所有需求记录成功，总数: {}, 当前页: {}, 每页大小: {}", total, page, page_size);
        } else {
            spdlog::error("获取所有需求记录失败");
            send_operation_failed(res, "获取", "需求记录");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_all_requirement_records");
        send_internal_error(res);
    }
}
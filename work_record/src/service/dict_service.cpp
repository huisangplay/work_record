#include "service/dict_service.h"
#include "util/dao_util.h"
#include "util/log_util.h"
#include "util/response_util.h"
#include <nlohmann/json.hpp>
#include "dao/work_type_dict_dao.h"
#include "dao/affected_type_dict_dao.h"
#include "dao/source_type_dict_dao.h"
#include "dao/work_record_status_dict_dao.h"
#include "dao/requirement_status_dict_dao.h"
#include "dao/issue_progress_dict_dao.h"
#include "dao/department_dict_dao.h"
#include "dao/employee_dict_dao.h"

extern sqlite3* db;
using namespace httplib;
using namespace dao_util;
using namespace response_util;
using json = nlohmann::json;

// 获取所有字典表通用接口重构
void get_dict(const Request& req, Response& res) {
    try {
        std::string table = req.get_param_value("table");
        
        if (table == "work_type_dict") {
            std::vector<WorkTypeDict> list;
            auto result = queryAllWorkTypeDict(db, list);
            if (result == DaoResult::SUCCESS) {
                json arr = json::array();
                for (const auto& item : list) {
                    arr.push_back(json{{"id", item.id}, {"type", item.type}, {"comment", item.comment}});
                }
                send_data_direct(res, arr);
                spdlog::info("获取工作类型字典成功，数量: {}", list.size());
            } else {
                spdlog::error("获取工作类型字典失败");
                send_operation_failed(res, "获取", "工作类型字典");
            }
        } else if (table == "affected_type_dict") {
            std::vector<AffectedTypeDict> list;
            auto result = queryAllAffectedTypeDict(db, list);
            if (result == DaoResult::SUCCESS) {
                json arr = json::array();
                for (const auto& item : list) {
                    arr.push_back(json{{"id", item.id}, {"affected", item.affected}, {"comment", item.comment}});
                }
                send_data_direct(res, arr);
                spdlog::info("获取影响类型字典成功，数量: {}", list.size());
            } else {
                spdlog::error("获取影响类型字典失败");
                send_operation_failed(res, "获取", "影响类型字典");
            }
        } else if (table == "source_type_dict") {
            std::vector<SourceTypeDict> list;
            auto result = queryAllSourceTypeDict(db, list);
            if (result == DaoResult::SUCCESS) {
                json arr = json::array();
                for (const auto& item : list) {
                    arr.push_back(json{{"id", item.id}, {"type", item.type}, {"comment", item.comment}});
                }
                send_data_direct(res, arr);
                spdlog::info("获取来源类型字典成功，数量: {}", list.size());
            } else {
                spdlog::error("获取来源类型字典失败");
                send_operation_failed(res, "获取", "来源类型字典");
            }
        } else if (table == "work_record_status_dict") {
            std::vector<WorkRecordStatusDict> list;
            auto result = queryAllWorkRecordStatusDict(db, list);
            if (result == DaoResult::SUCCESS) {
                json arr = json::array();
                for (const auto& item : list) {
                    arr.push_back(json{{"id", item.id}, {"status_name", item.status_name}, {"status_class", item.status_class}});
                }
                send_data_direct(res, arr);
                spdlog::info("获取工作记录状态字典成功，数量: {}", list.size());
            } else {
                spdlog::error("获取工作记录状态字典失败");
                send_operation_failed(res, "获取", "工作记录状态字典");
            }
        } else if (table == "requirement_status_dict") {
            std::vector<RequirementStatusDict> list;
            auto result = queryAllRequirementStatusDict(db, list);
            if (result == DaoResult::SUCCESS) {
                json arr = json::array();
                for (const auto& item : list) {
                    arr.push_back(json{{"id", item.id}, {"status", item.status}, {"comment", item.comment}, {"requirement_status_class", item.requirement_status_class}});
                }
                send_data_direct(res, arr);
                spdlog::info("获取需求状态字典成功，数量: {}", list.size());
            } else {
                spdlog::error("获取需求状态字典失败");
                send_operation_failed(res, "获取", "需求状态字典");
            }
        } else if (table == "issue_progress_dict") {
            std::vector<IssueProgressDict> list;
            auto result = queryAllIssueProgressDict(db, list);
            if (result == DaoResult::SUCCESS) {
                json arr = json::array();
                for (const auto& item : list) {
                    arr.push_back(json{{"id", item.id}, {"progress", item.progress}, {"progress_class", item.progress_class}, {"comment", item.comment}});
                }
                send_data_direct(res, arr);
                spdlog::info("获取问题进展字典成功，数量: {}", list.size());
            } else {
                spdlog::error("获取问题进展字典失败");
                send_operation_failed(res, "获取", "问题进展字典");
            }
        } else if (table == "department_dict") {
            std::vector<DepartmentDict> list;
            auto result = queryAllDepartmentDict(db, list);
            if (result == DaoResult::SUCCESS) {
                json arr = json::array();
                for (const auto& item : list) {
                    arr.push_back(json{{"id", item.id}, {"name", item.name}, {"description", item.description}});
                }
                send_data_direct(res, arr);
                spdlog::info("获取部门字典成功，数量: {}", list.size());
            } else {
                spdlog::error("获取部门字典失败");
                send_operation_failed(res, "获取", "部门字典");
            }
        } else if (table == "employee_dict") {
            std::vector<EmployeeDict> list;
            auto result = queryAllEmployeeDict(db, list);
            if (result == DaoResult::SUCCESS) {
                json arr = json::array();
                for (const auto& item : list) {
                    arr.push_back(json{{"id", item.id}, {"name", item.name}, {"employee_number", item.employee_number}, {"department_name", item.department_name}});
                }
                send_data_direct(res, arr);
                spdlog::info("获取员工字典成功，数量: {}", list.size());
            } else {
                spdlog::error("获取员工字典失败");
                send_operation_failed(res, "获取", "员工字典");
            }
        } else {
            spdlog::warn("请求无效的字典类型: {}", table);
            send_bad_request(res, "无效的字典类型");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_dict");
        send_internal_error(res);
    }
}

// 新增字典项
void add_dict(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        std::string table = j["table"];

        if (table == "work_type_dict") {
            WorkTypeDict item;
            item.type = j["name"];
            item.comment = j.value("comment", "");
            auto result = insertWorkTypeDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res, json{{"id", item.id}});
                spdlog::info("新增工作类型字典成功，ID: {}, 名称: {}", item.id, item.type);
            } else {
                spdlog::error("新增工作类型字典失败，名称: {}", item.type);
                send_operation_failed(res, "插入", "字典项");
            }
        } else if (table == "affected_type_dict") {
            AffectedTypeDict item;
            item.affected = j["name"];
            item.comment = j.value("comment", "");
            auto result = insertAffectedTypeDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res, json{{"id", item.id}});
                spdlog::info("新增影响类型字典成功，ID: {}, 名称: {}", item.id, item.affected);
            } else {
                spdlog::error("新增影响类型字典失败，名称: {}", item.affected);
                send_operation_failed(res, "插入", "字典项");
            }
        } else if (table == "source_type_dict") {
            SourceTypeDict item;
            item.type = j["name"];
            item.comment = j.value("comment", "");
            auto result = insertSourceTypeDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res, json{{"id", item.id}});
                spdlog::info("新增来源类型字典成功，ID: {}, 名称: {}", item.id, item.type);
            } else {
                spdlog::error("新增来源类型字典失败，名称: {}", item.type);
                send_operation_failed(res, "插入", "字典项");
            }
        } else if (table == "work_record_status_dict") {
            WorkRecordStatusDict item;
            item.status_name = j["name"];
            item.status_class = j.value("status_class", "bg-primary");
            auto result = insertWorkRecordStatusDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res, json{{"id", item.id}});
                spdlog::info("新增工作记录状态字典成功，ID: {}, 名称: {}", item.id, item.status_name);
            } else {
                spdlog::error("新增工作记录状态字典失败，名称: {}", item.status_name);
                send_operation_failed(res, "插入", "字典项");
            }
        } else if (table == "requirement_status_dict") {
            RequirementStatusDict item;
            item.status = j["name"];
            item.requirement_status_class = j.value("status_class", "bg-primary");
            item.comment = j.value("comment", "");
            auto result = insertRequirementStatusDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res, json{{"id", item.id}});
                spdlog::info("新增需求状态字典成功，ID: {}, 名称: {}", item.id, item.status);
            } else {
                spdlog::error("新增需求状态字典失败，名称: {}", item.status);
                send_operation_failed(res, "插入", "字典项");
            }
        } else if (table == "issue_progress_dict") {
            IssueProgressDict item;
            item.progress = j["name"];
            item.progress_class = j.value("progress_class", "bg-primary");
            item.comment = j.value("comment", "");
            auto result = insertIssueProgressDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res, json{{"id", item.id}});
                spdlog::info("新增问题进展字典成功，ID: {}, 名称: {}", item.id, item.progress);
            } else {
                spdlog::error("新增问题进展字典失败，名称: {}", item.progress);
                send_operation_failed(res, "插入", "字典项");
            }
        } else if (table == "department_dict") {
            DepartmentDict item;
            item.name = j["name"];
            item.description = j.value("comment", "");
            auto result = insertDepartmentDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res, json{{"id", item.id}});
                spdlog::info("新增部门字典成功，ID: {}, 名称: {}", item.id, item.name);
            } else {
                spdlog::error("新增部门字典失败，名称: {}", item.name);
                send_operation_failed(res, "插入", "字典项");
            }
        } else if (table == "employee_dict") {
            EmployeeDict item;
            item.name = j["name"];
            item.employee_number = j.value("employee_number", "");
            item.department_name = j.value("department_name", "");
            auto result = insertEmployeeDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res, json{{"id", item.id}});
                spdlog::info("新增员工字典成功，ID: {}, 姓名: {}", item.id, item.name);
            } else {
                spdlog::error("新增员工字典失败，姓名: {}", item.name);
                send_operation_failed(res, "插入", "字典项");
            }
        } else {
            spdlog::warn("请求新增无效的字典类型: {}", table);
            send_bad_request(res, "无效的字典类型");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "add_dict");
        send_internal_error(res);
    }
}

// 删除字典项
void delete_dict(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        std::string table = j["table"];
        const int64_t id = j["id"];

        dao_util::DaoResult result;
        if (table == "work_type_dict") {
            result = deleteWorkTypeDict(db, id);
        } else if (table == "affected_type_dict") {
            result = deleteAffectedTypeDict(db, id);
        } else if (table == "source_type_dict") {
            result = deleteSourceTypeDict(db, id);
        } else if (table == "work_record_status_dict") {
            result = deleteWorkRecordStatusDict(db, id);
        } else if (table == "requirement_status_dict") {
            result = deleteRequirementStatusDict(db, id);
        } else if (table == "issue_progress_dict") {
            result = deleteIssueProgressDict(db, id);
        } else if (table == "department_dict") {
            result = deleteDepartmentDict(db, id);
        } else if (table == "employee_dict") {
            result = deleteEmployeeDict(db, id);
        } else {
            send_bad_request(res, "无效的字典类型");
            return;
        }

        if (result==dao_util::DaoResult::SUCCESS) {
            send_success(res);
        } else {
            send_operation_failed(res, "删除", "字典项");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "delete_dict");
        send_internal_error(res);
    }
}

// 更新字典项
void update_dict(const Request& req, Response& res) {
    try {
        auto j = json::parse(req.body);
        std::string table = j["table"];
        int id = j["id"];

        if (table == "work_type_dict") {
            WorkTypeDict item;
            item.id = id;
            item.type = j["name"];
            item.comment = j.value("comment", "");
            auto result = updateWorkTypeDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res);
                spdlog::info("更新工作类型字典成功，ID: {}, 名称: {}", item.id, item.type);
            } else {
                spdlog::error("更新工作类型字典失败，ID: {}, 名称: {}", item.id, item.type);
                send_operation_failed(res, "更新", "字典项");
            }
        } else if (table == "affected_type_dict") {
            AffectedTypeDict item;
            item.id = id;
            item.affected = j["name"];
            item.comment = j.value("comment", "");
            auto result = updateAffectedTypeDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res);
                spdlog::info("更新影响类型字典成功，ID: {}, 名称: {}", item.id, item.affected);
            } else {
                spdlog::error("更新影响类型字典失败，ID: {}, 名称: {}", item.id, item.affected);
                send_operation_failed(res, "更新", "字典项");
            }
        } else if (table == "source_type_dict") {
            SourceTypeDict item;
            item.id = id;
            item.type = j["name"];
            item.comment = j.value("comment", "");
            auto result = updateSourceTypeDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res);
                spdlog::info("更新来源类型字典成功，ID: {}, 名称: {}", item.id, item.type);
            } else {
                spdlog::error("更新来源类型字典失败，ID: {}, 名称: {}", item.id, item.type);
                send_operation_failed(res, "更新", "字典项");
            }
        } else if (table == "work_record_status_dict") {
            WorkRecordStatusDict item;
            item.id = id;
            item.status_name = j["name"];
            item.status_class = j.value("status_class", "bg-primary");
            auto result = updateWorkRecordStatusDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res);
                spdlog::info("更新工作记录状态字典成功，ID: {}, 名称: {}", item.id, item.status_name);
            } else {
                spdlog::error("更新工作记录状态字典失败，ID: {}, 名称: {}", item.id, item.status_name);
                send_operation_failed(res, "更新", "字典项");
            }
        } else if (table == "requirement_status_dict") {
            RequirementStatusDict item;
            item.id = id;
            item.status = j["name"];
            item.requirement_status_class = j.value("status_class", "bg-primary");
            item.comment = j.value("comment", "");
            auto result = updateRequirementStatusDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res);
                spdlog::info("更新需求状态字典成功，ID: {}, 名称: {}", item.id, item.status);
            } else {
                spdlog::error("更新需求状态字典失败，ID: {}, 名称: {}", item.id, item.status);
                send_operation_failed(res, "更新", "字典项");
            }
        } else if (table == "issue_progress_dict") {
            IssueProgressDict item;
            item.id = id;
            item.progress = j["name"];
            item.progress_class = j.value("progress_class", "bg-primary");
            item.comment = j.value("comment", "");
            auto result = updateIssueProgressDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res);
                spdlog::info("更新问题进展字典成功，ID: {}, 名称: {}", item.id, item.progress);
            } else {
                spdlog::error("更新问题进展字典失败，ID: {}, 名称: {}", item.id, item.progress);
                send_operation_failed(res, "更新", "字典项");
            }
        } else if (table == "department_dict") {
            DepartmentDict item;
            item.id = id;
            item.name = j["name"];
            item.description = j.value("comment", "");
            auto result = updateDepartmentDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res);
                spdlog::info("更新部门字典成功，ID: {}, 名称: {}", item.id, item.name);
            } else {
                spdlog::error("更新部门字典失败，ID: {}, 名称: {}", item.id, item.name);
                send_operation_failed(res, "更新", "字典项");
            }
        } else if (table == "employee_dict") {
            EmployeeDict item;
            item.id = id;
            item.name = j["name"];
            item.employee_number = j.value("employee_number", "");
            item.department_name = j.value("department_name", "");
            auto result = updateEmployeeDict(db, item);
            if (result == DaoResult::SUCCESS) {
                send_success(res);
                spdlog::info("更新员工字典成功，ID: {}, 姓名: {}", item.id, item.name);
            } else {
                spdlog::error("更新员工字典失败，ID: {}, 姓名: {}", item.id, item.name);
                send_operation_failed(res, "更新", "字典项");
            }
        } else {
            spdlog::warn("请求更新无效的字典类型: {}", table);
            send_bad_request(res, "无效的字典类型");
        }
    } catch (const std::exception& e) {
        log_util::log_exception(e, "update_dict");
        send_internal_error(res);
    }
}
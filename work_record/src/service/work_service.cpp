#include"service/work_service.h"
#include "dao/filerecord_dao.h"
#include "dao/work_record_file_dao.h"
#include "constants/constants_sql.h"
#include "constants/constants_upload.h"
#include "model/work_record_model.h"
#include "model/file_record_model.h"
#include <nlohmann/json.hpp>
#include <sqlite3/sqlite3.h>
#include "util/db_util.h"
#include "util/file_util.h"
#include <fstream>
#include <ctime>
#include "dao/requirement_record_dao.h"
#include "dao/work_record_status_dict_dao.h"
#include "service/requirement_service.h"
#include "util/string_util.h"
#include "dao/work_record_dao.h"
#include "util/dao_util.h"
#include "util/log_util.h"
#include "util/response_util.h"
#include <filesystem>
using namespace httplib;
using namespace dao_util;
using namespace response_util;
using json = nlohmann::json;
namespace fs = std::filesystem;
extern sqlite3 * db;

void get_work_records(const Request& req, Response& res) {
    try {
        WorkRecord newRecord;

        newRecord.requirement_id = std::stoi(req.get_file_value("requirement_id").content);
        newRecord.work_type_id = std::stoi(req.get_file_value("work_type_id").content);
        newRecord.affected_id = std::stoi(req.get_file_value("affected_id").content);
        newRecord.source_type_id = std::stoi(req.get_file_value("source_type_id").content);
        newRecord.work_record_status_id = std::stoi(req.get_file_value("work_record_status_id").content);
        newRecord.work_content = req.get_file_value("work_content").content;
        newRecord.employee_id = std::stoi(req.get_file_value("employee_id").content);
        newRecord.completion_time = req.get_file_value("completion_time").content;
        
        // 新建工作记录时处理completion_time
        if (req.has_file("completion_time")) {
            newRecord.completion_time = req.get_file_value("completion_time").content;
        }
        if (newRecord.completion_time.empty() || newRecord.completion_time == "") {
            time_t now = time(nullptr);
            char buf[20];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
            newRecord.completion_time = buf;
        }

        RequirementRecord reqInfo;
        auto reqResult = getRequirementById(db, newRecord.requirement_id, reqInfo);
        if (reqResult != DaoResult::SUCCESS) {
            spdlog::error("获取需求信息失败，需求ID: {}", newRecord.requirement_id);
            send_bad_request(res, "无效的需求ID");
            return;
        }

        // ==== 创建上传目录 ====
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        char year[8];
        strftime(year, sizeof(year), "%Y", t);
        std::string safeYear = year;
        std::string safeTitle = sanitizeFilename(reqInfo.title);
        std::string uploadDir = std::string(constants_upload::STATIC_UPLOAD_DIR) + safeYear + "/" + safeTitle;
        if (!createDirectory(uploadDir)) {
            spdlog::error("无法创建上传目录: {}", uploadDir);
            send_operation_failed(res, "创建", "上传目录");
            return;
        }
        
        // 插入工作记录
        int64_t newWorkId = 0;
        auto workResult = insertWorkRecord(db, newRecord, newWorkId);
        if (workResult != DaoResult::SUCCESS) {
            spdlog::error("工单插入失败，需求ID: {}", newRecord.requirement_id);
            send_operation_failed(res, "插入", "工单");
            return;
        }
        newRecord.id = newWorkId;
        if (newRecord.id <= 0) {
            spdlog::error("新建工单ID无效: {}", newRecord.id);
            send_operation_failed(res, "创建", "工单ID");
            return;
        }
        
        const auto &files = req.get_file_values("files");
        for (const auto& file : files) {
            fs::path origPath(file.filename);
            std::string base = sanitizeFilename(origPath.stem().string());
            std::string ext = origPath.extension().string();
            std::string safeName = base + "_" + std::to_string(time(nullptr)) + ext;
            std::string fullPath = uploadDir + "/" + safeName;
            if (!writeBinaryFile(fullPath, file.content)) {
                spdlog::error("无法写入文件: {}", fullPath);
                send_operation_failed(res, "写入", "文件");
                return;
            }
            FileRecord fileRec;
            fileRec.file_name = file.filename;
            fileRec.file_path = "/upload/" + safeYear + "/" + safeTitle + "/" + safeName;
            int64_t fileId = 0;
            auto fileResult = insertFileRecord(db, fileRec, fileId);
            if (fileResult != DaoResult::SUCCESS) {
                spdlog::error("文件记录创建失败，文件名: {}", file.filename);
                res.status = 500;
                res.set_content(json{{"error", false}, {"message", "文件记录创建失败"}}.dump(), "application/json");
                return;
            }
            
            // 建立关联
            auto relResult = insertWorkFileRel(db, newRecord.id, fileId);
            if (relResult != DaoResult::SUCCESS) {
                spdlog::error("工单文件关联失败，工单ID: {}, 文件ID: {}", newRecord.id, fileId);
                send_operation_failed(res, "关联", "工单文件");
                return;
            }
            
            newRecord.file_info[fileId] = json{{"name", fileRec.file_name},{"path", fileRec.file_path},{"upload_time", fileRec.create_time}}.dump();
        }
        
        send_success(res, json{{"id", newRecord.id}});
        spdlog::info("新建工单成功，ID: {}, 需求ID: {}", newRecord.id, newRecord.requirement_id);
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_work_records");
        send_internal_error(res);
    }
}


// 根据ID获取单条工作记录
void get_work_record_by_id(const Request& req, Response& res) {
    try {
        int64_t id = std::stoi(req.get_param_value("id"));
        
        WorkRecord record;
        auto result = getWorkRecordById(db, id, record);
        if (result != DaoResult::SUCCESS) {
            spdlog::error("获取工作记录失败，ID: {}", id);
            send_not_found(res, "工作记录不存在");
            return;
        }
        
        // 构建文件信息JSON
        nlohmann::json filesJson = nlohmann::json::object();
        for (const auto& f : record.file_info) {
            filesJson[std::to_string(f.first)] = nlohmann::json::parse(f.second);
        }
        
        // 返回单条记录
        nlohmann::json resp = {
            {"id", record.id},
            {"requirement_id", record.requirement_id},
            {"requirement_title", record.requirement_title},
            {"work_type_id", record.work_type_id},
            {"work_type", record.work_type},
            {"affected_id", record.affected_id},
            {"affected_name", record.affected_name},
            {"source_type_id", record.source_type_id},
            {"source_type_name", record.source_type_name},
            {"work_record_status_id", record.work_record_status_id},
            {"status_name", record.status_name},
            {"status_class", record.status_class},
            {"work_content", record.work_content},
            {"create_time", record.create_time},
            {"employee_id", record.employee_id},
            {"employee_name", record.employee_name},
            {"department_id", record.department_id},
            {"files", filesJson},
            {"completion_time", record.completion_time}
        };
        
        send_success(res, resp);
        spdlog::info("获取工作记录成功，ID: {}", id);
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_work_record_by_id");
        send_internal_error(res);
    }
}

// 获取所有工单接口重构
void get_all_work_records(const Request& req, Response& res) {
    int page = 1, page_size = 20, total = 0;
    std::string scope = req.has_param("scope") ? req.get_param_value("scope") : "";
    try { page = std::stoi(req.get_param_value("page")); } catch (...) {}
    try { page_size = std::stoi(req.get_param_value("page_size")); } catch (...) {}
    if (page < 1) page = 1;
    if (page_size != 10 && page_size != 20 && page_size != 50) page_size = 20;
    std::string status_id = req.has_param("status_id") ? req.get_param_value("status_id") : "";
    std::string affected_id = req.has_param("affected_id") ? req.get_param_value("affected_id") : "";
    std::string source_type_id = req.has_param("source_type_id") ? req.get_param_value("source_type_id") : "";
    std::string requirement_id = req.has_param("requirement_id") ? req.get_param_value("requirement_id") : "";
    std::string work_type_id = req.has_param("work_type_id") ? req.get_param_value("work_type_id") : "";
    auto records = queryWorkRecordsPagedDao(db, scope, page, page_size, total, status_id, affected_id, source_type_id, requirement_id, work_type_id);
    nlohmann::json j = nlohmann::json::array();
    for (const auto& r : records) {
        nlohmann::json filesJson = nlohmann::json::object();
        for (const auto& f : r.file_info) {
            filesJson[std::to_string(f.first)] = nlohmann::json::parse(f.second);
        }
        j.push_back({
            {"id", r.id},
            {"requirement_id", r.requirement_id},
            {"requirement_title", r.requirement_title},
            {"work_type_id", r.work_type_id},
            {"work_type", r.work_type},
            {"affected_id", r.affected_id},
            {"affected_name", r.affected_name},
            {"source_type_id", r.source_type_id},
            {"source_type_name", r.source_type_name},
            {"work_record_status_id", r.work_record_status_id},
            {"status_name", r.status_name},
            {"status_class", r.status_class},
            {"work_content", r.work_content},
            {"create_time", r.create_time},
            {"employee_id", r.employee_id},
            {"employee_name", r.employee_name},
            {"department_id", r.department_id},
            {"files", filesJson},
            {"completion_time", r.completion_time}
        });
    }
    nlohmann::json resp = {
        {"total", total},
        {"page", page},
        {"page_size", page_size},
        {"records", j}
    };
    send_success(res, resp);
}

// 查询工单状态字典重构
void get_work_record_status_dict(const Request& req, Response& res) {
    try {
        std::vector<WorkRecordStatusDict> list;
        auto result = queryAllWorkRecordStatusDict(db, list);
        if (result != DaoResult::SUCCESS) {
            spdlog::error("获取工单状态字典失败");
            send_operation_failed(res, "获取", "工单状态字典");
            return;
        }
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& item : list) {
            arr.push_back(nlohmann::json{
                {"id", item.id},
                {"status_name", item.status_name},
                {"status_class", item.status_class}
            });
        }
        send_data_direct(res, arr);
        spdlog::info("获取工单状态字典成功，数量: {}", list.size());
    } catch (const std::exception& e) {
        log_util::log_exception(e, "get_work_record_status_dict");
        send_internal_error(res);
    }
}


// 删除工单接口重构
void delete_work_record(const Request& req, Response& res) {
    try {
        int id = std::stoi(req.matches[1]);
        
        // 查出所有 file_record_id 及路径
        std::vector<std::pair<int, std::string>> fileInfos;
        sqlite3_stmt* selStmt = nullptr;
        if (sqlite3_prepare_v2(db, constants_sql::SQL_SELECT_WORK_FILES, -1, &selStmt, nullptr) != SQLITE_OK) {
            spdlog::error("删除工单失败，SQL预处理失败，工单ID: {}", id);
            send_operation_failed(res, "删除", "工单");
            return;
        }
        sqlite3_bind_int(selStmt, 1, id);
        while (sqlite3_step(selStmt) == SQLITE_ROW) {
            int fid = sqlite3_column_int(selStmt, 0);
            std::string fpath = reinterpret_cast<const char*>(sqlite3_column_text(selStmt, 1));
            fileInfos.emplace_back(fid, fpath);
        }
        sqlite3_finalize(selStmt);
        
        // 删除 work_record_files 关联
        auto relResult = deleteWorkFileRelByWork(db, id);
        if (relResult != DaoResult::SUCCESS) {
            spdlog::error("删除工单文件关联失败，工单ID: {}", id);
            send_operation_failed(res, "删除", "工单");
            return;
        }
        
        // 删除 file_record 及物理文件
        std::set<std::string> dirSet;
        for (const auto& [fid, fpath] : fileInfos) {
            auto fileResult = deleteFileRecord(db, fid);
            if (fileResult != DaoResult::SUCCESS) {
                spdlog::error("删除文件记录失败，文件ID: {}", fid);
            }
            std::string fullPath = "./static" + fpath;
            std::error_code ec;
            fs::remove(fs::path(fullPath), ec);
            auto dir = fs::path(fullPath).parent_path();
            dirSet.insert(dir.string());
        }
        
        // 删除空目录（只删到 upload/年/需求名 这一级）
        for (const auto& dir : dirSet) {
            std::error_code ec;
            auto p = fs::path(dir);
            if (fs::exists(p, ec) && fs::is_directory(p, ec) && fs::is_empty(p, ec)) {
                fs::remove(p, ec);
                auto parent = p.parent_path();
                if (fs::exists(parent, ec) && fs::is_directory(parent, ec) && fs::is_empty(parent, ec) && parent.filename() != "upload") {
                    fs::remove(parent, ec);
                }
            }
        }
        
        // 删除 work_record 主表记录
        sqlite3_stmt* delStmt = nullptr;
        if (sqlite3_prepare_v2(db, "DELETE FROM work_record WHERE id = ?;", -1, &delStmt, nullptr) != SQLITE_OK) {
            spdlog::error("删除工单主记录失败，SQL预处理失败，工单ID: {}", id);
            send_operation_failed(res, "删除", "工单");
            return;
        }
        sqlite3_bind_int(delStmt, 1, id);
        if (sqlite3_step(delStmt) != SQLITE_DONE) {
            spdlog::error("删除工单主记录失败，工单ID: {}", id);
            sqlite3_finalize(delStmt);
            send_operation_failed(res, "删除", "工单");
            return;
        }
        sqlite3_finalize(delStmt);
        
        send_success(res);
        spdlog::info("删除工单成功，ID: {}", id);
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "delete_work_record");
        send_internal_error(res);
    }
}

// 上传文件接口重构
void upload_work_record_file(const Request& req, Response& res) {
    try {
        int id = std::stoi(req.matches[1]);
        
        // 查找工单及需求标题
        WorkRecord record;
        RequirementRecord reqInfo;
        auto workResult = getWorkRecordById(db, id, record);
        auto reqResult = getRequirementById(db, record.requirement_id, reqInfo);
        if (workResult != DaoResult::SUCCESS || reqResult != DaoResult::SUCCESS) {
            spdlog::error("工单不存在，工单ID: {}", id);
            send_not_found(res, "工单不存在");
            return;
        }
        
        // 目录 static/upload/今年/需求标题/
        time_t now = time(nullptr);
        tm* t = localtime(&now);
        char year[8];
        strftime(year, sizeof(year), "%Y", t);
        std::string safeYear = sanitizeFilename(std::string(year));
        std::string safeTitle = sanitizeFilename(reqInfo.title);
        std::string uploadDir = std::string(constants_upload::STATIC_UPLOAD_DIR) + safeYear + "/" + safeTitle;
        if (!createDirectory(uploadDir)) {
            spdlog::error("无法创建上传目录: {}", uploadDir);
            send_operation_failed(res, "创建", "上传目录");
            return;
        }
        
        const auto &files = req.get_file_values("files");
        std::vector<int> fileIds;
        for (const auto& file : files) {
            fs::path origPath(file.filename);
            std::string base = sanitizeFilename(origPath.stem().string());
            std::string ext = origPath.extension().string();
            std::string safeName = base + "_" + std::to_string(time(nullptr)) + ext;
            std::string fullPath = uploadDir + "/" + safeName;
            if (!writeBinaryFile(fullPath, file.content)) {
                spdlog::error("无法写入文件: {}", fullPath);
                send_operation_failed(res, "写入", "文件");
                return;
            }
            FileRecord fileRec;
            fileRec.file_name = file.filename;
            fileRec.file_path = "/upload/" + safeYear + "/" + safeTitle + "/" + safeName;
            int64_t fileId = 0;
            auto fileResult = insertFileRecord(db, fileRec, fileId);
            if (fileResult != DaoResult::SUCCESS) {
                spdlog::error("文件记录创建失败，文件名: {}", file.filename);
                send_operation_failed(res, "创建", "文件记录");
                return;
            }
            fileIds.push_back(fileId);
        }
        
        // 建立关联
        for (int fileId : fileIds) {
            auto relResult = insertWorkFileRel(db, id, fileId);
            if (relResult != DaoResult::SUCCESS) {
                spdlog::error("工单文件关联失败，工单ID: {}, 文件ID: {}", id, fileId);
                send_operation_failed(res, "关联", "工单文件");
                return;
            }
        }
        
        send_success(res);
        spdlog::info("工单上传文件成功，工单ID: {}, 文件数量: {}", id, fileIds.size());
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "upload_work_record_file");
        send_internal_error(res);
    }
}

// 文件下载接口
void download_file(const Request& req, Response& res) {
    std::string path = req.get_param_value("path");
    std::string filename = req.has_param("filename") ? req.get_param_value("filename") : "";
    if (path.empty() || path.find("..") != std::string::npos) {
        res.status = 400;
        res.set_content("Invalid path", "text/plain");
        return;
    }
    std::string fullPath = "./static" + path;
    std::wstring wfullPath = utf8string2wstring(fullPath);
    std::ifstream ifs(wfullPath.c_str(), std::ios::binary);
    if (!ifs) {
        res.status = 404;
        res.set_content("File not found", "text/plain");
        return;
    }
    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    if (filename.empty()) {
        // 自动从 path 提取文件名
        size_t pos = path.find_last_of("/");
        filename = (pos != std::string::npos) ? path.substr(pos + 1) : path;
    }
    std::string header = "attachment; filename=\"" + filename + "\"";
    res.set_header("Content-Disposition", header.c_str());
    res.set_content(buffer.data(), buffer.size(), "application/octet-stream");
    // 可根据文件类型设置 content-type
}

// 单文件删除接口重构
void delete_file(const Request& req, Response& res) {
    try {
        int fileId = std::stoi(req.matches[1]);
        
        // 查找文件路径
        FileRecord fileRec;
        auto fileResult = getFileRecordById(db, fileId, fileRec);
        if (fileResult != DaoResult::SUCCESS) {
            spdlog::error("文件不存在，文件ID: {}", fileId);
            send_not_found(res, "文件不存在");
            return;
        }
        
        // 删除 work_record_files 关联
        sqlite3_stmt* delRelStmt;
        if (sqlite3_prepare_v2(db, constants_sql::SQL_DELETE_WORK_FILE_REL, -1, &delRelStmt, nullptr) != SQLITE_OK) {
            spdlog::error("删除文件关联失败，SQL预处理失败，文件ID: {}", fileId);
            send_operation_failed(res, "删除", "文件");
            return;
        }
        sqlite3_bind_int(delRelStmt, 1, fileId);
        if (sqlite3_step(delRelStmt) != SQLITE_DONE) {
            spdlog::error("删除文件关联失败，文件ID: {}", fileId);
            sqlite3_finalize(delRelStmt);
            send_operation_failed(res, "删除", "文件");
            return;
        }
        sqlite3_finalize(delRelStmt);
        
        // 删除 file_record
        auto deleteResult = deleteFileRecord(db, fileId);
        if (deleteResult != DaoResult::SUCCESS) {
            spdlog::error("删除文件记录失败，文件ID: {}", fileId);
        }
        
        // 删除物理文件
        fs::path fullPath = "./static" + fileRec.file_path;
        std::error_code ec;
        fs::remove(fullPath, ec);
        
        // 删除空目录（只删到 upload/年/需求名 这一级）
        auto dir = fs::path(fullPath).parent_path();
        if (fs::exists(dir) && fs::is_directory(dir) && fs::is_empty(dir)) {
            fs::remove(dir, ec);
            auto parent = dir.parent_path();
            if (fs::exists(parent) && fs::is_directory(parent) && fs::is_empty(parent) && parent.filename() != "upload") {
                fs::remove(parent, ec);
            }
        }
        
        send_success(res);
        spdlog::info("删除文件成功，文件ID: {}", fileId);
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "delete_file");
        send_internal_error(res);
    }
}

// 更新工单接口（支持字段和文件）
void update_work_record(const Request& req, Response& res) {
    try {
        WorkRecord record;
        if (!req.has_file("id")) {
            spdlog::error("更新工单失败，缺少ID参数");
            send_bad_request(res, "缺少ID参数");
            return;
        }
        
        record.id = std::stoi(req.get_file_value("id").content);
        record.requirement_id = std::stoi(req.get_file_value("requirement_id").content);
        record.work_type_id = std::stoi(req.get_file_value("work_type_id").content);
        record.affected_id = std::stoi(req.get_file_value("affected_id").content);
        record.source_type_id = std::stoi(req.get_file_value("source_type_id").content);
        record.work_record_status_id = std::stoi(req.get_file_value("work_record_status_id").content);
        record.work_content = req.get_file_value("work_content").content;
        record.employee_id = std::stoi(req.get_file_value("employee_id").content);
        record.completion_time = req.get_file_value("completion").content;
        
        // 1. 处理需删除文件
        if (req.has_file("delete_file_ids")) {
            std::string delIdsStr = req.get_file_value("delete_file_ids").content;
            std::vector<int> delIds;
            size_t pos = 0;
            while ((pos = delIdsStr.find(',')) != std::string::npos) {
                delIds.push_back(std::stoi(delIdsStr.substr(0, pos)));
                delIdsStr.erase(0, pos + 1);
            }
            if (!delIdsStr.empty()) delIds.push_back(std::stoi(delIdsStr));
            
            for (int fileId : delIds) {
                FileRecord fileRec;
                auto fileResult = getFileRecordById(db, fileId, fileRec);
                if (fileResult == DaoResult::SUCCESS) {
                    // 删除关联
                    sqlite3_stmt* delRelStmt;
                    if (sqlite3_prepare_v2(db, constants_sql::SQL_DELETE_WORK_FILE_REL, -1, &delRelStmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_int(delRelStmt, 1, fileId);
                        sqlite3_step(delRelStmt);
                        sqlite3_finalize(delRelStmt);
                    }
                    
                    // 删除文件记录
                    auto deleteResult = deleteFileRecord(db, fileId);
                    if (deleteResult != DaoResult::SUCCESS) {
                        spdlog::error("删除文件记录失败，文件ID: {}", fileId);
                    }
                    
                    // 删除物理文件
                    fs::path fullPath = "./static" + fileRec.file_path;
                    std::error_code ec;
                    fs::remove(fullPath, ec);
                    
                    // 删除空目录
                    auto dir = fs::path(fullPath).parent_path();
                    if (fs::exists(dir) && fs::is_directory(dir) && fs::is_empty(dir)) {
                        fs::remove(dir, ec);
                        auto parent = dir.parent_path();
                        if (fs::exists(parent) && fs::is_directory(parent) && fs::is_empty(parent) && parent.filename() != "upload") {
                            fs::remove(parent, ec);
                        }
                    }
                }
            }
        }

        // 2. 处理新上传文件（直接保存并建立关联）
        std::vector<int> newFileIds;
        if (req.has_file("files")) {
            RequirementRecord reqInfo;
            auto reqResult = getRequirementById(db, record.requirement_id, reqInfo);
            if (reqResult != DaoResult::SUCCESS) {
                spdlog::error("更新工单失败，无效的需求ID: {}", record.requirement_id);
                send_operation_failed(res, "更新", "工单");
                return;
            }
            
            time_t now = time(nullptr);
            tm* t = localtime(&now);
            char year[8];
            strftime(year, sizeof(year), "%Y", t);
            std::string safeYear = year;
            std::string safeTitle = sanitizeFilename(reqInfo.title);
            std::string uploadDir = std::string(constants_upload::STATIC_UPLOAD_DIR) + safeYear + "/" + safeTitle;
            if (!createDirectory(uploadDir)) {
                spdlog::error("更新工单失败，无法创建上传目录: {}", uploadDir);
                send_operation_failed(res, "创建", "上传目录");
                return;
            }
            
            const auto &files = req.get_file_values("files");
            for (const auto& file : files) {
                fs::path origPath(file.filename);
                std::string base = sanitizeFilename(origPath.stem().string());
                std::string ext = origPath.extension().string();
                std::string safeName = base + "_" + std::to_string(time(nullptr)) + ext;
                std::string fullPath = uploadDir + "/" + safeName;
                if (!writeBinaryFile(fullPath, file.content)) {
                    spdlog::error("更新工单失败，无法写入文件: {}", fullPath);
                    send_operation_failed(res, "写入", "文件");
                    return;
                }
                
                FileRecord fileRec;
                fileRec.file_name = file.filename;
                fileRec.file_path = "/upload/" + safeYear + "/" + safeTitle + "/" + safeName;
                int64_t fileId = 0;
                auto fileResult = insertFileRecord(db, fileRec, fileId);
                if (fileResult != DaoResult::SUCCESS) {
                    spdlog::error("更新工单失败，文件记录创建失败，文件名: {}", file.filename);
                    send_operation_failed(res, "创建", "文件记录");
                    return;
                }
                
                auto relResult = insertWorkFileRel(db, record.id, fileId);
                if (relResult != DaoResult::SUCCESS) {
                    spdlog::error("更新工单失败，工单文件关联失败，工单ID: {}, 文件ID: {}", record.id, fileId);
                    send_operation_failed(res, "关联", "工单文件");
                    return;
                }
                
                newFileIds.push_back(fileId);
            }
        }
        
        // 3. 处理 new_file_ids 字段（兼容前端主动传递的id，防止多端上传）
        if (req.has_file("new_file_ids")) {
            std::string newIdsStr = req.get_file_value("new_file_ids").content;
            size_t pos = 0;
            while ((pos = newIdsStr.find(',')) != std::string::npos) {
                int id = std::stoi(newIdsStr.substr(0, pos));
                auto relResult = insertWorkFileRel(db, record.id, id);
                if (relResult != DaoResult::SUCCESS) {
                    spdlog::error("更新工单失败，关联现有文件失败，工单ID: {}, 文件ID: {}", record.id, id);
                }
                newIdsStr.erase(0, pos + 1);
            }
            if (!newIdsStr.empty()) {
                int id = std::stoi(newIdsStr);
                auto relResult = insertWorkFileRel(db, record.id, id);
                if (relResult != DaoResult::SUCCESS) {
                    spdlog::error("更新工单失败，关联现有文件失败，工单ID: {}, 文件ID: {}", record.id, id);
                }
            }
        }
        
        // 4. 更新工单其它字段
        if (req.has_file("completion_time")) {
            record.completion_time = req.get_file_value("completion_time").content;
        }
        if (record.completion_time.empty() || record.completion_time == "") {
            time_t now = time(nullptr);
            char buf[20];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
            record.completion_time = buf;
        }
        
        auto updateResult = updateWorkRecord(db, record);
        if (updateResult != DaoResult::SUCCESS) {
            spdlog::error("更新工单失败，工单ID: {}", record.id);
            send_operation_failed(res, "更新", "工单");
            return;
        }
        
        send_success(res, json{{"new_file_ids", newFileIds}});
        spdlog::info("更新工单成功，ID: {}", record.id);
        
    } catch (const std::exception& e) {
        log_util::log_exception(e, "update_work_record");
        send_internal_error(res);
    }
}
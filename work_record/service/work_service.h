#pragma once
#include "../dao/work_record_dao.h"
#include "../dao/filerecord_dao.h"
#include "../dao/work_record_file_dao.h"
#include "../constants/constants_sql.h"
#include "../constants/constants_upload.h"
#include "../model/work_record_model.h"
#include "../model/file_record_model.h"
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"
#include "../db/sqlite3.h"
#include "../db/db_util.h"
#include "../util/file_util.h"
#include <fstream>
#include <ctime>
#include "../dao/requirement_record_dao.h"
#include "../dao/work_record_status_dict_dao.h"
#include "../service/requirement_service.h"
extern sqlite3* db;
using namespace httplib;
using json = nlohmann::json;
namespace fs = std::filesystem;

inline void get_work_records(const Request& req, Response& res) {
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);
    try {
        WorkRecord newRecord;

        newRecord.requirement_id = std::stoi(req.get_file_value("requirement_id").content);
        newRecord.work_type_id = std::stoi(req.get_file_value("work_type_id").content);
        newRecord.affected_id = std::stoi(req.get_file_value("affected_id").content);
        newRecord.source_type_id = std::stoi(req.get_file_value("source_type_id").content);
        newRecord.work_record_status_id = std::stoi(req.get_file_value("work_record_status_id").content);
        newRecord.work_content = req.get_file_value("work_content").content;
        newRecord.employee_id = std::stoi(req.get_file_value("employee_id").content);

        RequirementRecord reqInfo;
        if (!getRequirementById(db, newRecord.requirement_id, reqInfo)) {
            throw std::runtime_error("无效的需求ID");
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
            throw std::runtime_error("无法创建上传目录");
        }
        // 插入工单
        int newWorkId = 0;
        insertWorkRecord(db, newRecord, newWorkId);
        newRecord.id = newWorkId;
        if (newRecord.id <= 0) {
            throw std::runtime_error("新建工单ID无效");
        }
        const auto &files = req.get_file_values("files");
        for (const auto& file : files) {
            fs::path origPath(file.filename);
            std::string base = sanitizeFilename(origPath.stem().string());
            std::string ext = origPath.extension().string();
            std::string safeName = base + "_" + std::to_string(time(nullptr)) + ext;
            std::string fullPath = uploadDir + "/" + safeName;
            if (!writeBinaryFile(fullPath, file.content)) {
                res.status = 500;
                res.set_content(json{{"error", std::string("无法打开或写入文件: ") + fullPath}}.dump(), "application/json");
                return;
            }
            FileRecord fileRec;
            fileRec.file_name = file.filename;
            fileRec.file_path = "/upload/" + safeYear + "/" + safeTitle + "/" + safeName;
            int fileId = 0;
            if (!insertFileRecord(db, fileRec, fileId)) {
                throw std::runtime_error("文件记录创建失败");
            }
            // 建立关联
            sqlite3_stmt* relStmt = nullptr;
            if (sqlite3_prepare_v2(db, constants_sql::SQL_INSERT_WORK_FILE_REL, -1, &relStmt, nullptr) != SQLITE_OK) {
                throw std::runtime_error("工单文件关联SQL预处理失败");
            }
            sqlite3_bind_int(relStmt, 1, newRecord.id);
            sqlite3_bind_int(relStmt, 2, fileId);
            if (sqlite3_step(relStmt) != SQLITE_DONE)  {
                std::string errMsg = "工单文件关联失败: ";
                errMsg += sqlite3_errmsg(db);
                sqlite3_finalize(relStmt);
                throw std::runtime_error(errMsg);
            }
            sqlite3_finalize(relStmt);
            newRecord.file_info[fileId] = json{{"name", fileRec.file_name},{"path", fileRec.file_path},{"upload_time", fileRec.create_time}}.dump();
        }
        sqlite3_exec(db, "COMMIT;", 0, 0, 0);
        res.set_content(json{{"id", newRecord.id}}.dump(), "application/json");
    } catch (const std::exception& e) {
        sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// 后端分页接口支持 affected_id 过滤
inline std::vector<WorkRecord> queryWorkRecordsPaged(const std::string& scope, int page, int page_size, int& total, const std::string& status_id = "", const std::string& affected_id = "", const std::string& source_type_id = "", const std::string& requirement_id = "", const std::string& work_type_id = "") {
    return queryWorkRecordsPagedDao(db, scope, page, page_size, total, status_id, affected_id, source_type_id, requirement_id, work_type_id);
}

// 获取所有工单接口重构
inline void get_all_work_records(const Request& req, Response& res) {
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
            {"files", filesJson}
        });
    }
    nlohmann::json resp = {
        {"total", total},
        {"page", page},
        {"page_size", page_size},
        {"records", j}
    };
    res.set_content(resp.dump(), "application/json");
}

// 查询工单状态字典重构
inline void get_work_record_status_dict(const Request& req, Response& res) {
    try {
        auto list = queryAllWorkRecordStatusDict(db);
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& item : list) {
            arr.push_back({
                {"id", item.id},
                {"status_name", item.status_name},
                {"status_class", item.status_class}
            });
        }
        res.set_content(arr.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}


// 删除工单接口重构
inline void delete_work_record(const Request& req, Response& res) {
    int id = std::stoi(req.matches[1]);
    // 查出所有 file_record_id 及路径
    std::vector<std::pair<int, std::string>> fileInfos;
    const char* selSql = constants_sql::SQL_SELECT_WORK_FILES;
    sqlite3_stmt* selStmt;
    if (sqlite3_prepare_v2(db, selSql, -1, &selStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(selStmt, 1, id);
        while (sqlite3_step(selStmt) == SQLITE_ROW) {
            int fid = sqlite3_column_int(selStmt, 0);
            std::string fpath = reinterpret_cast<const char*>(sqlite3_column_text(selStmt, 1));
            fileInfos.emplace_back(fid, fpath);
        }
        sqlite3_finalize(selStmt);
    }
    // 删除 work_record_files 关联
    deleteWorkFileRelByWork(db, id);
    // 删除 file_record 及物理文件
    std::set<std::string> dirSet;
    for (const auto& [fid, fpath] : fileInfos) {
        deleteFileRecord(db, fid);
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
    // 删除工单与需求的关联（如有）
    const char* delReqRel = "DELETE FROM requirement_work_record WHERE work_record_id = ?;";
    sqlite3_stmt* delReqRelStmt;
    if (sqlite3_prepare_v2(db, delReqRel, -1, &delReqRelStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(delReqRelStmt, 1, id);
        sqlite3_step(delReqRelStmt);
        sqlite3_finalize(delReqRelStmt);
    }
    // 删除工单
    sqlite3_stmt* delWorkStmt;
    sqlite3_prepare_v2(db, constants_sql::SQL_DELETE_WORK_RECORD, -1, &delWorkStmt, nullptr);
    sqlite3_bind_int(delWorkStmt, 1, id);
    sqlite3_step(delWorkStmt);
    sqlite3_finalize(delWorkStmt);
    res.set_content(json{{"success", true}}.dump(), "application/json");
}

// 上传文件接口重构
inline void upload_work_record_file(const Request& req, Response& res) {
    int id = std::stoi(req.matches[1]);
    // 查找工单及需求标题
    WorkRecord record;
    RequirementRecord reqInfo;
    if (!getWorkRecordById(db, id, record) || !getRequirementById(db, record.requirement_id, reqInfo)) {
        res.status = 404;
        res.set_content(json{{"error", "工单不存在"}}.dump(), "application/json");
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
        res.status = 500;
        res.set_content(json{{"error", std::string("无法创建上传目录: ") + uploadDir}}.dump(), "application/json");
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
            res.status = 500;
            res.set_content(json{{"error", std::string("无法打开或写入文件: ") + fullPath}}.dump(), "application/json");
            return;
        }
        FileRecord fileRec;
        fileRec.file_name = file.filename;
        fileRec.file_path = "/upload/" + safeYear + "/" + safeTitle + "/" + safeName;
        int fileId = 0;
        if (!insertFileRecord(db, fileRec, fileId)) {
            res.status = 500;
            res.set_content(json{{"error", "文件记录创建失败"}}.dump(), "application/json");
            return;
        }
        fileIds.push_back(fileId);
    }
    // 建立关联
    for (int fileId : fileIds) {
        insertWorkFileRel(db, id, fileId);
    }
    res.set_content(json{{"success", true}}.dump(), "application/json");
}

// 文件下载接口
inline void download_file(const Request& req, Response& res) {
    std::string path = req.get_param_value("path");
    std::string filename = req.has_param("filename") ? req.get_param_value("filename") : "";
    if (path.empty() || path.find("..") != std::string::npos) {
        res.status = 400;
        res.set_content("Invalid path", "text/plain");
        return;
    }
    std::string fullPath = "./static" + path;
    std::ifstream ifs(fullPath, std::ios::binary);
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
inline void delete_file(const Request& req, Response& res) {
    int fileId = std::stoi(req.matches[1]);
    // 查找文件路径
    FileRecord fileRec;
    if (!getFileRecordById(db, fileId, fileRec)) {
        res.status = 404;
        res.set_content(json{{"error", "文件不存在"}}.dump(), "application/json");
        return;
    }
    // 删除 work_record_files 关联
    sqlite3_stmt* delRelStmt;
    sqlite3_prepare_v2(db, constants_sql::SQL_DELETE_WORK_FILE_REL, -1, &delRelStmt, nullptr);
    sqlite3_bind_int(delRelStmt, 1, fileId);
    sqlite3_step(delRelStmt);
    sqlite3_finalize(delRelStmt);
    // 删除 file_record
    deleteFileRecord(db, fileId);
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
    res.set_content(json{{"success", true}}.dump(), "application/json");
}

// 更新工单接口（支持字段和文件）
inline void update_work_record(const Request& req, Response& res) {
    try {
        WorkRecord record;
        if (!req.has_file("id")) throw std::runtime_error("缺少id");
        record.id = std::stoi(req.get_file_value("id").content);
        record.requirement_id = std::stoi(req.get_file_value("requirement_id").content);
        record.work_type_id = std::stoi(req.get_file_value("work_type_id").content);
        record.affected_id = std::stoi(req.get_file_value("affected_id").content);
        record.source_type_id = std::stoi(req.get_file_value("source_type_id").content);
        record.work_record_status_id = std::stoi(req.get_file_value("work_record_status_id").content);
        record.work_content = req.get_file_value("work_content").content;
        record.employee_id = std::stoi(req.get_file_value("employee_id").content);

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
                if (getFileRecordById(db, fileId, fileRec)) {
                    sqlite3_stmt* delRelStmt;
                    sqlite3_prepare_v2(db, constants_sql::SQL_DELETE_WORK_FILE_REL, -1, &delRelStmt, nullptr);
                    sqlite3_bind_int(delRelStmt, 1, fileId);
                    sqlite3_step(delRelStmt);
                    sqlite3_finalize(delRelStmt);
                    deleteFileRecord(db, fileId);
                    fs::path fullPath = "./static" + fileRec.file_path;
                    std::error_code ec;
                    fs::remove(fullPath, ec);
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
            if (!getRequirementById(db, record.requirement_id, reqInfo)) {
                throw std::runtime_error("无效的需求ID");
            }
            time_t now = time(nullptr);
            tm* t = localtime(&now);
            char year[8];
            strftime(year, sizeof(year), "%Y", t);
            std::string safeYear = year;
            std::string safeTitle = sanitizeFilename(reqInfo.title);
            std::string uploadDir = std::string(constants_upload::STATIC_UPLOAD_DIR) + safeYear + "/" + safeTitle;
            if (!createDirectory(uploadDir)) {
                throw std::runtime_error("无法创建上传目录");
            }
            const auto &files = req.get_file_values("files");
            for (const auto& file : files) {
                fs::path origPath(file.filename);
                std::string base = sanitizeFilename(origPath.stem().string());
                std::string ext = origPath.extension().string();
                std::string safeName = base + "_" + std::to_string(time(nullptr)) + ext;
                std::string fullPath = uploadDir + "/" + safeName;
                if (!writeBinaryFile(fullPath, file.content)) {
                    res.status = 500;
                    res.set_content(json{{"error", std::string("无法打开或写入文件: ") + fullPath}}.dump(), "application/json");
                    return;
                }
                FileRecord fileRec;
                fileRec.file_name = file.filename;
                fileRec.file_path = "/upload/" + safeYear + "/" + safeTitle + "/" + safeName;
                int fileId = 0;
                if (!insertFileRecord(db, fileRec, fileId)) {
                    throw std::runtime_error("文件记录创建失败");
                }
                insertWorkFileRel(db, record.id, fileId);
                newFileIds.push_back(fileId);
            }
        }
        // 3. 处理 new_file_ids 字段（兼容前端主动传递的id，防止多端上传）
        if (req.has_file("new_file_ids")) {
            std::string newIdsStr = req.get_file_value("new_file_ids").content;
            size_t pos = 0;
            while ((pos = newIdsStr.find(',')) != std::string::npos) {
                int id = std::stoi(newIdsStr.substr(0, pos));
                insertWorkFileRel(db, record.id, id);
                newIdsStr.erase(0, pos + 1);
            }
            if (!newIdsStr.empty()) insertWorkFileRel(db, record.id, std::stoi(newIdsStr));
        }
        // 4. 更新工单其它字段
        updateWorkRecord(db, record);
        res.set_content(json{{"success", true}, {"new_file_ids", newFileIds}}.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
}

// =================== 需求相关接口已迁移到 requirement_service.h ===================
// 只保留接口声明和注释，删除所有需求相关实现，避免重复定义
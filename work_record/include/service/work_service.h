#pragma once
#include <httplib/httplib.h>
#include "model/work_record_model.h"
void get_work_records(const httplib::Request& req, httplib::Response& res) ;

// 后端分页接口支持 affected_id 过滤
std::vector<WorkRecord> queryWorkRecordsPaged(const std::string& scope, int page, int page_size, int& total, const std::string& status_id = "", const std::string& affected_id = "", const std::string& source_type_id = "", const std::string& requirement_id = "", const std::string& work_type_id = "") ;

// 获取所有工单接口重构
void get_all_work_records(const httplib::Request& req, httplib::Response& res) ;

// 查询工单状态字典重构
void get_work_record_status_dict(const httplib::Request& req, httplib::Response& res) ;


// 删除工单接口重构
void delete_work_record(const httplib::Request& req, httplib::Response& res) ;

// 上传文件接口重构
void upload_work_record_file(const httplib::Request& req, httplib::Response& res) ;

// 文件下载接口
void download_file(const httplib::Request& req, httplib::Response& res) ;

// 单文件删除接口重构
void delete_file(const httplib::Request& req, httplib::Response& res) ;

// 更新工单接口（支持字段和文件）
void update_work_record(const httplib::Request& req, httplib::Response& res) ;
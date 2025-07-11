#pragma once
#include <httplib/httplib.h>
#include "model/work_record_model.h"
void get_work_records(const httplib::Request& req, httplib::Response& res) ;

// 根据ID获取单条工作记录
void get_work_record_by_id(const httplib::Request& req, httplib::Response& res) ;

// 获取所有工作记录接口重构
void get_all_work_records(const httplib::Request& req, httplib::Response& res) ;

// 查询工作记录状态字典重构
void get_work_record_status_dict(const httplib::Request& req, httplib::Response& res) ;

// 删除工作记录接口重构
void delete_work_record(const httplib::Request& req, httplib::Response& res) ;

// 上传文件接口重构
void upload_work_record_file(const httplib::Request& req, httplib::Response& res) ;

// 文件下载接口
void download_file(const httplib::Request& req, httplib::Response& res) ;

// 单文件删除接口重构
void delete_file(const httplib::Request& req, httplib::Response& res) ;

// 更新工作记录接口（支持字段和文件）
void update_work_record(const httplib::Request& req, httplib::Response& res) ;
#pragma once
#include <httplib/httplib.h>

// 删除需求进展接口
void delete_requirement_record(const httplib::Request& req, httplib::Response& res) ;
// 获取所有需求接口重构
void get_all_requirement_records(const httplib::Request& req, httplib::Response& res) ;


// 需求进展新增/修改分发接口
void add_or_update_requirement_record(const httplib::Request& req, httplib::Response& res) ;

// 更新需求记录
void update_requirement(const httplib::Request& req, httplib::Response& res) ;

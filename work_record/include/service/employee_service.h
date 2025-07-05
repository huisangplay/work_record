#pragma once
#include <httplib/httplib.h>
// 获取所有人员，支持 department_id 过滤
void get_all_employees(const httplib::Request& req, httplib::Response& res) ;

// 新增人员
void add_employee(const httplib::Request& req, httplib::Response& res) ;

// 更新人员
void update_employee(const httplib::Request& req, httplib::Response& res) ;

// 删除人员
void delete_employee(const httplib::Request& req, httplib::Response& res) ;
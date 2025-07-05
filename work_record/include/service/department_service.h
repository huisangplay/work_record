#pragma once
#include <httplib/httplib.h>

// 获取所有部门
void get_all_departments(const httplib::Request& req, httplib::Response& res) ;

// 新增部门
void add_department(const httplib::Request& req, httplib::Response& res) ;

// 更新部门
void update_department(const httplib::Request& req, httplib::Response& res) ;

// 删除部门
void delete_department(const httplib::Request& req, httplib::Response& res) ;
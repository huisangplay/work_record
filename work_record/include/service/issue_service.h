#pragma once
#include <httplib/httplib.h>

// 获取所有问题接口重构（字段补全、结构规范、异常健壮）
void get_all_issues(const httplib::Request& req, httplib::Response& res) ;
// 分页查询问题记录接口
void get_issues_paged(const httplib::Request& req, httplib::Response& res) ;
// 新增问题接口
void add_issue(const httplib::Request& req, httplib::Response& res) ;

// 修改问题接口
void update_issue(const httplib::Request& req, httplib::Response& res) ;

// 删除问题接口
void delete_issue(const httplib::Request& req, httplib::Response& res) ;
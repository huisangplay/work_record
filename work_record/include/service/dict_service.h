#include <httplib/httplib.h>
// 获取所有字典表通用接口重构
void get_dict(const httplib::Request& req, httplib::Response& res) ;

// 新增字典项
void add_dict(const httplib::Request& req, httplib::Response& res) ;

// 更新字典项
void update_dict(const httplib::Request& req, httplib::Response& res) ;

// 删除字典项
void delete_dict(const httplib::Request& req, httplib::Response& res) ;
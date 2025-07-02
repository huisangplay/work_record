#ifdef _WIN32
#include <winsock2.h>
#endif
#include "service/work_service.h"
#include "service/department_service.h"
#include "service/employee_service.h"
#include "util/file_util.h"
#include <iostream>
#include "third_party/httplib.h"
#include "third_party/json.hpp"
#include "db/sqlite3.h"
#include "service/issue_service.h"
#include "service/dict_service.h"
using namespace httplib;
using json = nlohmann::json;

sqlite3* db;

// ==== 主程序入口 ====
int main() {
    // 设置控制台输出代码页为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    db = openDB();
    if (!db) {
        std::cerr << "database connect failed. please check your profile." << std::endl;
        return 1;
    }

    Server svr;
    svr.set_base_dir("./static");

    // [GET] 获取工作记录
    svr.Get("/api/work_records", get_all_work_records);

    // [GET] 获取需求记录
    svr.Get("/api/requirement_records", get_all_requirement_records);

    // [GET] 获取问题记录
    svr.Get("/api/issues", get_all_issues);

    // [POST] 创建工作记录（含文件上传）
    svr.Post("/api/get_work_records", get_work_records);

    // [GET] 工单状态字典
    svr.Get("/api/work_record_status_dict", get_work_record_status_dict);
    // [DELETE] 删除工单
    svr.Delete(R"(/api/work_record/(\d+))", delete_work_record);
    // [POST] 工单上传文件
    svr.Post(R"(/api/work_record/(\d+)/upload)", upload_work_record_file);
    // [GET] 文件下载
    svr.Get("/api/download", download_file);
    // [DELETE] 删除单个文件
    svr.Delete(R"(/api/file/(\d+))", delete_file);
    // [GET] 通用字典接口
    svr.Get("/api/dict", get_dict);
    svr.Post("/api/dict", add_dict);
    svr.Put("/api/dict", update_dict);
    svr.Delete("/api/dict", delete_dict);
    // [POST] 更新工作记录
    svr.Post("/api/update_work_record", update_work_record);
    // [POST] 需求进展新增/修改（自动分发）
    svr.Post("/api/requirement_record", add_or_update_requirement_record);
    // [DELETE] 需求进展删除（只支持 GET 参数 id）
    svr.Delete("/api/delete_requirement_record", delete_requirement_record);
    // [POST] 新增问题
    svr.Post("/api/add_issue", add_issue);
    // [POST] 修改问题
    svr.Post("/api/update_issue", update_issue);
    // [POST] 删除问题
    svr.Post("/api/delete_issue", delete_issue);

    // [GET] 获取所有部门
    svr.Get("/api/departments", get_all_departments);
    
    // [POST] 新增部门
    svr.Post("/api/department", add_department);
    
    // [POST] 更新部门
    svr.Post("/api/update_department", update_department);
    
    // [DELETE] 删除部门
    svr.Delete("/api/department", delete_department);
    
    // [GET] 获取所有人员
    svr.Get("/api/employees", get_all_employees);
    
    // [POST] 新增人员
    svr.Post("/api/employee", add_employee);
    
    // [POST] 更新人员
    svr.Post("/api/update_employee", update_employee);
    
    // [DELETE] 删除人员
    svr.Delete("/api/employee", delete_employee);

    std::cout << "🚀 服务已启动：http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    sqlite3_close(db);
    return 0;
}



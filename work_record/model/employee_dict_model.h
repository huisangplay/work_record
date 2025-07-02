#pragma once
#include <string>
#include "..\third_party\json.hpp"

struct EmployeeDict {
    int id;
    std::string name;  // 对应数据库的name字段
    std::string employee_number;
    int department_id;
    std::string comment;
    std::string create_time;
    std::string department_name; // 关联查询时使用
}; 
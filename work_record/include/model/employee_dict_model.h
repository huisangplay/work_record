#pragma once
#include <string>
#include <cstdint>

struct EmployeeDict {
    int64_t id;
    std::string name;  // 对应数据库的name字段
    std::string employee_number;
    int64_t department_id;
    std::string comment;
    std::string create_time;
    std::string department_name; // 关联查询时使用
}; 
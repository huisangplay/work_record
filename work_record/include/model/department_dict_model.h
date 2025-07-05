#pragma once
#include <string>

struct DepartmentDict {
    int id;
    std::string name;  // 对应数据库的name字段
    std::string description;  // 对应数据库的description字段
    std::string create_time;
}; 
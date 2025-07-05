#pragma once
#include <string>
#include <cstdint>

struct RequirementRecord {
    int64_t id;
    std::string title;
    int64_t requirement_status_id;
    std::string requirement_status_name;
    std::string requirement_status_class;
    int64_t affected_id;
    std::string affected_name;
    int64_t source_type_id;
    std::string source_type_name;
    std::string create_time;
    std::string update_time;
    int64_t employee_id;
    std::string employee_name;
    int64_t department_id;
};

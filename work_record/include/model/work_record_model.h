#pragma once
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <cstdint>

struct WorkRecord {
    int64_t id;
    int64_t requirement_id;
    std::string requirement_title;
    std::string create_time;
    int64_t work_type_id;
    std::string work_type;
    std::string work_content;
    int64_t affected_id;
    std::string affected_name;
    int64_t source_type_id;
    std::string source_type_name;
    int64_t work_record_status_id;
    std::string status_name;
    std::string status_class;
    std::string completion_time;
    int64_t employee_id;
    std::string employee_name;
    int64_t department_id;
    std::map<int64_t, std::string> file_info;
};

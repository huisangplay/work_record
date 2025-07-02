#pragma once
#include <string>
#include <map>
#include "..\third_party\json.hpp"
struct WorkRecord {
    int id;
    int requirement_id;
    std::string requirement_title;
    std::string create_time;
    int work_type_id;
    std::string work_type;
    std::string work_content;
    int affected_id;
    std::string affected_name;
    int source_type_id;
    std::string source_type_name;
    int work_record_status_id;
    std::string status_name;
    std::string status_class;
    std::string update_time;
    int employee_id;
    std::string employee_name;
    int department_id;
    std::map<int, std::string> file_info;
};

#pragma once
#include <string>
struct RequirementRecord {
    int id;
    std::string title;
    int requirement_status_id;
    std::string requirement_status_name;
    std::string requirement_status_class;
    int affected_id;
    std::string affected_name;
    int source_type_id;
    std::string source_type_name;
    std::string create_time;
    std::string update_time;
    int employee_id;
    std::string employee_name;
    int department_id;
};

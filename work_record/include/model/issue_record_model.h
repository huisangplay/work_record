#pragma once
#include <string>
#include <cstdint>

struct IssueRecord {
    int64_t id;
    std::string issue_title;
    std::string reported_by;
    std::string description;
    int64_t progress_id;
    std::string progress_name;
    std::string progress_class;
    int64_t affected_id;
    std::string affected_name;
    int64_t source_type_id;
    std::string source_type_name;
    std::string create_time;
    std::string update_time;
    std::string responsible_person;
    int64_t employee_id;
    std::string employee_name;
    int64_t department_id;
};

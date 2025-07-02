#pragma once
#include <string>
struct IssueRecord {
    int id;
    std::string issue_title;
    std::string reported_by;
    std::string description;
    int progress_id;
    std::string progress_name;
    std::string progress_class;
    int affected_id;
    std::string affected_name;
    int source_type_id;
    std::string source_type_name;
    std::string create_time;
    std::string update_time;
    std::string responsible_person;
    int employee_id;
    std::string employee_name;
    int department_id;
};

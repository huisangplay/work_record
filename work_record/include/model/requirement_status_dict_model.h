#pragma once
#include <string>
#include <cstdint>

struct RequirementStatusDict {
    int64_t id;
    std::string status;
    std::string comment;
    std::string requirement_status_class;
};

#pragma once
#include <string>
#include <cstdint>

struct IssueProgressDict {
    int64_t id;
    std::string progress;
    std::string progress_class;
    std::string comment;
};

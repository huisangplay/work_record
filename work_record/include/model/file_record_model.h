#pragma once
#include <cstdint>
#include <string>

struct FileRecord {
    int64_t id;
    std::string file_name;
    std::string file_path;
    std::string create_time;
};

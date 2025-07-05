#pragma once
#include <string>
#include <string_view>
#include <algorithm>
#include <filesystem>
#include <fstream>

inline std::string sanitizeFilename(const std::string& name) {
    static constexpr std::string_view invalidChars = "<>:\"/\\|?*";
    std::string clean;
    clean.reserve(name.size());
    std::transform(name.begin(), name.end(), std::back_inserter(clean), [](char c) {
        return invalidChars.find(c) != std::string_view::npos ? '_' : c;
    });
    if (clean.size() > 100) clean.resize(100);
    return clean;
}

inline bool createDirectory(const std::string& path) {
#if defined(_WIN32) && __cpp_char8_t
    try {
        std::filesystem::path fsPath(reinterpret_cast<const char8_t*>(path.c_str()));
        std::filesystem::create_directories(fsPath);
        return true;
    } catch (...) {
        return false;
    }
#else
    try {
        std::filesystem::path fsPath = std::filesystem::u8path(path);
        std::filesystem::create_directories(fsPath);
        return true;
    } catch (...) {
        return false;
    }
#endif
}

inline bool writeBinaryFile(const std::string& utf8Path, const std::string& data) {
#if defined(_WIN32) && __cpp_char8_t
    try {
        std::filesystem::path fsPath(reinterpret_cast<const char8_t*>(utf8Path.c_str()));
        std::ofstream ofs(fsPath, std::ios::binary);
        if (!ofs) return false;
        ofs.write(data.data(), data.size());
        return ofs.good();
    } catch (...) {
        return false;
    }
#else
    try {
        std::filesystem::path fsPath = std::filesystem::u8path(utf8Path);
        std::ofstream ofs(fsPath, std::ios::binary);
        if (!ofs) return false;
        ofs.write(data.data(), data.size());
        return ofs.good();
    } catch (...) {
        return false;
    }
#endif
}
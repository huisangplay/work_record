#pragma once
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

std::wstring utf8string2wstring(const std::string& str) {
#ifdef _WIN32
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
#else
    // 非 Windows 平台的简单实现
    return std::wstring(str.begin(), str.end());
#endif
}
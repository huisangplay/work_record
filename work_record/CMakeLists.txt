cmake_minimum_required(VERSION 3.16)
project(work_record)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置编译选项
if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-Wall -Wextra)
endif()

# 查找必要的包
find_package(Threads REQUIRED)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/third_party)
include_directories(${CMAKE_SOURCE_DIR}/include)

# 源文件
set(MAIN_SOURCE main.cpp)

# DAO 层源文件
file(GLOB DAO_SOURCES "src/dao/*.cpp")

# Service 层源文件
file(GLOB SERVICE_SOURCES "src/service/*.cpp")

# Util 层源文件
file(GLOB UTIL_SOURCES "src/util/*.cpp")

# Config 层源文件
file(GLOB CONFIG_SOURCES "src/config/*.cpp")

# 第三方库源文件
set(THIRD_PARTY_SOURCES
    third_party/sqlite3/sqlite3.c
    third_party/httplib/httplib.cc
)

# 检查 httplib 是否存在
if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/httplib.cc")
    list(APPEND THIRD_PARTY_SOURCES third_party/httplib.cc)
endif()

# 所有源文件
set(ALL_SOURCES
    ${MAIN_SOURCE}
    ${DAO_SOURCES}
    ${SERVICE_SOURCES}
    ${UTIL_SOURCES}
    ${CONFIG_SOURCES}
    ${THIRD_PARTY_SOURCES}
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME} 
    Threads::Threads
)

# Windows 特定设置
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
    
    # 设置控制台代码页为 UTF-8
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601)
endif()

# 复制静态文件到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/static
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/static
)

# 复制数据库文件到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/db
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/db
)

# 打印构建信息
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Source files:")
message(STATUS "  Main: ${MAIN_SOURCE}")
message(STATUS "  DAO: ${DAO_SOURCES}")
message(STATUS "  Service: ${SERVICE_SOURCES}")
message(STATUS "  Util: ${UTIL_SOURCES}")
message(STATUS "  Config: ${CONFIG_SOURCES}")
message(STATUS "  Third Party: ${THIRD_PARTY_SOURCES}") 
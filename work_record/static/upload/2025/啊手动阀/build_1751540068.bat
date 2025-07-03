@echo off
chcp 65001 >nul
pushd %~dp0
REM 编译 sqlite3.o（如果不存在）
if not exist sqlite3.o (
    echo [INFO] sqlite3.o 不存在，开始编译 db\sqlite3.c...
    gcc -c db\sqlite3.c -o sqlite3.o
    if errorlevel 1 (
        echo [ERROR] 编译 db\sqlite3.c 失败
        popd
        exit /b 1
    )
) else (
    echo [SKIP] sqlite3.o 已存在，跳过编译
)

REM 编译 httplib.o（如果存在 .cpp 文件）
if exist third_party\httplib.cpp (
    if not exist httplib.o (
        echo [INFO] httplib.o 不存在，开始编译 third_party\httplib.cpp...
        g++ -std=c++20 -c third_party\httplib.cpp -o httplib.o
        if errorlevel 1 (
            echo [ERROR] 编译 third_party\httplib.cpp 失败
            popd
            exit /b 1
        )
    ) else (
        echo [SKIP] httplib.o 已存在，跳过编译
    )
) else (
    echo [SKIP] 未找到 third_party\httplib.cpp，跳过 httplib.o 编译
)

REM 编译 json.o（如果存在 .cpp 文件）
if exist third_party\json.cpp (
    if not exist json.o (
        echo [INFO] json.o 不存在，开始编译 third_party\json.cpp...
        g++ -std=c++20 -c third_party\json.cpp -o json.o
        if errorlevel 1 (
            echo [ERROR] 编译 third_party\json.cpp 失败
            popd
            exit /b 1
        )
    ) else (
        echo [SKIP] json.o 已存在，跳过编译
    )
) else (
    echo [SKIP] 未找到 third_party\json.cpp，跳过 json.o 编译
)

REM 编译 main.o
if exist main.o del main.o

echo [INFO] 开始编译 main.cpp...
g++ -std=c++20 -c main.cpp -o main.o
if errorlevel 1 (
    echo [ERROR] 编译 main.cpp 失败
    popd
    exit /b 1
) else (
    echo [OK] main.cpp 编译完成
)

REM 链接
set LINK_OBJS=main.o sqlite3.o
if exist httplib.o set LINK_OBJS=%LINK_OBJS% httplib.o
if exist json.o set LINK_OBJS=%LINK_OBJS% json.o

echo [INFO] 开始链接...
g++ %LINK_OBJS% -o myapp.exe -lWs2_32 -lpthread
if errorlevel 1 (
    echo [ERROR] 链接失败
    popd
    exit /b 1
) else (
    echo [OK] build success myapp.exe
)
popd


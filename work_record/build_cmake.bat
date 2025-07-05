@echo off
chcp 65001 >nul
pushd %~dp0

echo [INFO] 开始 CMake 构建...

REM 创建构建目录
if not exist build_cmake mkdir build_cmake
cd build_cmake

REM 配置项目
echo [INFO] 配置项目...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo [ERROR] CMake 配置失败
    popd
    exit /b 1
)

REM 编译项目
echo [INFO] 编译项目...
cmake --build . --config Release
if errorlevel 1 (
    echo [ERROR] 编译失败
    popd
    exit /b 1
)

echo [OK] 构建成功！
echo [INFO] 可执行文件位置: build_cmake\bin\work_record.exe

REM 返回上级目录
cd ..

REM 询问是否运行程序
set /p choice="是否运行程序？(y/n): "
if /i "%choice%"=="y" (
    echo [INFO] 启动程序...
    cd build_cmake\bin
    work_record.exe
    cd ..\..
)

popd 
@echo off
REM Set UTF-8 encoding
chcp 65001 >nul
REM Set console output encoding
powershell -Command "[Console]::OutputEncoding = [System.Text.Encoding]::UTF8" >nul 2>&1

pushd %~dp0

echo [INFO] Starting CMake build...

REM Create build directory
if not exist build_cmake mkdir build_cmake
cd build_cmake

REM Configure project
echo [INFO] Configuring project...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo [ERROR] CMake configuration failed
    popd
    exit /b 1
)

REM Build project
echo [INFO] Building project...
cmake --build . --config Release
if errorlevel 1 (
    echo [ERROR] Build failed
    popd
    exit /b 1
)

echo [OK] Build successful!
echo [INFO] Executable location: build_cmake\bin\work_record.exe

REM Return to parent directory
cd ..

REM Ask if run program
set /p choice="Run program? (y/n): "
if /i "%choice%"=="y" (
    echo [INFO] Starting program...
    cd build_cmake\bin
    work_record.exe
    cd ..\..
)

popd 
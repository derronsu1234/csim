@echo off
setlocal
REM ============================================================
REM  FastCsim one-click MSVC STATIC build script
REM  Usage: double-click, or run the "build: msvc: static" task in VS Code
REM  Builds with CSIM_USE_STATIC_MODELS=ON (all models linked into the exe,
REM  no model DLLs are produced).
REM  Requires: CMake, Visual Studio 2022 (C++ toolchain), Bison, Flex
REM  Adjust MSYS_BIN (bison/flex) below if needed.
REM  NOTE: keep this file ASCII-only to avoid cmd codepage issues.
REM ============================================================

REM ---------- Toolchain paths (edit if needed) ----------
set "MSYS_BIN=D:\msys64\usr\bin"
set "BUNDLED_BIN=%~dp0third_party\msys\usr\bin"
REM 优先使用工程内置的 bison/flex/m4（third_party/msys），再回退到 MSYS2
if exist "%BUNDLED_BIN%\bison.exe" set "PATH=%BUNDLED_BIN%;%PATH%"
if exist "%MSYS_BIN%\bison.exe" set "PATH=%MSYS_BIN%;%PATH%"

where bison.exe >nul 2>nul
if errorlevel 1 goto err_bison

where cmake >nul 2>nul
if errorlevel 1 goto err_cmake

REM ---------- Go to project root ----------
cd /d "%~dp0"

REM ---------- Ensure output directory exists ----------
if not exist "bin" mkdir "bin"

REM ---------- 1. CMake configure (first time only) ----------
if exist "build_msvc_static\CMakeCache.txt" goto do_build
echo [1/3] Configuring CMake (Visual Studio 17 2022, x64, Debug, STATIC models)...
cmake -G "Visual Studio 17 2022" -A x64 -DCSIM_USE_STATIC_MODELS=ON -S . -B build_msvc_static
REM Visual Studio 16 2019
if errorlevel 1 goto err_cfg

REM ---------- 2. Build ----------
:do_build
echo [2/3] Building (Debug)...
cmake --build build_msvc_static --config Debug -- /m
if errorlevel 1 goto err_build

echo.
echo ============================================================
echo  Build OK: bin\Debug\circuit.exe  (STATIC, no model DLLs)
echo  Usage: bin\Debug\circuit.exe test\case1_op.sp
echo ============================================================
goto :eof

:err_bison
echo [ERROR] bison not found. Install MSYS2 or edit MSYS_BIN in build_msvc_static.bat.
exit /b 1

:err_cmake
echo [ERROR] cmake not found. Install CMake and add it to PATH.
exit /b 1

:err_cfg
echo [ERROR] CMake configure failed.
exit /b 1

:err_build
echo [ERROR] Build failed.
exit /b 1

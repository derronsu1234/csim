@echo off
setlocal
REM ============================================================
REM  FastCsim one-click STATIC build script
REM  Builds all component models statically into circuit.exe
REM  (no model DLLs; only system runtime DLLs are needed).
REM  Usage: double-click, or run "build: static" task in VS Code
REM  Requires: CMake, MinGW-w64 (GCC), Bison, Flex
REM  NOTE: keep this file ASCII-only to avoid cmd codepage issues.
REM ============================================================

REM ---------- Toolchain paths (edit if needed) ----------
set "MINGW_BIN=D:\msys64\mingw64\bin"
set "MSYS_BIN=D:\msys64\usr\bin"
set "BUNDLED_BIN=%~dp0third_party\msys\usr\bin"
REM 优先使用工程内置的 bison/flex/m4（third_party/msys），再回退到 MSYS2
if exist "%BUNDLED_BIN%\bison.exe" set "PATH=%BUNDLED_BIN%;%PATH%"
if exist "%MINGW_BIN%\mingw32-make.exe" set "PATH=%MINGW_BIN%;%MSYS_BIN%;%PATH%"

where mingw32-make.exe >nul 2>nul
if errorlevel 1 goto err_make

REM ---------- Go to project root ----------
cd /d "%~dp0"

REM ---------- Ensure output directory exists ----------
if not exist "bin" mkdir "bin"

REM ---------- 1. CMake configure (static mode) ----------
if exist "build_static\CMakeCache.txt" goto do_build
echo [1/2] Configuring CMake (static models, MinGW Makefiles, Debug)...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCSIM_USE_STATIC_MODELS=ON -S . -B build_static
if errorlevel 1 goto err_cmake

REM ---------- 2. Build ----------
:do_build
echo [2/2] Building (static)...
mingw32-make.exe -C build_static -j8
if errorlevel 1 goto err_build

echo.
echo ============================================
echo  Build OK: bin\circuit.exe  (static, no model DLLs)
echo  Usage: bin\circuit.exe test\case1_op.sp
echo ============================================
goto :eof

:err_make
echo [ERROR] mingw32-make not found. Install MinGW-w64 or edit MINGW_BIN in build_static.bat.
exit /b 1

:err_cmake
echo [ERROR] CMake configure failed.
exit /b 1

:err_build
echo [ERROR] Build failed.
exit /b 1

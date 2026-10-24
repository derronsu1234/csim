@echo off
setlocal
REM ============================================================
REM  FastCsim one-click build script
REM  Usage: double-click, or run the "build" task in VS Code
REM  Requires: CMake, MinGW-w64 (GCC), Bison, Flex
REM  Adjust MINGW_BIN / MSYS_BIN below if needed.
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

REM ---------- Ensure output directory exists (linker does not create it) ----------
if not exist "bin" mkdir "bin"

REM ---------- 1. CMake configure (first time only) ----------
if exist "build\CMakeCache.txt" goto do_build
echo [1/2] Configuring CMake (MinGW Makefiles, Debug)...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -S . -B build
if errorlevel 1 goto err_cmake

REM ---------- 2. Build ----------
:do_build
echo [2/2] Building...
mingw32-make.exe -C build -j8
if errorlevel 1 goto err_build

echo.
echo ============================================
echo  Build OK: bin\circuit.exe
echo  Usage: bin\circuit.exe test\case1_op.sp
echo ============================================
goto :eof

:err_make
echo [ERROR] mingw32-make not found. Install MinGW-w64 or edit MINGW_BIN in build.bat.
exit /b 1

:err_cmake
echo [ERROR] CMake configure failed.
exit /b 1

:err_build
echo [ERROR] Build failed.
exit /b 1

@echo off
REM Build all corex examples with GCC (MSYS2 UCRT64).
REM Usage:  build.bat          -> compile every example into build\
REM         build.bat run      -> compile, then run each example
REM         build.bat clean    -> remove the build\ folder

setlocal enabledelayedexpansion

set CC=gcc
set CFLAGS=-std=c99 -Wall -Wextra -I.
set LDFLAGS=-lm
set OUT=build

if "%~1"=="clean" (
    if exist "%OUT%" rmdir /s /q "%OUT%"
    echo Cleaned %OUT%\
    goto :eof
)

if not exist "%OUT%" mkdir "%OUT%"

set EXAMPLES=pong camera_demo collision_demo impl_guard_test

for %%E in (%EXAMPLES%) do (
    echo [build] %%E
    %CC% %CFLAGS% examples\%%E.c -o "%OUT%\%%E.exe" %LDFLAGS%
    if errorlevel 1 (
        echo [error] failed to build %%E
        exit /b 1
    )
)

echo All examples built into %OUT%\

if "%~1"=="run" (
    for %%E in (%EXAMPLES%) do (
        echo.
        echo ===== %%E =====
        "%OUT%\%%E.exe"
    )
)

endlocal

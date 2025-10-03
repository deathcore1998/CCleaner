@echo off
REM --- Build folder ---
set BUILD_DIR=build

echo --- Create build folder if it does not exist ---
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

cd %BUILD_DIR%

echo --- Check if a .sln file already exists ---
set "SlnFound=false"
for %%f in (*.sln) do (
    set "SlnFound=true"
)

setlocal enabledelayedexpansion

if "!SlnFound!"=="true" (
    echo --- Solution file already exists in %BUILD_DIR%. Skipping generation.
) else (
    echo --- Generate Visual Studio 2022 solution ---
    cmake .. -G "Visual Studio 17 2022"
    echo Solution generated successfully.
)

endlocal

pause
@echo off
call m.bat
cd build
if exist c_utils_test.exe (
    echo RUNNING
    c_utils_test.exe
) else (
    echo NO EXECUTABLE FOUND
)
cd ..

@echo off
call m.bat
cd build
if exist c_utils_test.exe (
    echo STARTING GDB
    gdb -q c_utils_test.exe
) else (
    echo NO EXECUTABLE FOUND
)
cd ..

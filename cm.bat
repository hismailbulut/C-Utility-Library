@echo off
if not exist build\ (
    mkdir build
)
cd build
cmake -DTESTS_ENABLED=1 .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_EXPORT_COMPILE_COMMANDS=1
if exist compile_commands.json (
    move compile_commands.json ..\compile_commands.json
) else (
    echo COMPILE COMMANDS JSON NOT FOUND
)
cd ..

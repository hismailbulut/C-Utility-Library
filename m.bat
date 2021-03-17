@echo off
if not exist build\ (
    call cm.bat
)
cd build
make
cd ..

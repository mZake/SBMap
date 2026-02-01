@echo off

rem Get absolute path to root and build directory
pushd %~dp0
cd ..
set ROOT=%CD%
set BUILD=%ROOT%\build
popd

if not exist "%BUILD%" mkdir "%BUILD%"

rem Configure and build
cmake -S "%ROOT%" -B "%BUILD%" -DCMAKE_BUILD_TYPE=Release
cmake --build "%BUILD%" --config Release

pause

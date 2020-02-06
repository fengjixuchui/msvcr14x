@echo off
if %Platform% == x64 (
set LIB=%msvcr140_ROOT%\x64\Release;%msvcr140_ROOT%\x64\Debug;%LIB%
)
if %Platform% == x86 (
set LIB=%msvcr140_ROOT%\Release;%msvcr140_ROOT%\Debug;%LIB%
)

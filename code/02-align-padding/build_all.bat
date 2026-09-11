@echo off
REM ============================================================
REM  W2D3 one-click build+run helper (NOT learning code)
REM  Usage: open "x64 Native Tools Command Prompt for VS 2022",
REM         cd into this folder, then type build_all.bat
REM  All cpp files are written by the learner; this just calls cl.
REM  NOTE: do NOT double-click this file. It must run inside the
REM        VS x64 Native Tools window where "cl" is on PATH.
REM ============================================================
echo ===== [1/4] verify =====
cl /EHsc verify.cpp
if exist verify.exe (verify.exe) else (echo BUILD FAILED: verify)
echo.
echo ===== [2/4] ptr_arith =====
cl /EHsc ptr_arith.cpp
if exist ptr_arith.exe (ptr_arith.exe) else (echo BUILD FAILED: ptr_arith)
echo.
echo ===== [3/4] memcpy_pitfall =====
cl /EHsc memcpy_pitfall.cpp
if exist memcpy_pitfall.exe (memcpy_pitfall.exe) else (echo BUILD FAILED: memcpy_pitfall)
echo.
echo ===== [4/4] pack_demo =====
cl /EHsc pack_demo.cpp
if exist pack_demo.exe (pack_demo.exe) else (echo BUILD FAILED: pack_demo)
echo.
echo ===== ALL DONE - screenshot this window =====
pause

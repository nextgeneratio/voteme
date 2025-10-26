@echo off
setlocal ENABLEDELAYEDEXPANSION

REM Build VoteMe on Windows without Makefile
REM - Detects GCC/Clang (MinGW/MSYS2/LLVM) first, otherwise MSVC (cl)
REM - Produces executables in bin\ and ensures data\ exists

cd /d "%~dp0"

if not exist bin mkdir bin
if not exist data mkdir data

set CC=
where gcc >NUL 2>&1 && set CC=gcc
if not defined CC (
  where clang >NUL 2>&1 && set CC=clang
)

if defined CC goto build_gcc

where cl >NUL 2>&1 && goto build_msvc

echo [ERROR] No supported C compiler found.
echo - Install MSYS2 + MinGW-w64 (gcc) OR Visual Studio (cl) OR LLVM (clang)
exit /b 1

:build_gcc
echo.
echo === Building with %CC% ===
set CFLAGS=-Wall -Wextra -std=c99 -O2 -Isrc

echo [1/5] bin\voteme.exe
%CC% %CFLAGS% -o bin\voteme.exe src\main.c src\display.c
if errorlevel 1 goto err

echo [2/5] bin\admin.exe
%CC% %CFLAGS% -o bin\admin.exe ^
  src\admin.c ^
  src\data_handler_enhanced.c ^
  src\voting.c ^
  src\ui_utils.c ^
  src\csv_io.c ^
  src\data_errors.c ^
  src\entity_codec.c ^
  src\entity_service.c ^
  src\voting-interface.c
if errorlevel 1 goto err

echo [3/5] bin\voter_register.exe
%CC% %CFLAGS% -o bin\voter_register.exe ^
  src\voter_register.c ^
  src\voting-interface.c ^
  src\data_handler_enhanced.c ^
  src\csv_io.c ^
  src\data_errors.c
if errorlevel 1 goto err

echo [4/5] bin\candidate_register.exe
%CC% %CFLAGS% -o bin\candidate_register.exe ^
  src\candidate_register.c ^
  src\data_handler_enhanced.c ^
  src\csv_io.c ^
  src\data_errors.c
if errorlevel 1 goto err

echo [5/5] bin\vote.exe
%CC% %CFLAGS% -o bin\vote.exe ^
  src\vote_cli.c ^
  src\voting-interface.c ^
  src\data_handler_enhanced.c ^
  src\csv_io.c ^
  src\data_errors.c
if errorlevel 1 goto err

echo.
echo [SUCCESS] Build completed with %CC%.
echo Run: bin\voteme.exe  (main menu)
exit /b 0

:build_msvc
echo.
echo === Building with MSVC (cl) ===
set CLFLAGS=/nologo /W4 /EHsc /I src

echo [1/5] bin\voteme.exe
cl %CLFLAGS% /Fe:bin\voteme.exe src\main.c src\display.c
if errorlevel 1 goto err

echo [2/5] bin\admin.exe
cl %CLFLAGS% /Fe:bin\admin.exe ^
  src\admin.c ^
  src\data_handler_enhanced.c ^
  src\voting.c ^
  src\ui_utils.c ^
  src\csv_io.c ^
  src\data_errors.c ^
  src\entity_codec.c ^
  src\entity_service.c ^
  src\voting-interface.c
if errorlevel 1 goto err

echo [3/5] bin\voter_register.exe
cl %CLFLAGS% /Fe:bin\voter_register.exe ^
  src\voter_register.c ^
  src\voting-interface.c ^
  src\data_handler_enhanced.c ^
  src\csv_io.c ^
  src\data_errors.c
if errorlevel 1 goto err

echo [4/5] bin\candidate_register.exe
cl %CLFLAGS% /Fe:bin\candidate_register.exe ^
  src\candidate_register.c ^
  src\data_handler_enhanced.c ^
  src\csv_io.c ^
  src\data_errors.c
if errorlevel 1 goto err

echo [5/5] bin\vote.exe
cl %CLFLAGS% /Fe:bin\vote.exe ^
  src\vote_cli.c ^
  src\voting-interface.c ^
  src\data_handler_enhanced.c ^
  src\csv_io.c ^
  src\data_errors.c
if errorlevel 1 goto err

echo.
echo [SUCCESS] Build completed with MSVC.
echo Run: bin\voteme.exe  (main menu)
exit /b 0

:err
echo.
echo [FAILED] Build failed. See errors above.
exit /b 1

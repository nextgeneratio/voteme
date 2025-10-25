@echo off
set outfile=voteme.exe
set cflags=-Wall -Wextra
@echo [*] Compiling...
@gcc %cflags% src\*.c -o %outfile%
if ERRORLEVEL 0 (
	@echo [-] Compilation failed!
) else (
	@echo [*] Done!
)
pause

@echo off
set outfile=bin\voteme.exe
set cflags=-Wall -Wextra
@echo [*] Compiling...
@gcc %cflags% src\* -o %outfile%
if ERRORLEVEL 0 (
	@echo [-] Compilation failed!
) else (
	@echo [*] Done!
)
pause

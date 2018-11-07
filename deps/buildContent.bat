@echo off
for /f "tokens=1-4 delims=/ " %%i in ("%date%") do (
     set day=%%i
     set month=%%j
     set year=%%k
)
set datestr=%year%_%month%_%day%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
msbuild /nologo /fileLogger /flp:Summary;Verbosity=normal;logfile=%~dp0/../Application/bin/data/logs/%datestr%.build;append=true "%~dp0/../Content/" 
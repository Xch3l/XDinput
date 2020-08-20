@echo off
setlocal
:: %1 Source file
:: %2 Target exe

:: Check if target EXE uses DirectInput8
set dllname=dinput8.dll
find /I "%dllname%" %2>NUL
if "%ERRORLEVEL%" == "0" goto doCopy

:: Check if target EXE uses DirectInput7 or older
set dllname=dinput.dll
find /I "%dllname%" %2>NUL
if "%ERRORLEVEL%" == "0" goto doCopy

:wrongVersion
echo Target EXE does not support dinput or dinput8
goto fin

:doCopy
copy %1 "%~dp2%dllname%"

:fin
endlocal
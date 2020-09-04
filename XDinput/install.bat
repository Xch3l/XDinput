@echo off
:: %1 Source file
:: %2 Target exe

:: Check if SDL is used
if not exist "%~dp2\sdl.dll" goto DoCheck
call :DoCheck %1 %~dp2\SDL.dll
goto :Fin

:DoCheck
setlocal

:: Check if target EXE uses DirectInput8
set dllname=dinput8.dll
find /I "%dllname%" %2>NUL
if "%ERRORLEVEL%" == "0" goto DoCopy

:: Check if target EXE uses DirectInput7 or older
set dllname=dinput.dll
find /I "%dllname%" %2>NUL
if "%ERRORLEVEL%" == "0" goto DoCopy

:NoSupported
echo Target EXE does not support dinput or dinput8
goto Fin

:DoCopy
echo Copying "%dllname%" for target "%2"
copy %1 "%~dp2%dllname%"

:Fin
endlocal
goto :eof
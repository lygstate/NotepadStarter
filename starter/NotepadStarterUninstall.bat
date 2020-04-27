::By using short path, to support place in Unicode Path
@echo off

::Gain Administrator permission
set SHOW_SUBWINDOW=0
call "%~dps0request-admin.bat" "%~dpnxs0" %*

call :RetrieveFileTime NotepadStarterTime "%SystemRoot%\NotepadStarter.exe"

call :RecoverNotepad "%SystemRoot%"
call :RecoverNotepad "%SystemRoot%\System32"
call :RecoverNotepad "%SystemRoot%\SysWOW64"

del /F /Q "%SystemRoot%\NotepadStarter.exe"

cd /d %~dps0..
if exist "NotepadStarter\.git" (goto skipGitRepository)
del /F /Q NotepadStarterPlugin.dll
:skipGitRepository

reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f
reg delete "HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f
reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /reg:64
reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /reg:32

if "%1"=="uninstall" goto uninstall_direct

rd /s /q NotepadStarter & pause

:uninstall_direct
rd /s /q NotepadStarter

goto :eof
:RecoverNotepad           -Passing the directory have notepad.exe who will be replaced
setlocal
@echo on
cd /d "%~1"
if not exist notepad.NotepadStarter.exe (goto recoverFile)
if not exist notepad.exe (goto recoverFile)

takeown /f notepad.exe
echo y | cacls notepad.exe /Grant Administrators:F

call :RetrieveFileTime CurrentFileTime notepad.exe
if "%NotepadStarterTime%" equ "%CurrentFileTime%" (goto recoverFile)
del /F /Q notepad.NotepadStarter.exe
:recoverFile
echo y | move /-Y notepad.NotepadStarter.exe notepad.exe
endlocal

goto :eof
:RetrieveFileTime    - Retrieve the file modification time %1 from the the file %2
if "%~1" EQU "" goto :eof
if "%~2" EQU "" goto :eof
setlocal
for %%a in ("%~2") do set "FileDate=%%~tza"
echo Calculating "%~2" with result:%FileDate%
endlocal & set "%~1=%FileDate%"

goto :eof

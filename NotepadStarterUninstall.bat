::通过使用短文件路径，支持放在Unicode路径下。
@echo off
cd /d %~dps0
set WD=%CD%
echo %WD%

:: --> Check for permissions
net session >nul 2>&1
:: --> If error flag not set, we do have administrator permission.
if %errorLevel% == 0 (
    goto gotAdmin
)

echo %~dpnxs0
call request-admin.bat %~dpnxs0 & exit /B

:gotAdmin
reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f 

:SYSTEMROOT
set NotepadFolder=%SYSTEMROOT%
set NEXT=SYSTEM32
goto RecoverNotepad

:SYSTEM32
set NotepadFolder=%SYSTEMROOT%\SYSTEM32
set NEXT=SysWOW64
goto RecoverNotepad

:SysWOW64
set NotepadFolder=%SYSTEMROOT%\SysWOW64
set NEXT=end
goto RecoverNotepad

:RecoverNotepad
if not exist %NotepadFolder%\notepad.NotepadStarter.exe (goto %NEXT%)
takeown /f %NotepadFolder%\notepad.exe
icacls %NotepadFolder%\notepad.exe /grant "%USERNAME%":f
del %NotepadFolder%\notepad.exe
echo errorlevel=%errorlevel%
if not %errorLevel% == 0 (
    goto %NEXT%
)
copy %NotepadFolder%\notepad.NotepadStarter.exe %NotepadFolder%\notepad.exe /y
call :date_to_number time1 "%NotepadFolder%\notepad.exe"
echo "%NotepadFolder%\notepad.exe"=%time1%
call :date_to_number time2 "%NotepadFolder%\notepad.NotepadStarter.exe"  
echo "%NotepadFolder%\notepad.NotepadStarter.exe"  =%time1%

if "%time1%" EQU "%time2%" ( 
  del "%NotepadFolder%\notepad.NotepadStarter.exe"
)
goto %NEXT% 

:end

pause

goto :eof
:date_to_number    - passing a variable by reference
if "%~1" EQU "" goto :eof
if "%~2" EQU "" goto :eof
setlocal
for %%a in ("%~2") do set "FileDate=%%~tza"
echo Calculating "%~2" %FileDate%
endlocal & set "%~1=%FileDate%"
goto:eof

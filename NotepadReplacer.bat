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

:SYSTEMROOT
set NotepadFolder=%SYSTEMROOT%
set NEXT=SYSTEM32
goto ReplaceNotepad;

:SYSTEM32
set NotepadFolder=%SYSTEMROOT%\SYSTEM32
set NEXT=SysWOW64
goto ReplaceNotepad;

:SysWOW64
set NotepadFolder=%SYSTEMROOT%\SysWOW64
set NEXT=end
goto ReplaceNotepad;

:ReplaceNotepad
if not exist %NotepadFolder%\notepad.exe (goto CopyNotepad)
takeown /f %NotepadFolder%\notepad.exe
icacls %NotepadFolder%\notepad.exe /grant "%USERNAME%":f
if exist %NotepadFolder%\notepad.NotepadStarter.exe (goto CopyNotepad)
copy %NotepadFolder%\notepad.exe %NotepadFolder%\notepad.NotepadStarter.exe
:CopyNotepad
copy %WD%\NotepadStarter.exe %NotepadFolder%\notepad.exe /y
goto %NEXT% 

:end
reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f 
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Notepad++" /t REG_SZ /d "%WD%"
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "readme" /t REG_SZ /d "call NotepadStarter.exe instead of original notepad.exe! To disable this option just remove notepad.exe entry"

pause
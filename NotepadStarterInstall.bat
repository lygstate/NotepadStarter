::通过使用短文件路径，支持放在Unicode路径下。
@echo on
echo Args:[%*]
if not "%1"=="ReplaceSystemNotepad" (
set UseImageFileExecution="UseImageFileExecution"
)

cd /d %~dps0
set WD=%CD%
echo %WD%
set "NotepadPlusPlus=%WD%\notepad++.exe"
if not exist "%NotepadPlusPlus%" ( goto NoNotepadPlusPlus )

:: --> Check for permissions
net session >nul 2>&1
:: --> If error flag not set, we do have administrator permission.
if %errorLevel% == 0 (
    goto gotAdmin
)

echo %~dpnxs0
call request-admin.bat %~dpnxs0 %* & exit /B

:gotAdmin
reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f 
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Notepad++" /t REG_SZ /d "%NotepadPlusPlus%"
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "readme" /t REG_SZ /d "call NotepadStarter.exe instead of original notepad.exe! To disable this option just remove notepad.exe entry"
del /F /Q %SystemRoot%\NotepadStarter.exe
copy %WD%\NotepadStarter.exe %SystemRoot%\NotepadStarter.exe /y
if defined UseImageFileExecution (
  reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%SystemRoot%\NotepadStarter.exe"
  goto finished
)

:SYSTEMROOT
set NotepadFolder=%SystemRoot%
set NEXT=SYSTEM32
goto ReplaceNotepad

:SYSTEM32
set NotepadFolder=%SystemRoot%\SYSTEM32
set NEXT=SysWOW64
goto ReplaceNotepad

:SysWOW64
set NotepadFolder=%SystemRoot%\SysWOW64
set NEXT=finished
goto ReplaceNotepad

:ReplaceNotepad
if not exist %NotepadFolder%\notepad.exe (goto CopyNotepad)
takeown /f %NotepadFolder%\notepad.exe
icacls %NotepadFolder%\notepad.exe /grant "%USERNAME%":f
if exist %NotepadFolder%\notepad.NotepadStarter.exe (goto CopyNotepad)
copy %NotepadFolder%\notepad.exe %NotepadFolder%\notepad.NotepadStarter.exe
:CopyNotepad
copy %WD%\NotepadStarter.exe %NotepadFolder%\notepad.exe /y
goto %NEXT% 

:finished
:NoNotepadPlusPlus
::pause
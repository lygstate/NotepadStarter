::测试Unicode的支持情况，将这个文件放在Unicode目录下执行
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
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%WD%\NotepadStarter.exe"
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "readme" /t REG_SZ /d "call NotepadStarter.exe instead of original notepad.exe! To disable this option just remove notepad.exe entry"

::pause
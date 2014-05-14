::测试Unicode的支持情况，将这个文件放在Unicode目录下执行

cd /d %~dp0

if defined ENABLE_ADMIN goto run_as_admin
set ENABLE_ADMIN=""
call request-admin.bat %~dpnx0
goto :finished

:run_as_admin
set WD=%CD%
reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f 
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%WD%\NotepadStarter.exe"
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "readme" /t REG_SZ /d "call NotepadStarter.exe instead of original notepad.exe! To disable this option just remove notepad.exe entry"

:finished

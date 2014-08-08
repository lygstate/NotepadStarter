::通过使用短文件路径，支持放在Unicode路径下。
@echo on
echo Args:[%*]
if not "%1"=="ReplaceSystemNotepad" (
set UseImageFileExecution="UseImageFileExecution"
)

cd /d %~dps0..\..
set "NotepadPlusPlus=%CD%\notepad++.exe"
cd /d %~dps0
set "NotepadStarter=%CD%\NotepadStarter.exe"
if not exist "%NotepadStarter%" ( goto NoNotepadPlusPlusOrNotepadStarter )
if not exist "%NotepadPlusPlus%" ( goto NoNotepadPlusPlusOrNotepadStarter )

::获取管理员权限
set SHOW_SUBWINDOW=0
call "%~dps0request-admin.bat" "%~dpnxs0" %*

reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f 
copy /Y %NotepadStarter% "%SystemRoot%\NotepadStarter.exe"
if defined UseImageFileExecution (
  reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%SystemRoot%\NotepadStarter.exe"
  goto PreppareRegistry
)

call :ReplaceNotepad "%SystemRoot%"
call :ReplaceNotepad "%SystemRoot%\System32"
call :ReplaceNotepad "%SystemRoot%\SysWOW64"

:PreppareRegistry
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Notepad++" /t REG_SZ /d "%NotepadPlusPlus%"
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "readme" /t REG_SZ /d "call NotepadStarter.exe instead of original notepad.exe! To disable this option just remove notepad.exe entry"

:NoNotepadPlusPlusOrNotepadStarter
::pause

goto :eof
:ReplaceNotepad           -Passing the directory have notepad.exe who will be replaced
setlocal
@echo on
cd /d "%~1"
if not exist "notepad.exe" (goto CopyNotepad)
takeown /f notepad.exe
echo Y | cacls notepad.exe /Grant Administrators:F
echo N | move /-Y notepad.exe notepad.NotepadStarter.exe
:CopyNotepad
copy /Y "%NotepadStarter%" notepad.exe
endlocal
goto :eof

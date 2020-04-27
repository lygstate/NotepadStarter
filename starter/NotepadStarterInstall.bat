::By using short path, to support place in Unicode Path
@echo on
echo Args:[%*]
if not "%1"=="ReplaceSystemNotepad" (
set UseImageFileExecution="UseImageFileExecution"
)

cd /d %~dps0..\..
set "NotepadPlusPlus=%CD%\notepad++.exe"

set "NotepadPlusPlusDir="
for /f "tokens=2,*" %%a in ('reg query HKEY_LOCAL_MACHINE\SOFTWARE\Notepad++ /ve 2^>NUL') do set "NotepadPlusPlusDir=%%b"

if exist "%NotepadPlusPlusDir%" goto find_notepad_dir

for /f "tokens=2,*" %%a in ('reg query HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Notepad++ /ve 2^>NUL') do set "NotepadPlusPlusDir=%%b"

:find_notepad_dir

if not exist "%NotepadPlusPlus%" (
  if exist "%NotepadPlusPlusDir%" (
    set "NotepadPlusPlus=%NotepadPlusPlusDir%\notepad++.exe"
  )
)

cd /d %~dps0
set "NotepadStarter=%CD%\NotepadStarter.exe"
if not exist "%NotepadStarter%" ( goto NoNotepadPlusPlusOrNotepadStarter )
if not exist "%NotepadPlusPlus%" ( goto NoNotepadPlusPlusOrNotepadStarter )

::Gain Administrator permission
set SHOW_SUBWINDOW=0
call "%~dps0request-admin.bat" "%~dpnxs0" %*

reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f
del /F "%WinDir%\NotepadStarter.exe"
copy /Y "%NotepadStarter%" "%WinDir%\NotepadStarter.exe"
if defined UseImageFileExecution (
  reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%WinDir%\NotepadStarter.exe"
  goto PreppareRegistry
)

call :ReplaceNotepad "%SystemRoot%"
call :ReplaceNotepad "%SystemRoot%\System32"
call :ReplaceNotepad "%SystemRoot%\SysWOW64"

:PreppareRegistry
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Notepad++" /t REG_SZ /d "%NotepadPlusPlus%"
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "readme" /t REG_SZ /d "call NotepadStarter.exe instead of original notepad.exe! To disable this option just remove notepad.exe entry"

:NoNotepadPlusPlusOrNotepadStarter
pause

goto :eof
:ReplaceNotepad           -Passing the directory have notepad.exe who will be replaced
setlocal
@echo on
cd /d "%~1"
if not exist "notepad.exe" (goto CopyNotepad)
takeown /f notepad.exe
echo y | cacls notepad.exe /Grant Administrators:F
del /F notepad.NotepadStarter.exe
rename notepad.exe notepad.NotepadStarter.exe
:CopyNotepad
copy /Y "%NotepadStarter%" notepad.exe
endlocal
goto :eof

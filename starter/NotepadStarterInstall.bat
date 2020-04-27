::By using short path, to support place in Unicode Path
@echo on
echo Args:[%*]

set "NotepadPlusPlusDir="

for /f "tokens=2,*" %%a in ('reg query HKLM\SOFTWARE\Notepad++ /ve 2^>NUL') do set "NotepadPlusPlusDir=%%b"
if exist "%NotepadPlusPlusDir%" goto found_notepad_dir

for /f "tokens=2,*" %%a in ('reg query HKLM\SOFTWARE\Wow6432Node\Notepad++ /ve 2^>NUL') do set "NotepadPlusPlusDir=%%b"
if exist "%NotepadPlusPlusDir%" goto found_notepad_dir

for /f "tokens=2,*" %%a in ('reg query HKLM\SOFTWARE\Notepad++ /ve /reg:64 2^>NUL') do set "NotepadPlusPlusDir=%%b"
if exist "%NotepadPlusPlusDir%" goto found_notepad_dir

for /f "tokens=2,*" %%a in ('reg query HKLM\SOFTWARE\Notepad++ /ve /reg:32 2^>NUL') do set "NotepadPlusPlusDir=%%b"
if exist "%NotepadPlusPlusDir%" goto found_notepad_dir

:found_notepad_dir

cd /d %~dps0..\..
set "NotepadPlusPlus=%CD%\notepad++.exe"
if exist "%NotepadPlusPlus%" goto found_notepad_exe

cd /d %~dps0..\..\..
set "NotepadPlusPlus=%CD%\notepad++.exe"
if exist "%NotepadPlusPlus%" goto found_notepad_exe

set "NotepadPlusPlus=%NotepadPlusPlusDir%\notepad++.exe"

:found_notepad_exe

cd /d %~dps0
set "NotepadStarter=%CD%\NotepadStarter.exe"
if not exist "%NotepadStarter%" ( goto NoNotepadPlusPlusOrNotepadStarter )
if not exist "%NotepadPlusPlus%" ( goto NoNotepadPlusPlusOrNotepadStarter )

::Gain Administrator permission
set SHOW_SUBWINDOW=0
call "%~dps0request-admin.bat" "%~dpnxs0" %*

reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f
reg delete "HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f
reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /reg:64
reg delete "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /reg:32
del /F "%WinDir%\NotepadStarter.exe"
copy /Y "%NotepadStarter%" "%WinDir%\NotepadStarter.exe"
goto PreppareRegistry

call :ReplaceNotepad "%SystemRoot%"
call :ReplaceNotepad "%SystemRoot%\System32"
call :ReplaceNotepad "%SystemRoot%\SysWOW64"

:PreppareRegistry
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%WinDir%\NotepadStarter.exe"
reg add "HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%WinDir%\NotepadStarter.exe"
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%WinDir%\NotepadStarter.exe" /reg:64
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Debugger" /t REG_SZ /d "%WinDir%\NotepadStarter.exe" /reg:32

reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Notepad++" /t REG_SZ /d "%NotepadPlusPlus%"
reg add "HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Notepad++" /t REG_SZ /d "%NotepadPlusPlus%"
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Notepad++" /t REG_SZ /d "%NotepadPlusPlus%" /reg:64
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "Notepad++" /t REG_SZ /d "%NotepadPlusPlus%" /reg:32

reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\notepad.exe" /f /v "readme" /t REG_SZ /d "call NotepadStarter.exe instead of original notepad.exe! To disable this option just remove notepad.exe entry"

if "%1"=="install" goto :oef

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

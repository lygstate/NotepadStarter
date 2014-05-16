@echo on
echo %~dps0
cd /d %~dps0
call "%~dps0\NotepadStarterInstall.bat" ReplaceSystemNotepad

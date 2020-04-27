@echo off
cd /d %~dps0\..
echo Release with: %1 %2 %CD%
set "CONFIGURATION=%2"
rd /s /q bin
mkdir bin\plugins\NotepadStarter
copy /y starter\NotepadStarterInstall.bat bin\plugins\NotepadStarter\
copy /y starter\NotepadStarterUninstall.bat bin\plugins\NotepadStarter\
copy /y starter\NotepadStarterReplacer.bat bin\plugins\NotepadStarter\
copy /y starter\request-admin.bat bin\plugins\NotepadStarter\
copy /y readme.md  bin\plugins\NotepadStarter\
if "%1" == "x86" (
  copy /y %CONFIGURATION%\NotepadStarter.exe bin\plugins\NotepadStarter\
  copy /y %CONFIGURATION%\NotepadStarterPlugin.dll bin\plugins\
) else (
  copy /y x64\%CONFIGURATION%\NotepadStarter.exe bin\plugins\NotepadStarter\
  copy /y x64\%CONFIGURATION%\NotepadStarterPlugin.dll bin\plugins\
)
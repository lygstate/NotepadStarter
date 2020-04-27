@echo off
cd /d %~dps0\..
echo Release with: %1 %2 %CD%
set "CONFIGURATION=%2"
rd /s /q bin
set ReleaseDir=bin\%1\%2\plugins
mkdir %ReleaseDir%\NotepadStarter
copy /y starter\NotepadStarterInstall.bat %ReleaseDir%\NotepadStarter\
copy /y starter\NotepadStarterUninstall.bat %ReleaseDir%\NotepadStarter\
copy /y starter\NotepadStarterReplacer.bat %ReleaseDir%\NotepadStarter\
copy /y starter\request-admin.bat %ReleaseDir%\NotepadStarter\
copy /y readme.md  %ReleaseDir%\NotepadStarter\
if "%1" == "x86" (
  copy /y %CONFIGURATION%\NotepadStarter.exe %ReleaseDir%\NotepadStarter\
  copy /y %CONFIGURATION%\NotepadStarterPlugin.dll %ReleaseDir%\
) else (
  copy /y x64\%CONFIGURATION%\NotepadStarter.exe %ReleaseDir%\NotepadStarter\
  copy /y x64\%CONFIGURATION%\NotepadStarterPlugin.dll %ReleaseDir%\
)
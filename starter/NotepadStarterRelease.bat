@echo off
cd /d %~dps0\..
echo Release with: %1 %2 %CD%
set "CONFIGURATION=%2"
set ReleaseDir=bin\%1\%2\plugins
rd /s /q %ReleaseDir%
mkdir %ReleaseDir%\NotepadStarter
copy /y starter\NotepadStarterInstall.bat %ReleaseDir%\NotepadStarter\
copy /y starter\NotepadStarterUninstall.bat %ReleaseDir%\NotepadStarter\
copy /y starter\NotepadStarterReplacer.bat %ReleaseDir%\NotepadStarter\
copy /y starter\request-admin.bat %ReleaseDir%\NotepadStarter\
copy /y readme.md  %ReleaseDir%\NotepadStarter\

copy /y %1\%CONFIGURATION%\NotepadStarter.exe %ReleaseDir%\NotepadStarter\
copy /y %1\%CONFIGURATION%\NotepadStarterPlugin.dll %ReleaseDir%\

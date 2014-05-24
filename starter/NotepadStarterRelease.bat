@echo off
cd /d %~dps0\..
copy /y starter\NotepadStarterInstall.bat bin\plugins\NotepadStarter\
copy /y starter\NotepadStarterUninstall.bat bin\plugins\NotepadStarter\
copy /y starter\NotepadStarterReplacer.bat bin\plugins\NotepadStarter\
copy /y starter\request-admin.bat bin\plugins\NotepadStarter\
copy /y readme.md  bin\plugins\NotepadStarter\

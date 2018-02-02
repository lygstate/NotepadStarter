@echo off
cd /d %~dps0\..
copy /y starter\NotepadStarterInstall.bat bin64\plugins\NotepadStarter\
copy /y starter\NotepadStarterUninstall.bat bin64\plugins\NotepadStarter\
copy /y starter\NotepadStarterReplacer.bat bin64\plugins\NotepadStarter\
copy /y starter\request-admin.bat bin64\plugins\NotepadStarter\
copy /y readme.md  bin64\plugins\NotepadStarter\

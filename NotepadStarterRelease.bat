@echo off
cd /d %~dps0
copy /y NotepadStarterInstall.bat bin\plugins\NotepadStarter\
copy /y NotepadStarterUninstall.bat bin\plugins\NotepadStarter\
copy /y NotepadStarterReplacer.bat bin\plugins\NotepadStarter\
copy /y request-admin.bat bin\plugins\NotepadStarter\
copy /y readme.md  bin\plugins\NotepadStarter\
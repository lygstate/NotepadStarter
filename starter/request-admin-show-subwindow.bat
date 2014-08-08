@echo off
echo PARAMS: %PARAMS%
echo %~dps0
cd /d %~dps0

set SHOW_SUBWINDOW=1

call %~dps0request-admin.bat %~s0 abc GH "G H"

:gotAdmin
echo Got admin
pause
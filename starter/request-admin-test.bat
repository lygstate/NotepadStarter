@echo off
echo PARAMS: %PARAMS%
echo %~dps0
cd /d %~dps0

:: --> Check for permissions
net session >nul 2>&1
:: --> If error flag not set, we do have administrator permission.
if %errorLevel% == 0 (
    goto gotAdmin
)

call %~dps0request-admin.bat %~s0 abc GH "G H"

:gotAdmin
echo Has admin
pause
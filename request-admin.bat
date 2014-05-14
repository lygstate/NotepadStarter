@echo off
set PARAMS=%2 %3 %4 %5 %6 %7 %8 %9 %SHIFT%

:: BatchGotAdmin
:-------------------------------------
:: --> Check for permissions
net session >nul 2>&1
:: --> If error flag not set, we do have administrator permission.
if %errorLevel% == 0 (
    goto gotAdmin
)

:UACPrompt
    echo Requesting administrative privileges...
    echo Set oShell = CreateObject^("WScript.Shell"^) > "%temp%\getadmin.vbs"
    echo param = oShell.ExpandEnvironmentStrings^("%%PARAMS%%"^) >> "%temp%\getadmin.vbs"
    echo Const Q="""" >> "%temp%\getadmin.vbs"
    echo param = "/c " ^& Q^& Q^&"%~1"^&Q ^& " " ^& param ^&Q >> "%temp%\getadmin.vbs"
    ::echo WScript.Echo param >> "%temp%\getadmin.vbs"
    echo Set UAC = CreateObject^("Shell.Application"^) >> "%temp%\getadmin.vbs"
    echo UAC.ShellExecute "cmd", param, "", "runas", 1 >> "%temp%\getadmin.vbs"
    call "%temp%\getadmin.vbs" & del "%temp%\getadmin.vbs" & exit /B

:gotAdmin
    echo Directly running with administrative privileges...

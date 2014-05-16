@echo off
set PARAMS=%2 %3 %4 %5 %6 %7 %8 %9 %SHIFT%

:: BatchGotAdmin
:-------------------------------------
:: --> Check for permissions
net session >nul 2>&1
:: --> If error flag not set, we do have administrator permission.
if %errorLevel% == 0 (
    goto hasAdmin
)

:requestAdmin
echo Requesting administrative privileges...
:: 1表示显示子窗体，0表示隐藏子窗体 TODO: lygstate 使得子窗体属性跟父窗体属性一致
:: 参见 http://ss64.com/vb/shellexecute.html (normal=1, hide=0)
set SHOW_SUBWINDOW=1
echo Set oShell = CreateObject^("WScript.Shell"^) > "%temp%\getadmin.vbs"
echo param = oShell.ExpandEnvironmentStrings^("%%PARAMS%%"^) >> "%temp%\getadmin.vbs"
echo Const Q="""" >> "%temp%\getadmin.vbs"
echo param = "/c " ^& Q^& Q^&"%~1"^&Q ^& " " ^& param ^&Q >> "%temp%\getadmin.vbs"
::echo WScript.Echo param >> "%temp%\getadmin.vbs"
echo Set UAC = CreateObject^("Shell.Application"^) >> "%temp%\getadmin.vbs"
echo UAC.ShellExecute "cmd", param, "", "runas", SHOW_SUBWINDOW >> "%temp%\getadmin.vbs"
call "%temp%\getadmin.vbs" & del "%temp%\getadmin.vbs" & exit /B

:hasAdmin
echo Directly running with administrative privileges, just running through

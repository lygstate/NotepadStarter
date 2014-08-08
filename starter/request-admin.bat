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
:: Reference to  http://ss64.com/vb/shellexecute.html (normal=1, hide=0)
:: 1 means show the window of the called scripts
:: 0 means hidden the window of the called scripts
:: We use the environment variable SHOW_SUBWINDOW([0,1]) to decide the if the
:: called scripts need to hidden the window.
if not defined SHOW_SUBWINDOW ( set SHOW_SUBWINDOW=1 )
echo Set oShell = CreateObject^("WScript.Shell"^) > "%temp%\getadmin.vbs"
echo param = oShell.ExpandEnvironmentStrings^("%%PARAMS%%"^) >> "%temp%\getadmin.vbs"
echo Const Q="""" >> "%temp%\getadmin.vbs"
echo param = "/c " ^& Q^& Q^&"%~1"^&Q ^& " " ^& param ^&Q >> "%temp%\getadmin.vbs"
::echo WScript.Echo param >> "%temp%\getadmin.vbs"
echo Set UAC = CreateObject^("Shell.Application"^) >> "%temp%\getadmin.vbs"
echo UAC.ShellExecute "cmd", param, "", "runas", %SHOW_SUBWINDOW% >> "%temp%\getadmin.vbs"
call "%temp%\getadmin.vbs"

@echo off
:: Use the following codes can exit the parent(caller) bat(script) file
call :___halt1
exit /b
:___halt1
call :___halt12 2> nul
exit /b
:___halt12
::pause
()

:hasAdmin
echo Directly running with administrative privileges, just running through

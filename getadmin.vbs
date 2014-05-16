Set oShell = CreateObject("WScript.Shell") 
param = oShell.ExpandEnvironmentStrings("%PARAMS%") 
Const Q ="""" 
param = "/c " & Q& Q&"request-admin.bat"&Q & " " & param &Q
WScript.Echo param
Set UAC = CreateObject("Shell.Application")
UAC.ShellExecute "cmd", param, "", "runas", 1 

## Introduction
This tool designed as a Notepad++ plugin by [Yonggang Luo](luoyonggang(at)gmail.com), when
it installed as a Notepad++ plugin, it's will automatically replace the system default notepad.exe
application with Notepad++ (Without need for removing anything from the windows system.).
It's tested under Windows 7, but Windows XP should also works.

This tool is based on the sources from [npplauncher](http://superstepho.free.fr/)
by [Stepho,2005] and [npplauncher at sf](http://sourceforge.net/projects/npplauncher/)
by [Mattes H. Mattesh,2013](mattesh(at)gmx.net).

## Design
NotepadStarter make use of a debugger feature in Windows the system will call a hooked 
process with appended parameters to allow debugging the intended application.
This hook application will be call whenever the correct application was resolved.
  
notepad.exe receives always only one parameter which is now just deferred to Notepad++.
Because notepad.exe is a blocking executable, so NotepadStarter behaves blocking as notepad.exe.
Notepad++ have multiple tab page, so NotepadStarter will terminated when the corresponding
tab page closed or the corresponding Notepad++ application is closed.

## Release
Use [Plugin Manager](http://www.brotherstone.co.uk/npp/pm/admin/welcome) to release NotepadStarter

## Contributions
Comments, issues and contributions can be done at [Github|NotepadStarter](https://github.com/lygstate/notepadstarter)

## NotepadStarter.exe parameters
* NotepadStarter.exe :install-registry
  Install the NotepadStarter as `notepad.exe Image File Execution`
* NotepadStarter.exe :install-replace
  Replace the notepad.exe under `%WinDir%`, `%WinDir%\System32` and `%WinDir%\SysWOW64` with NotepadStarter.exe
* NotepadStarter.exe :uninstall
  Uninstall NotepadStarter
* NotepadStarter.exe valid-windows-file-path
  Open the file valid-windows-file-path by notepad++ with blocking mode

## Installation
Downloading the NotepadStarter-major.minor.maintenance.revision.zip from [NotepadStarter releases](https://github.com/lygstate/NotepadStarter/releases)

### For newest version of notepad++
Create directory ${Notepad++InstallDir}\plugins\
Extract the content of downloaded zip file into ${Notepad++InstallDir}\plugins\NotepadStarterPlugin, running Notepad++, then it's will prompt a window to installing the NotepadStarter.
### For old version of notepad++
Extract the content of downloaded zip file into ${Notepad++InstallDir}\plugins, running Notepad++, then it's will prompt a window to installing the NotepadStarter.

## Uninstallation
Running the script ${Notepad++InstallDir}\plugins\NotepadStarter\NotepadStarterUninstall.bat

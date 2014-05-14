/* -------------------------------------
This file is part of AnalysePlugin for NotePad++ 
Copyright (C)2011 Matthias H. mattesh(at)gmx.net

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
------------------------------------- */

#ifndef NOTEPAD_DEBUG_H
#define NOTEPAD_DEBUG_H
#if _DEBUG
// use MDBG_COMP to define a compnent of file identifier which becomes prepended to all output
#ifndef MDBG_COMP
#define MDBG_COMP ""
#endif

#include <crtdbg.h>
#define DBG0(msg)                               _RPT1(_CRT_WARN, MDBG_COMP##msg##"\t"##__FILE__##"(%d)\n", __LINE__)
#define DBG1(msg, arg1)                         _RPT2(_CRT_WARN, MDBG_COMP##msg##"\t"##__FILE__##"(%d)\n", arg1, __LINE__)
#define DBG2(msg, arg1, arg2)                   _RPT3(_CRT_WARN, MDBG_COMP##msg##"\t"##__FILE__##"(%d)\n", arg1, arg2, __LINE__)
#define DBG3(msg, arg1, arg2, arg3)             _RPT4(_CRT_WARN, MDBG_COMP##msg##"\t"##__FILE__##"(%d)\n", arg1, arg2, arg3, __LINE__)
#define DBG4(msg, arg1, arg2, arg3, arg4)       _RPT5(_CRT_WARN, MDBG_COMP##msg##"\t"##__FILE__##"(%d)\n", arg1, arg2, arg3, arg4, __LINE__)
//#define DBG5(msg, arg1, arg2, arg3, arg4, arg5) _RPT6(_CRT_WARN, msg##__FILE__##"("##__LINE__##")\n", arg1, arg2, arg3, arg4, arg5)
#define DBGW0(msg)                               _RPTW1(_CRT_WARN, TEXT(MDBG_COMP)##L##msg##L"\t"##TEXT(__FILE__)##L"(%d)\n", __LINE__)
#define DBGW1(msg, arg1)                         _RPTW2(_CRT_WARN, TEXT(MDBG_COMP)##L##msg##L"\t"##TEXT(__FILE__)##L"(%d)\n", arg1, __LINE__)
#define DBGW2(msg, arg1, arg2)                   _RPTW3(_CRT_WARN, TEXT(MDBG_COMP)##L##msg##L"\t"##TEXT(__FILE__)##L"(%d)\n", arg1, arg2, __LINE__)
#define DBGW3(msg, arg1, arg2, arg3)             _RPTW4(_CRT_WARN, TEXT(MDBG_COMP)##L##msg##L"\t"##TEXT(__FILE__)##L"(%d)\n", arg1, arg2, arg3, __LINE__)
#define DBGW4(msg, arg1, arg2, arg3, arg4)       _RPTW5(_CRT_WARN, TEXT(MDBG_COMP)##L##msg##L"\t"##TEXT(__FILE__)##L"(%d)\n", arg1, arg2, arg3, arg4, __LINE__)
#else
#define DBG0(msg)                               
#define DBG1(msg, arg1)                         
#define DBG2(msg, arg1, arg2)                   
#define DBG3(msg, arg1, arg2, arg3)             
#define DBG4(msg, arg1, arg2, arg3, arg4)       
#define DBG5(msg, arg1, arg2, arg3, arg4, arg5) 
#define DBGW0(msg)                               
#define DBGW1(msg, arg1)                         
#define DBGW2(msg, arg1, arg2)                   
#define DBGW3(msg, arg1, arg2, arg3)             
#define DBGW4(msg, arg1, arg2, arg3, arg4)       
#define DBGW5(msg, arg1, arg2, arg3, arg4, arg5) 
#endif
#endif
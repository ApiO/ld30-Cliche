@echo off
call ..\pgtech\platform\win32\build\Debug\compile_server.exe cfg: ..\pgtech\platform\win32\build\Debug\data sch:  ..\pgtech\platform\win32\build\Debug\schemas src: resources data: data -l
pause
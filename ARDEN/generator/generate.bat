@echo off
set ROOT_PATH=%~dp0
set LUA_PATH=%ROOT_PATH:~0,-1%\?.lua;%ROOT_PATH:~0,-1%\generator\?.lua

moon "%ROOT_PATH:~0,-1%\generator\%1\Main.moon" %2

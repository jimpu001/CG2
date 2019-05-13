setlocal
cd /d %~dp0

zip\zip.exe -r abgabe.zip *.cpp *.h *.sln *.pdf *.txt *.vcxproj Makefile *.pro data/shaders

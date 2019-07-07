#!/bin/bash

# move to the projects directory
my_dir="`dirname \"$0\"`"
cd $my_dir

# zip all relevant files
zip -r abgabe.zip *.cpp *.h *.sln *.pdf *.txt *.vcxproj Makefile *.pro data/shaders

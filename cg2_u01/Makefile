# Makefile for unix systems
# this requires GNU make

APPNAME_HELLOCUBE=CG2Application

# Use pkg-config to search installed libraries
USE_PKGCONFIG=1

# Compiler flags

# enable all warnings in general, disable some
# (we initially provide a rough skeleton of the framework,
# so some unused variables might be present, which should not be
# considered as a problem)
WARNFLAGS= -Wall -Wextra \
	   -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function \
	   -Wno-unused-parameter -Wno-unknown-pragmas
# base flags, used for all builds
BASECXXFLAGS = -std=c++11 # we use some C++ 11 constructs in the code ...

# optimize flags, only used for RELEASE=1 builds
# for OpenMP, you might want to add -fopenmp
OPTIMIZEFLAGS = -ffast-math -mtune=native -march=native -O4 -DNDEBUG

ifeq ($(RELEASE), 1)
#
CFLAGS =   $(BASECFLAGS) $(WARNFLAGS) $(OPTIMIZEFLAGS) -s
CXXFLAGS = $(BASECXXFLAGS) $(WARNFLAGS) $(OPTIMIZEFLAGS) -s
else
CFLAGS =   $(BASECFLAGS) $(WARNFLAGS) -g
CXXFLAGS = $(BASECXXFLAGS) $(WARNFLAGS) -g
endif

# OpenGL Libraries
CPPFLAGS += -Iglad/include -I./
LINK_GL =

ifeq ($(USE_PKGCONFIG),1)
# Try to find the system's GLFW3 library via pkg-config

ifeq ($(shell pkg-config --atleast-version=3 glfw3 && echo 1 || echo 0),1)
CPPFLAGS += $(shell pkg-config --cflags glfw3)
LINK_GL += $(shell pkg-config --static --libs glfw3)
else
$(error GLFW3 library not found via pkg-config, please install it, or try using the local one by running make USE_PKGCONFIG=0)
endif

else
# use the local version of GLFW3
CPPFLAGS += -IGLFW/include 
LINK_GL += -L GLFW/lib-linux-x86_64 -lglfw3 -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor
endif

# all needed libraries
LINK = $(LINK_GL) -lm -lrt -ldl

# Files

# "shared" files are all files needed both for the uebung and the project
CFILES_SHARED=glad/src/glad.c 
CPPFILES_SHARED=glfw_backend.cpp  util.cpp 

INCFILES_SHARED=$(wildcard *.h)
SRCFILES_SHARED=$(CFILES_SHARED) $(CPPFILES_SHARED)
OBJECTS_SHARED =$(patsubst %.cpp,%.o,$(CPPFILES_SHARED)) $(patsubst %.c,%.o,$(CFILES_SHARED))
PRJFILES_SHARED=Makefile $(wildcard *.sln)

CPPFILES_HELLOCUBE=cg2application.cpp
SRCFILES_HELLOCUBE=$(CPPFILES_HELLOCUBE)
OBJECTS_HELLOCUBE =$(patsubst %.cpp,%.o,$(CPPFILES_HELLOCUBE))
PRJFILES_HELLOCUBE=CG2Application.vcxproj

# build rules
.PHONY: all
all:	$(APPNAME_HELLOCUBE)

# build and start with "make run"
.PHONY: run
run:	$(APPNAME_HELLOCUBE)
	__GL_SYNC_TO_VBLANK=1 ./$(APPNAME_HELLOCUBE)

# automatic dependency generation
# create $(DEPDIR) (and an empty file dir)
# create a .d file for every .c/.cpp source file which contains
# 		   all dependencies for that file
.PHONY: depend
depend:	$(DEPDIR)/dependencies
DEPDIR   = ./dep
DEPFILES = $(patsubst %.c,$(DEPDIR)/%.d,$(CFILES_SHARED)) $(patsubst %.cpp,$(DEPDIR)/%.d,$(CPPFILES_SHARED) $(CPPFILES_HELLOCUBE))
$(DEPDIR)/dependencies: $(DEPDIR)/dir $(DEPFILES)
	@cat $(DEPFILES) > $(DEPDIR)/dependencies
$(DEPDIR)/dir:
	@mkdir -p $(DEPDIR)
	@mkdir -p $(DEPDIR)/glad/src
	@mkdir -p $(DEPDIR)/vd
	@touch $(DEPDIR)/dir
$(DEPDIR)/%.d: %.c $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	@set -e; $(CC) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
$(DEPDIR)/%.d: %.cpp $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	@set -e; $(CXX) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
-include $(DEPDIR)/dependencies

# rules to build applications
$(APPNAME_HELLOCUBE): $(OBJECTS_SHARED) $(OBJECTS_HELLOCUBE) $(DEPDIR)/dependencies
	$(CXX) $(CFLAGS) $(OBJECTS_SHARED) $(OBJECTS_HELLOCUBE) $(LDFLAGS) $(LINK) -o$(APPNAME_HELLOCUBE)

# remove all unneeded files
.PHONY: clean
clean:
	@echo removing binaries: $(APPNAME_HELLOCUBE)
	@rm -f $(APPNAME_HELLOCUBE)
	@echo removing object files: $(OBJECTS_SHARED) $(OBJECTS_HELLOCUBE)
	@rm -f $(OBJECTS_SHARED) $(OBJECTS_HELLOCUBE)
	@echo removing dependency files
	@rm -rf $(DEPDIR)
	@echo removing tags
	@rm -f tags

# update the tags file
.PHONY: TAGS
TAGS:	tags

tags:	$(SRCFILES_SHARED) $(SRCFILES_HELLOCUBE) $(INCFILES) 
	ctags $(SRCFILES_SHARED) $(SRCFILES_HELLOCUBE) $(INCFILES)

# look for 'TODO' in all relevant files
.PHONY: todo
todo:
	-egrep -in "TODO" $(SRCFILES_SHARED) $(SRCFILES_HELLOCUBE) $(INCFILES)


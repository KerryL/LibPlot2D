# makefile.inc (libPlot2D)
# This file contains all of the environment definitions
# common to each of the makefiles within the project. 
#

# Include directories that are not already on the path
# DO NOT include the -I prefix to these paths - it will
# be added automatically
INCDIRS_TEMP = \
	$(CURDIR)/include 

INCDIRS = $(addprefix -I,$(INCDIRS_TEMP))

# For 3rd party files - will be included as system files so warnings
# are ignored.  These macros must be defined in your bashrc file.
LIB_INCDIRS_TEMP = \
	$(EIGEN)

LIB_INCDIRS = $(addprefix -isystem,$(LIB_INCDIRS_TEMP))

# Library directories that are not already on the path
# DO NOT include the -L prefix to these paths - it will
# be added automatically
LIBDIRS_TEMP = \
	

LIBDIRS = $(addprefix -L,$(LIBDIRS_TEMP))

# Libraries to link against
# DO NOT include the -l prefix to these libraries - it
# will be added automatically
LIBS_TEMP = \
	

LIBS = $(addprefix -l,$(LIBS_TEMP))

# Static libraries to be build before the executable
# MUST be listed in order of dependence (i.e. first
# library must not be needed by other libraries and
# it must need information contained in the following
# libraries).
PSLIB = 

# Location to store pkg-config files
PKG_CONFIG_LOCATION = /usr/lib/pkgconfig
PKG_CONFIG_FILE = lp2d

# Compiler to use
CC = g++

# Archiver to use
AR = ar rcs
RANLIB = ranlib

# wxWidgets include paths as system paths
WX_CFLAGS:=$(shell wx-config --version=3.1 --debug=no --cppflags)
WX_CFLAGS_D:=$(shell wx-config --version=3.1 --debug=yes --cppflags)

# Compiler flags
CFLAGS = -Wall -Wextra $(LIB_INCDIRS) $(INCDIRS) `pkg-config --cflags glew, freetype2` -Wno-unused-local-typedefs -std=c++14
CFLAGS_RELEASE = $(CFLAGS) -O2 $(subst -I,-isystem,$(WX_CFLAGS))
CFLAGS_DEBUG = $(CFLAGS) -g $(subst -I,-isystem,$(WX_CFLAGS_D))

# Linker flags
LDFLAGS = $(LIBDIRS) $(LIBS) `pkg-config --libs gl,glew,freetype2`
LDFLAGS_DEBUG = $(LDFLAGS) `wx-config --version=3.1 --debug=no --libs all`
LDFLAGS_RELEASE = $(LDFLAGS) `wx-config --version=3.1 --debug=yes --libs all`

# Object file output directory
OBJDIR = $(CURDIR)/.obj/
OBJDIR_DEBUG = $(OBJDIR)debug/
OBJDIR_RELEASE = $(OBJDIR)release/

# Binary file output directory
BINDIR = $(CURDIR)/bin/

# Library output directory
LIBOUTDIR = $(CURDIR)/lib/

# Method for creating directories
MKDIR = mkdir -p

# Method for removing files
RM = rm -f

# Method for moving files
MV = mv

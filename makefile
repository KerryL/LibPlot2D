# makefile (libPlot2D)
#
# Include the common definitions
include makefile.inc

# Name of the executable to compile and link
TARGET = libPlot2d.lib
TARGET_DEBUG = libPlot2d_debug.lib

# Directories in which to search for source files
DIRS = \
	src/application \
	src/application/dataFiles \
	src/renderer \
	src/renderer/primitives \
	src/utilities \
	src/utilities/math \
	src/utilities/signals

# Source files
SRC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
VERSION_FILE = src/gitHash.cpp

# Object files
TEMP_OBJS_DEBUG = $(addprefix $(OBJDIR_DEBUG),$(SRC:.cpp=.o))
TEMP_OBJS_RELEASE = $(addprefix $(OBJDIR_RELEASE),$(SRC:.cpp=.o))
VERSION_FILE_OBJ_DEBUG = $(OBJDIR_DEBUG)$(VERSION_FILE:.cpp=.o)
VERSION_FILE_OBJ_RELEASE = $(OBJDIR_RELEASE)$(VERSION_FILE:.cpp=.o)
OBJS_DEBUG = $(filter-out $(VERSION_FILE_OBJ_DEBUG),$(TEMP_OBJS_DEBUG))
OBJS_RELEASE = $(filter-out $(VERSION_FILE_OBJ_RELEASE),$(TEMP_OBJS_RELEASE))
ALL_OBJS_DEBUG = $(OBJS_DEBUG) $(VERSION_FILE_OBJ_DEBUG)
ALL_OBJS_RELEASE = $(OBJS_RELEASE) $(VERSION_FILE_OBJ_RELEASE)

.PHONY: all debug clean version

all: $(TARGET)
debug: $(TARGET_DEBUG)

$(TARGET): $(OBJS_RELEASE) version_release
	$(MKDIR) $(BINDIR)
	$(CC) $(ALL_OBJS_RELEASE) $(LDFLAGS_RELEASE) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@

$(TARGET_DEBUG): $(OBJS_DEBUG) version_debug
	$(MKDIR) $(BINDIR)
	$(CC) $(ALL_OBJS_DEBUG) $(LDFLAGS_DEBUG) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@

$(OBJDIR_RELEASE)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

$(OBJDIR_DEBUG)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

version_debug:
	./getGitHash.sh
	$(MKDIR) $(dir $(VERSION_FILE_OBJ_DEBUG))
	$(CC) $(CFLAGS_DEBUG) -c $(VERSION_FILE) -o $(VERSION_FILE_OBJ_DEBUG)

version_release:
	./getGitHash.sh
	$(MKDIR) $(dir $(VERSION_FILE_OBJ_RELEASE))
	$(CC) $(CFLAGS_RELEASE) -c $(VERSION_FILE) -o $(VERSION_FILE_OBJ_RELEASE)

clean:
	$(RM) -r $(OBJDIR)
	$(RM) $(BINDIR)$(TARGET)
	$(RM) $(VERSION_FILE)

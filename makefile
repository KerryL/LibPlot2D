# makefile (DataPlotter)
#
# Include the common definitions
include makefile.inc

# Name of the executable to compile and link
TARGET = DataPlotter
TARGET_DEBUG = DataPlotterd

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
VERSION_FILE = src/application/gitHash.cpp
SRC += $(VERSION_FILE)

# Object files
OBJS_DEBUG = $(addprefix $(OBJDIR_DEBUG),$(SRC:.cpp=.o))
OBJS_RELEASE = $(addprefix $(OBJDIR_RELEASE),$(SRC:.cpp=.o))

.PHONY: all debug clean version
#.NOTPARALLEL:

all: $(TARGET)
debug: $(TARGET_DEBUG)

$(TARGET): $(OBJS_RELEASE)
	$(MKDIR) $(BINDIR)
	$(CC) $(OBJS_RELEASE) $(LDFLAGS_RELEASE) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(MKDIR) $(BINDIR)
	$(CC) $(OBJS_DEBUG) $(LDFLAGS_DEBUG) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@

$(OBJDIR_RELEASE)%.o: %.cpp version
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

$(OBJDIR_DEBUG)%.o: %.cpp version
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

version:
	./getGitHash.sh

clean:
	$(RM) -r $(OBJDIR)
	$(RM) $(BINDIR)$(TARGET)
	$(RM) $(VERSION_FILE)

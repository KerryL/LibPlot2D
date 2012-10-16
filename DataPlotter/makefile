# makefile (DataPlotter)
#
# Include the common definitions
include makefile.inc

# Name of the executable to compile and link
TARGET = DataPlotter

# Directories in which to search for source files
DIRS = \
	src/application \
	src/application/dataFiles \
	src/renderer \
	src/renderer/primitives \
	src/utilities \
	src/utilities/math \
	src/utilities/signals \
	src/utilities/signals/filters

# Source files
SRC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))

# Object files
OBJS = $(addprefix $(OBJDIR),$(SRC:.cpp=.o))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(MKDIR) $(BINDIR)
	$(CC) $(OBJS) $(LDFLAGS) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@

$(OBJDIR)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJDIR)
	$(RM) $(BINDIR)$(TARGET)

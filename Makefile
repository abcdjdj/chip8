SOURCES = chip8.cpp opcodes.cpp

# Default target, for now
TARGET = chip8-ncurses

ifeq ($(TARGET),chip8-ncurses)
	SOURCES += main_ncurses.cpp
endif

CXX = g++
LD = ld
CXXFLAGS = -Wall -O0

# 8XY shift hack
SHIFT_HACK = true

# Debugging
DEBUG = false

ifeq ($(DEBUG),true)
	CXXFLAGS += -g
endif

ifeq ($(SHIFT_HACK),true)
	CXXFLAGS += -DSHIFT_HACK
endif

ifeq ($(TARGET),chip8-ncurses)
	LDFLAGS = -lncursesw
endif

OBJS = $(SOURCES:.cpp=.o)

$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(LDFLAGS) -o $@.out

%.o : %.cpp
	$(CXX) $< -c $(CXXFLAGS) $(LDFLAGS) -o $@

.PHONY: clean
clean :
	rm -rf *.o *.out 

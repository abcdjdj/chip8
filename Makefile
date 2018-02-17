SOURCES = chip8.cpp

# Default target, for now
TARGET = chip8-ncurses

ifeq ($(TARGET),chip8-ncurses)
	SOURCES += main_ncurses.cpp
endif

CXX = g++
LD = ld
CXXFLAGS = -Wall -Werror -g -O0

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

SOURCES = chip8.cpp opcodes.cpp

# Target
TARGET = chip8-sfml

CXX = g++
LD = ld
CXXFLAGS = -Wall -Werror -O0

# 8XY shift hack
SHIFT_HACK = true

# FX55/65 I register hack (true = increment I register)
FX55_HACK = false

# Debugging
DEBUG = false

ifeq ($(DEBUG),true)
	CXXFLAGS += -g
endif

ifeq ($(SHIFT_HACK),true)
	CXXFLAGS += -DSHIFT_HACK
endif

ifeq ($(FX55_HACK),true)
	CXXFLAGS += -DFX55_HACK
endif


############# Target specific stuff ##############
ifeq ($(TARGET),chip8-ncurses)
	LDFLAGS = -lncursesw
	SOURCES += main_ncurses.cpp
endif
ifeq ($(TARGET),chip8-sfml)
	LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
	SOURCES += main_sfml.cpp
endif
#################################################

OBJS = $(SOURCES:.cpp=.o)

$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(LDFLAGS) -o $@.out

%.o : %.cpp
	$(CXX) $< -c $(CXXFLAGS) $(LDFLAGS) -o $@

.PHONY: clean
clean :
	rm -rf *.o *.out 

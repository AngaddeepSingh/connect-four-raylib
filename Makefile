# Connect Four - Makefile
#
#   make          build the game
#   make run      build and run it
#   make clean    delete the build files
#
# If the compiler can't find raylib.h, point RAYLIB_PATH at raylib's "src" folder:
#   make RAYLIB_PATH=C:/raylib/raylib/src
#
# On Windows the command may be called mingw32-make instead of make.

CC     = gcc
TARGET = connect4
SRC    = main.c connect.c cabinet.c
OBJ    = $(SRC:.c=.o)
CFLAGS = -std=c99 -Wall -Wextra -O2

ifeq ($(OS),Windows_NT)
    # ---------- Windows (MinGW / w64devkit) ----------
    EXE         = .exe
    RAYLIB_PATH ?= C:/raylib/raylib/src
    LDLIBS      = -lraylib -lopengl32 -lgdi32 -lwinmm
    CLEAN_CMD   = del /Q /F *.o $(TARGET).exe 2>NUL
else
    EXE       =
    CLEAN_CMD = rm -f *.o $(TARGET)
    UNAME_S  := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        # ---------- macOS ----------
        LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else
        # ---------- Linux ----------
        LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif

# Only add these if RAYLIB_PATH is set (Windows default above, or passed on the command line)
ifdef RAYLIB_PATH
    CFLAGS  += -I$(RAYLIB_PATH)
    LDFLAGS += -L$(RAYLIB_PATH)
endif

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rebuild a file when a header it includes changes
main.o:    connect.h cabinet.h
connect.o: connect.h
cabinet.o: cabinet.h connect.h

run: all
	./$(TARGET)$(EXE)

clean:
	-$(CLEAN_CMD)

.PHONY: all run clean

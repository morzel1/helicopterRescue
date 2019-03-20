# Makefile for heligame master on windows (computer lab doesnt have scons)

MINGW = J:\MinGW

SDL = J:\MinGW\external_libs\SDL2-2.0.5\i686-w64-mingw32
IMG = J:\MinGW\external_libs\SDL2_image-2.0.1\i686-w64-mingw32
TTF = J:\MinGW\external_libs\SDL2_ttf-2.0.14\i686-w64-mingw32
MINIAT = J:\MinGW\external_libs\miniat\out

# sdl includes
INCDIRS  = -I$(SDL)\include\SDL
INCDIRS += -I$(SDL)\include\SDL2
INCDIRS += -I$(IMG)\include
INCDIRS += -I$(IMG)\include\SDL2
INCDIRS += -I$(TTF)\include
INCDIRS += -I$(TTF)\include\SDL2

# miniat includes
INCDIRS += -I$(MINIAT)\vm\inc
INCDIRS += -I$(MINIAT)\vm\inc\miniat

# lib dirs
LIBDIRS  = -L$(SDL)\lib
LIBDIRS += -L$(IMG)\lib
LIBDIRS += -L$(TTF)\lib
LIBDIRS += -L$(MINIAT)\lib

# libraries
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lminiat

CC = $(MINGW)\bin\g++
CFLAGS = -D_REENTRANT -std=c++11

all:
	$(CC) $(CFLAGS) *.cpp -o main.exe $(INCDIRS) $(LIBDIRS) $(LIBS)

clean:
	del *.bin

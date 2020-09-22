

ifeq	($(OS),Windows_NT)
#OBJS specifies which files to compile as part of the project
OBJS = main.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -IC:\MinGW\include\SDL2

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LC:\MinGW\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2	-lSDL2_image

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = main

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

#this second branch compiles for linux
else
CPP_FILES = $(wildcard *.cpp)
OUT_NAMES = $(patsubst %.cpp,%.o,$(CPP_FILES))
CC = clang++
COMPILER_FLAGS = -Wall -I/usr/include/SDL2
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

all: $(OUT_NAMES)

%.o: %.cpp
	$(CC) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@

clean:
	rm *.o
endif



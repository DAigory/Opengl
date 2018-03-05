CC=g++
CFLAGS=-std=c++0x  -D_DEBUG -ggdb3 -o0 -c  -Wall -pedantic  -lXi $(shell pkg-config --cflags glfw3 glew)
LDFLAGS= $(shell pkg-config --libs glfw3 glew)
LIBS = SOIL

SOURCES=main.cpp shader.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o  $@ -l$(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
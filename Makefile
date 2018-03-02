CC=g++
CFLAGS=-std=c++0x  -D_DEBUG -g -c -Wall -lXi $(shell pkg-config --cflags glfw3 glew)
LDFLAGS=$(shell pkg-config --libs glfw3 glew)
SOURCES=main.cpp shader.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
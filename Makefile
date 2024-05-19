# Makefile for compiling CritRed.cpp

# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++11

# Target executable
TARGET = CritRed

# Source files
SRCS = CritRed.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)

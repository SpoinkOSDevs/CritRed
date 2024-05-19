# Makefile for compiling CritRed.cpp

# Compiler
CC = g++


# Target executable
TARGET = CritRed

# Source files
SRCS = CritRed.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)

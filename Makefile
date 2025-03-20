# Compiler
CC = g++

# Project name
PRJ_NAME := chat_app

# Directories
INC_DIR = inc
SRC_DIR = src

#Source files
SOURCE = main.cpp $(wildcard $(SRC_DIR)/*.cpp)

# Compiler Flags
CFLAGS := -Wall  -lrt -I$(INC_DIR)

# Linker Flags
LFLAGS := -lpthread

# Object files
OBJ = $(SOURCE:.cpp=.o)

# Link oject files to executable
$(PRJ_NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

# Compile source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Default target
all: $(PRJ_NAME)

# Clean object files
clean:
	rm -f $(OBJ) $(PRJ_NAME)

.PHONY: all, clean
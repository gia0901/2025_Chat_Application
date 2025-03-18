.PHONY: all, clean

# Project name
PRJ_NAME := chat_app

# Directories
SRC_DIR := $(PWD)/src
INC_DIR := $(PWD)/inc

# Compiler
CC = g++

# Flags
CFLAGS := -Wall -lpthread -lrt

# Object


# Build
all:
	$(CC) main.cpp $(SRC_DIR)/*.cpp -I$(INC_DIR) $(CFLAGS) -o $(PRJ_NAME)

clean:
	rm -rf $(PRJ_NAME)
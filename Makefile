CC = gcc
CFLAGS = -Wall
LINKER_FLAGS = -lSDL2
OBJS = main.c
OBJ_NAME = csnake
all:
	$(CC) $(CFLAGS) $(OBJS) -o $(OBJ_NAME) $(LINKER_FLAGS)


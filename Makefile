CC = gcc

SRC = src/*.c

OUT = game

CFLAGS = -Iinclude 


LIBS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LIBS)


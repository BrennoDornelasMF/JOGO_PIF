CC = gcc

SRC = src/main.c src/road.c src/player.c src/camera.c src/sprite.c

OUT = game

CFLAGS = -Iinclude

LIBS = -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LIBS)


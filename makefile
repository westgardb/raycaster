CC = gcc
CFLAGS = -Wall -g
LFLAGS = -lSDL2 -lm
TARGET = main

default: $(TARGET).c
	$(CC) -o $(TARGET) $(TARGET).c $(CFLAGS) $(LFLAGS)
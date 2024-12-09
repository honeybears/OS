
CC=gcc
CFLAGS=-g -Wall -pthread
OBJ=main.c Init.c Scheduler.c Thread.c ThreadQueue.c TestCase1.c TestCase2.c TestCase3.c
TARGET=hw2

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

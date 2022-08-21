TARGET = run.exe
HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
LIBS =

CC = gcc
CFLAGS = -g -Wall -pthread

default: $(TARGET)
all: default

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)

.PHONY: $(TARGET) clean

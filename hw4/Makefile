CC = gcc
CFLAGS = -g -Wall -Wvla -fsanitize=address
LDFLAGS =
OBJFILES = mymalloc.o main.o
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
	mv $(TARGET) $(OBJFILES) build

clean:
	rm -f *.o $(OBJFILES) $(TARGET) *~

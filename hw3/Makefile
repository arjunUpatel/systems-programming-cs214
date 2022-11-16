CC = gcc
CFLAGS = -g -Wall -Wvla -fsanitize=address
LDFLAGS =
OBJFILES = parser.o process.o stack.o shell.o
TARGET = shell

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
	mv $(TARGET) $(OBJFILES) build

clean:
	rm -f *.o $(OBJFILES) $(TARGET) *~

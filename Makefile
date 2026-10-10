CC = gcc
CFLAGS = -Wall -Wextra
TARGET = mini_db.exe
SOURCE = src/main.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

clean:
	del /Q $(TARGET) 2>NUL

	
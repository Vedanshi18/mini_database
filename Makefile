CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude
TARGET = mini_db.exe
SOURCES = src/main.c src/student.c src/database.c src/query.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	-del /Q $(TARGET) 2>NUL
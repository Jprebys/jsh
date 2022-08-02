TARGET = jsh
SRC_DIR = src
LIBS = -lreadline
CC = gcc
CFLAGS = -g -Wall -Wextra

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard $(SRC_DIR)/*.c))
HEADERS = $(wildcard $(SRC_DIR)/*.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f $(SRC_DIR)/*.o
	-rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

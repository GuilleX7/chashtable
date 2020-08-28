CC=gcc
NAME=chashtable-test
TARGET_FOLDER=bin/
SOURCES=test/test.c src/hash_table.c

OBJ=$(SOURCES:.c=.o)
BIN=$(TARGET_FOLDER)$(NAME)

.PHONY = $(NAME) all clean

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(BIN)

$(OBJ): $(TARGET_FOLDER)
	$(CC) $(@:.o=.c) -c -o $@ -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -O3

$(TARGET_FOLDER):
	mkdir -p $(TARGET_FOLDER)

clean:
	rm -r $(TARGET_FOLDER)
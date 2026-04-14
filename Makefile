CC = gcc
CFLAGS = -Wall -Iinclude

SRC = src/main.c src/snake.c src/keyboard.c src/screen.c src/math.c
OBJ = $(SRC:.c=.o)
EXEC = snake_os

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

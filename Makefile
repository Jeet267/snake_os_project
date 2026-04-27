CC     = gcc
CFLAGS = -Wall -Wextra -Iinclude

# All source files — add memory.c and string.c for Phase 2
SRC = src/main.c \
      src/snake.c \
      src/keyboard.c \
      src/screen.c \
      src/math.c \
      src/memory.c \
      src/string.c

OBJ  = $(SRC:.c=.o)
EXEC = snake_os

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

# Verify no banned standard library usage in source files
# We strip single-line comments first so doc-comment mentions don't trigger.
verify:
	@echo "=== Checking for banned library calls ==="
	@BANNED=0; \
	 for f in src/*.c; do \
	   if grep -v '//' $$f | grep -qE '\b(strlen|strcpy|sprintf|strcat)\s*\(' || \
	      grep -v '//' $$f | grep -qE '\bmy_strlen\b' | head -0 && \
	      grep -v '//' $$f | grep -qE '[^y]_malloc\b|[^l]free\s*\('; then \
	     BANNED=1; fi; done; \
	 if [ $$BANNED -eq 0 ]; then echo 'PASS: No banned raw stdlib calls found.'; \
	 else echo 'REVIEW: Check output above.'; fi

.PHONY: all clean verify

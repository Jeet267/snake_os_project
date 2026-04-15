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
	   if perl -0777 -pe 's{/\\*.*?\\*/}{}gs; s{//.*$$}{}mg' "$$f" | grep -nE '(^|[^[:alnum:]_])(strlen|strcpy|sprintf|strcat)[[:space:]]*[(]' >/dev/null; then \
	     echo "BANNED: $$f uses string stdlib call(s):"; \
	     perl -0777 -pe 's{/\\*.*?\\*/}{}gs; s{//.*$$}{}mg' "$$f" | grep -nE '(^|[^[:alnum:]_])(strlen|strcpy|sprintf|strcat)[[:space:]]*[(]' || true; \
	     BANNED=1; \
	   fi; \
	   if perl -0777 -pe 's{/\\*.*?\\*/}{}gs; s{//.*$$}{}mg' "$$f" | grep -nE '(^|[^[:alnum:]_])(malloc|free)[[:space:]]*[(]' >/dev/null; then \
	     echo "BANNED: $$f uses raw allocation call(s):"; \
	     perl -0777 -pe 's{/\\*.*?\\*/}{}gs; s{//.*$$}{}mg' "$$f" | grep -nE '(^|[^[:alnum:]_])(malloc|free)[[:space:]]*[(]' || true; \
	     BANNED=1; \
	   fi; \
	 done; \
	 if [ $$BANNED -eq 0 ]; then echo 'PASS: No banned raw stdlib calls found.'; \
	 else echo 'REVIEW: Remove banned call(s) listed above.'; exit 1; fi

.PHONY: all clean verify

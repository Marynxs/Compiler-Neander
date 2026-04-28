CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = neander

SRC = main.c \
      src/lexer/lexer.c \
      src/parser/parser.c \
      src/assemblyGenerator/assemblyGenerator.c \
      src/assembler/assembler.c \
      src/neanderImplementer/neanderImplementer.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) saida.asm saida.mem

rebuild: clean all
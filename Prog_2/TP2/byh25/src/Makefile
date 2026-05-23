CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -g
SRCDIR  = .
BINDIR  = ../bin
TARGET  = $(BINDIR)/objeto

SRCS = objetos.c pgm.c deteccao.c
OBJS = $(SRCS:.c=.o)

tudo: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(TARGET)

limpa:
	rm -f $(OBJS)

faxina: limpa
	rm -f $(TARGET)

.PHONY: tudo limpa faxina

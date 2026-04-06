#Variáveis de compilação
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm
BIN_DIR = ../bin
EXEC = $(BIN_DIR)/juros

#lista de arquivos objeto
OBJS = juros.o investimento.o utils.o

#Regra principal
all: tudo

#Regra 'tudo': compila e move o executável para bin
tudo: $(OBJS)
	$(CC) $(OBJS) -o juros $(LDFLAGS)
	mkdir -p $(BIN_DIR)
	mv juros $(EXEC)

#Regra genérica para compilar .c em .o
%.o: $.c
	$(CC) $(CFLAGS) -c $< -o $@

#Regra 'limpa': remove arquivos temporários
limpa:
	rm -f *.o 

#Regra 'faxina': limpa tudo, inclusive executável
faxina: limpa
	rm -f $(EXEC)
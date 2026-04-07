#Variáveis de compilação
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm
BIN_DIR = ../bin
EXEC = $(BIN_DIR)/juros

#lista de arquivos objeto
OBJS = juros.o investimento.o config.o

#Regra principal
all: tudo

#Regra 'tudo': compila e move o executável para bin
tudo: $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

#Regra de teste de memória
teste: tudo
	valgrind --leak-check=full ../bin/juros -c 1000 ../dolar.cot.txt ../selic.tax.txt

#Regra genérica para compilar .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Regra 'limpa': remove arquivos temporários
limpa:
	rm -f *.o 

#Regra 'faxina': limpa tudo, inclusive executável
faxina: limpa
	rm -f $(EXEC)
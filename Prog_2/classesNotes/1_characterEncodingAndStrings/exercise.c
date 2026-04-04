/*O formato CSV organiza dados de maneira tabular em texto simples,
onde cada linha de dados é uma string e a separação entre uma coluna
e outra é demarcada por um símbolo de vírgula (,).
Neste momento, para simplificar, vamos considerar que dados de uma
coluna não podem conter o símbolo de vírgula (,).
Faça um programa que recebe uma string de uma linha CSV e
apresente o valor de cada coluna separadamente. Numere as colunas
por ordem de leitura.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
    char string[1000], *separador = ",", *substring;
    int colunas = 1;
    
    fgets(string, sizeof(string), stdin);

    substring = strtok(string, separador);
    while (substring != NULL) {
        printf("Coluna %d: %s\n", colunas, substring);
        colunas++;

        substring = strtok(NULL, separador);
    }
}
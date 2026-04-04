/*
Considere o arquivo fornecido junto com o material desta aula para a
realização de testes. Este arquivo contém um texto em inglês.
Desenvolva um programa que receba o nome, abra um arquivo texto e
faça uma varredura em todas as suas palavras, satisfazendo o seguinte
requisito para apresentar o resultado:
● Identifique e conte as ocorrências da palavra mais frequente
    ○ Em empates, retorne qualquer uma das palavras mais
    frequentes
    ○ Nenhuma palavra terá mais do que 100 caracteres
    ○ Não se preocupe com pontuações
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
    FILE *arquivo = fopen("archiveForExercise", "r");
    if (!arquivo)
        return 1;

    char palavras[1000][101], palavra[101];
    int contagem[1000], total = 0;

    while (fscanf(arquivo, "%100s", palavra) == 1){
        int encontrada = 0;

        for (int i = 0; i < total; i++) {
            if (strcmp(palavras[i], palavra) == 0){
                contagem[i]++;
                encontrada = 1;
                break;
            }
        }

        if (!encontrada) {
            strcpy(palavras[total], palavra);
            contagem[total] = 1;
            total++;
        }
    }

    fclose(arquivo);

    int max = 0, indice = 0;

    for (int i = 0; i < total; i++) {
        if (contagem[i] > max) {
            max = contagem[i];
            indice = i;
        }
    }

    printf("Palavra mais frequente: %s (%d vezes)\n", palavras[indice], max);

    return 0;
}
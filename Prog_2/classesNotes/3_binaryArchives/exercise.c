/*
Exercício
O arquivo binário file.bin contém uma mensagem codificada. O arquivo é composto por uma sequência de registros de tamanho fixo.
Cada registro possui 5 bytes, sendo:
• 4 bytes correspondentes a um inteiro pos
• 1 byte correspondente a um caractere ch
O valor de pos indica a posição na string final onde o caractere ch deve ser colocado.
Por exemplo, se um registro contém:
pos = 148
ch = 'a'
isso significa que o caractere 'a' deve ser colocado na posição 148 da string reconstruída.
O tamanho da string pode ser determinado a partir do tamanho do arquivo
O programa deve imprimir a string reconstruída. Se você conseguir ler o texto, parabéns, você decodificou a mensagem!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {
    FILE* arquivoBin = fopen("file.bin", "rb");

    fseek(arquivoBin, 0, SEEK_END);
    long tam = ftell(arquivoBin);
    rewind(arquivoBin);

    int numRegistros = tam/5;

    char string[numRegistros + 1];

    int i;
    for (i = 0; i < numRegistros; i++)
        string[i] = '-';

    string[numRegistros] = '\0';

    for(i = 0; i < numRegistros; i++){
        int pos;
        char ch;

        fread(&pos, sizeof(int), 1, arquivoBin);
        fread(&ch, sizeof(char), 1, arquivoBin);

        string[pos] = ch;
    }

    printf("%s\n", string);

    fclose(arquivoBin);
    
    return 0;
}
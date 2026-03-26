/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo do programa principal, que usa o TAD racional.
 * Feito em 16/09/2024 para a disciplina CI1001 - Programação 1.
*/

/* coloque aqui seus includes (primeiro os <...>, depois os "...") */
#include <stdio.h>
#include "racional.h"
#include <stdlib.h>

/* programa principal */
int main ()
{
    long n, max, i; //variáveis pedidas pelo enunciado
    struct racional r1, r2, //variáveis r1 e r2
                    soma, subtr, multip, divisao; //variáveis utilizadas para receberem o cálculo feito nas funções aritméticas

    srand (0); //semente de rand

    scanf ("%ld", &n); //ler n
    if ((n<1)||(n>99))
        return(1); //inválido
    scanf ("%ld", &max); //ler max
    if ((max<1)||(max>29)) 
        return(1); //inválido

    for (i=1; i<=n; i++){ //loop for
        printf("%ld: ", i);
        r1=sorteia_r(-max, max);
        r2=sorteia_r(-max, max); 
        
        imprime_r(r1);
        imprime_r(r2);

        if ((valido_r(r1)==0) || (valido_r(r2)==0)){
            printf("NUMERO INVALIDO \n");
            return(1);
        }  

        soma= soma_r(r1, r2);
        subtr= subtrai_r(r1,r2);
        multip= multiplica_r(r1, r2);
        divisao= divide_r(r1, r2);
        if (divisao.den==0){
            printf("DIVISAO INVALIDA \n");
            return(1);
        }
        else {
            imprime_r(soma);
            imprime_r(subtr);
            imprime_r(multip);
            imprime_r(divisao);
            printf("\n");
        }   
    }
    return(0);
}

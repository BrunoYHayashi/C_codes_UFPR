/* 
 * Tipos Abstratos de Dados - TADs
 * Arquivo do programa principal, que usa o TAD racional.
 * Feito em 09/09/2025 para a disciplina CI1001 - Programação 1.
*/

/* coloque aqui seus includes (primeiro os <...>, depois os "...") */
#include <stdio.h>
#include <stdlib.h>
#include "racional.h"
/* coloque aqui as funções auxiliares que precisar neste arquivo */
static void alocaVetor (struct racional ***vetor, long n);
static void preencheVetor (struct racional **vetorR, long n);
static void imprimeVetor (struct racional **V, long n);
static void validaVetor (struct racional **V, long *max);
static void selectSort (struct racional **V, long n);
static struct racional *criaSoma (void);
static void somaVetorImpSoma (struct racional **V, long max, struct racional *r);
static void liberaRacionais (struct racional **V, long n);
static void liberaVetor (struct racional **V);
static void liberaSoma (struct racional *r);

/* programa principal */
int main ()
{
  long n;
  struct racional **vetorR, *soma;

  scanf ("%ld", &n); //lê n
  if ((n<1)||(n>99))
    return(1); //inválido

  alocaVetor (&vetorR, n);

  if (!vetorR)
    return (1); //falhou em alocar memória

  preencheVetor (vetorR, n); //preenche o vetor

  imprimeVetor (vetorR, n); //imprime vetor e os racionais apontados por ele

  validaVetor (vetorR, &n); //elimina os racionais inválidos
  imprimeVetor (vetorR, n);

  selectSort (vetorR, n); //ordena o vetor crescentemente
  imprimeVetor (vetorR, n);

  soma= criaSoma ();
  somaVetorImpSoma (vetorR, n, soma); //Soma os racionais apontados

  liberaRacionais (vetorR, n); //libera os racionais apontados
  imprimeVetor (vetorR, n); 

  liberaVetor (vetorR); //libera o vetor
  liberaSoma (soma); //libera o ponteiro da soma


  return 0;
}

static void alocaVetor (struct racional ***vetor, long n){
  *vetor = malloc ( (n+1)* sizeof (struct racional*)); //aloca vetor dinamicamente
}


static void preencheVetor (struct racional **vetorR, long n){
  int i;
  long numerador, denominador;
  for (i=1; i<=n; i++){
    scanf("%ld", &numerador);
    scanf("%ld", &denominador);
    vetorR[i] = cria_r(numerador, denominador);
  }
}

static void imprimeVetor (struct racional **V, long max){ //imprime o vetor e pula linha
  int i;
  printf("VETOR = ");
  for (i=1; i<=max; i++)
    imprime_r(V[i]);
  printf("\n");
}

static void validaVetor (struct racional **V, long *max){
  int i, j;
  i=1; //inicializa o contador do vetor inteiro em 1
  j=1; //inicializa o contador do vetor válido em 1

  while(i<=*max){ //enquanto i for menor ou igual que o número total de elementos do vetor inteiro faça:
    while ((i<=*max) && (!valido_r(V[i]))){ //enquanto i for menor ou igual ao n de elementos do vetor inteiro E V[i] for válido faça:
      i++; // pula inválidos
    }
    if (i<=*max){ //como i pode ter sido incrementado a ponto de ser maior que o número de elementos, pergunta de novo se ele ainda é menor ou igual a max
      V[j]=V[i]; // copia o válido mais próximo
      i++; //i continua de onde achou o válido mais próximo +1
      j++; //j avança um para preencher o próximo elemento do vetor
    }
  }

  *max = j-1; // atualiza quantidade de elementos válidos, usa-se j-1 pois após atualizar o último elemento, ele ainda incrementa 1 em j
} 

static void selectSort (struct racional **V, long max){ //selectSort
  int i, j, menor;
  struct racional *AUX;

  for (i=1; i<max; i++){ 
    menor= i; //supões que o menor índice é i
    for (j=i+1; j<=max; j++){ //verifica os outros elementos do vetor
      if ((compara_r(V[j], V[menor]))== -1){
        menor=j; //se existir um elemento do vetor menor do que o elemento de V[i], o menor índice vira ele (j)
      }
    }
    if (menor!=i){ //se menor não for = i, significa que achou outro menor, logo, faz as alterações
      AUX= V[i];
      V[i]= V[menor];
      V[menor]= AUX;
    }
  }
}

static struct racional *criaSoma (void){
  struct racional *soma;
  soma= cria_r(0, 1); //cria um racional para receber a soma, esse sendo "0/1" para não interferir na soma dos vetores
  return (soma);
}

static void somaVetorImpSoma (struct racional **V, long max, struct racional *r){ //soma os elementos do vetor e imprime a soma
  int i;
  
  for (i=1; i<=max; i++)
    soma_r(r, V[i], r); //soma todos os valores do vetor
  printf("SOMA = "); 
  imprime_r(r); //imprime a soma
  printf("\n"); 
}

static void liberaRacionais (struct racional **V, long n){
  long i;

  for (i=1; i<=n; i++){
    destroi_r(&V[i]);
  }
}

static void liberaVetor (struct racional **V){
  free (V);
}

static void liberaSoma (struct racional *r){
  free (r);
  r= NULL;
}
#ifndef DETECCAO_H
#define DETECCAO_H

#include "pgm.h"

#define MAX_OBJETOS 10000

typedef struct {
    int id;          /* numero do objeto (1-based) */
    int x;           /* coluna do primeiro pixel detectado */
    int y;           /* linha do primeiro pixel detectado */
    int area;        /* area em pixels */
    int cor;         /* label de cor para saida colorida (0-7) */
} Objeto;

/* Rotula pixels da imagem de entrada.
   rotulo[i] = 0 se branco, ou numero do objeto (1..n) se preto.
   Retorna numero de objetos encontrados. */
int detectar_objetos(const ImagemPGM *entrada, int *rotulo, Objeto *objetos);

/* Gera imagem de saida com objetos coloridos (PPM P6) */
void gerar_saida_colorida(const ImagemPGM *entrada, const int *rotulo,
                          const Objeto *objetos, int n_objetos,
                          const char *arquivo_saida);

/* Gera imagem com os 3 maiores objetos lado a lado */
void gerar_maiores(const ImagemPGM *entrada, const int *rotulo,
                   Objeto *objetos, int n_objetos,
                   const char *arquivo);

/* Imprime lista ordenada por area */
void imprimir_objetos(const Objeto *objetos, int n_objetos);

/* Atribui cores iguais a objetos com formas iguais */
void atribuir_cores(const ImagemPGM *entrada, const int *rotulo,
                    Objeto *objetos, int n_objetos);

/* Limiar: pixel com valor abaixo disso e considerado "preto" (objeto) */
#define LIMIAR 128

#endif

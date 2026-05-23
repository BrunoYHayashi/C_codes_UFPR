#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgm.h"
#include "deteccao.h"

static void uso(const char *prog)
{
    fprintf(stderr, "Uso: %s -i entrada.pgm -o saida.ppm [-a maiores.pgm]\n", prog);
    exit(1);
}

int main(int argc, char *argv[])
{
    char *arq_entrada = NULL;
    char *arq_saida   = NULL;
    char *arq_maiores = NULL;

    /* Analisa argumentos */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
            arq_entrada = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
            arq_saida = argv[++i];
        else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc)
            arq_maiores = argv[++i];
    }

    if (!arq_entrada || !arq_saida)
        uso(argv[0]);

    /* Le imagem de entrada */
    ImagemPGM *entrada = pgm_ler(arq_entrada);
    if (!entrada) {
        fprintf(stderr, "Erro ao ler imagem de entrada\n");
        return 1;
    }

    int total = entrada->largura * entrada->altura;

    /* Aloca array de rotulos */
    int *rotulo = (int *)calloc(total, sizeof(int));
    if (!rotulo) {
        fprintf(stderr, "Erro: sem memoria\n");
        pgm_liberar(entrada);
        return 1;
    }

    /* Aloca array de objetos */
    Objeto *objetos = (Objeto *)malloc(MAX_OBJETOS * sizeof(Objeto));
    if (!objetos) {
        fprintf(stderr, "Erro: sem memoria\n");
        free(rotulo);
        pgm_liberar(entrada);
        return 1;
    }

    /* Detecta objetos */
    int n = detectar_objetos(entrada, rotulo, objetos);

    /* Atribui mesma cor a objetos com mesma forma */
    atribuir_cores(entrada, rotulo, objetos, n);

    /* Imprime resultado */
    imprimir_objetos(objetos, n);

    /* Gera imagem de saida colorida */
    gerar_saida_colorida(entrada, rotulo, objetos, n, arq_saida);

    /* Gera imagem com os 3 maiores (opcional) */
    if (arq_maiores)
        gerar_maiores(entrada, rotulo, objetos, n, arq_maiores);

    free(rotulo);
    free(objetos);
    pgm_liberar(entrada);

    return 0;
}

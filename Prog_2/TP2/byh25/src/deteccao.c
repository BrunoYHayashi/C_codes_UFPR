#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deteccao.h"
#include "pgm.h"

/* ------------------------------------------------------------------ */
/* Fila simples para detecção                                              */
/* ------------------------------------------------------------------ */
typedef struct {
    int *dados;
    int inicio, fim, capacidade;
} Fila;

static Fila *fila_criar(int cap)
{
    Fila *f = (Fila *)malloc(sizeof(Fila));
    f->dados = (int *)malloc(cap * sizeof(int));
    f->inicio = f->fim = 0;
    f->capacidade = cap;
    return f;
}

static void fila_liberar(Fila *f)
{
    free(f->dados);
    free(f);
}

static void fila_enfileirar(Fila *f, int v)
{
    f->dados[f->fim++] = v;
}

static int fila_desenfileirar(Fila *f)
{
    return f->dados[f->inicio++];
}

static int fila_vazia(Fila *f)
{
    return f->inicio >= f->fim;
}

/* ------------------------------------------------------------------ */
/* Deteccao de objetos                                     */
/* ------------------------------------------------------------------ */
int detectar_objetos(const ImagemPGM *entrada, int *rotulo, Objeto *objetos)
{
    int lin = entrada->altura;
    int col = entrada->largura;
    int total = lin * col;

    memset(rotulo, 0, total * sizeof(int));

    /* Fila maxima: todos os pixels */
    Fila *fila = fila_criar(total);

    int n = 0; /* numero de objetos encontrados */

    /* Vizinhanca 4 */
    int dl[] = {-1,  1,  0,  0};
    int dc[] = { 0,  0, -1,  1};

    for (int l = 0; l < lin; l++) {
        for (int c = 0; c < col; c++) {
            int idx = l * col + c;
            if (rotulo[idx] == 0 && entrada->pixels[idx] < LIMIAR) {
                /* Novo objeto: BFS */
                n++;
                if (n > MAX_OBJETOS) {
                    fprintf(stderr, "Aviso: limite de objetos atingido\n");
                    fila_liberar(fila);
                    return n - 1;
                }
                objetos[n-1].id   = n;
                objetos[n-1].x    = c;
                objetos[n-1].y    = l;
                objetos[n-1].area = 0;
                objetos[n-1].cor  = (n - 1) % N_CORES;

                rotulo[idx] = n;
                fila->inicio = fila->fim = 0; /* reseta fila */
                fila_enfileirar(fila, idx);

                while (!fila_vazia(fila)) {
                    int cur = fila_desenfileirar(fila);
                    int cl = cur / col;
                    int cc = cur % col;
                    objetos[n-1].area++;

                    for (int d = 0; d < 4; d++) {
                        int nl = cl + dl[d];
                        int nc = cc + dc[d];
                        if (nl < 0 || nl >= lin || nc < 0 || nc >= col)
                            continue;
                        int ni = nl * col + nc;
                        if (rotulo[ni] == 0 && entrada->pixels[ni] < LIMIAR) {
                            rotulo[ni] = n;
                            fila_enfileirar(fila, ni);
                        }
                    }
                }
            }
        }
    }

    fila_liberar(fila);
    return n;
}

/* ------------------------------------------------------------------ */
/* Cores RGB para rotulagem */
static unsigned char cores[][3] = {
    {255,   0,   0}, /* vermelho       */
    {  0, 255,   0}, /* verde          */
    {  0,   0, 255}, /* azul           */
    {255, 255,   0}, /* amarelo        */
    {255,   0, 255}, /* magenta        */
    {  0, 255, 255}, /* ciano          */
    {255, 128,   0}, /* laranja        */
    {128,   0, 255}, /* roxo           */
    {  0, 128,   0}, /* verde escuro   */
    {255,   0, 128}, /* rosa           */
    {  0, 128, 255}, /* azul claro     */
    {128, 255,   0}, /* verde limao    */
    {139,  69,  19}, /* marrom         */
    {255, 200,   0}, /* dourado        */
    {  0, 200, 150}, /* verde agua     */
    {200,   0,   0}, /* vermelho escuro*/
};
#define N_CORES 16

/* ------------------------------------------------------------------ */
/* Gera imagem de saida colorida                             */
/* ------------------------------------------------------------------ */
void gerar_saida_colorida(const ImagemPGM *entrada, const int *rotulo, const Objeto *objetos, int n_objetos, const char *arquivo_saida)
{
    (void)n_objetos;
    int lin = entrada->altura;
    int col = entrada->largura;

    FILE *fp = fopen(arquivo_saida, "wb");
    if (!fp) {
        fprintf(stderr, "Erro: nao foi possivel criar '%s'\n", arquivo_saida);
        return;
    }

    fprintf(fp, "P6\n%d %d\n255\n", col, lin);

    for (int i = 0; i < lin * col; i++) {
        int r, g, b;
        if (rotulo[i] == 0) {
            /* fundo branco */
            r = g = b = 255;
        } else {
            int obj_idx = rotulo[i] - 1; /* rotulo e 1-based */
            int c = objetos[obj_idx].cor % N_CORES;
            r = cores[c][0];
            g = cores[c][1];
            b = cores[c][2];
        }
        fputc(r, fp);
        fputc(g, fp);
        fputc(b, fp);
    }

    fclose(fp);
}

/* ------------------------------------------------------------------ */
/* Comparador para qsort (ordem decrescente de area)                   */
/* ------------------------------------------------------------------ */
static int cmp_area_dec(const void *a, const void *b)
{
    return ((Objeto *)b)->area - ((Objeto *)a)->area;
}

static int cmp_area_dec_ptr(const void *a, const void *b)
{
    Objeto *oa = *(Objeto **)a;
    Objeto *ob = *(Objeto **)b;
    return ob->area - oa->area;
}

/* ------------------------------------------------------------------ */
/* Calcula bounding box de um objeto */
static void bbox(const ImagemPGM *src, const int *rotulo, int obj_id,
                 int *min_l, int *max_l, int *min_c, int *max_c)
{
    int lin = src->altura;
    int col = src->largura;
    *min_l = lin; *max_l = -1;
    *min_c = col; *max_c = -1;
    for (int l = 0; l < lin; l++) {
        for (int c = 0; c < col; c++) {
            if (rotulo[l * col + c] == obj_id) {
                if (l < *min_l) *min_l = l;
                if (l > *max_l) *max_l = l;
                if (c < *min_c) *min_c = c;
                if (c > *max_c) *max_c = c;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* Gera imagem com os 3 maiores objetos lado a lado                    */
/* ------------------------------------------------------------------ */
void gerar_maiores(const ImagemPGM *entrada, const int *rotulo,
                   Objeto *objetos, int n_objetos,
                   const char *arquivo)
{
    int k = (n_objetos < 3) ? n_objetos : 3;

    /* Ponteiros para ordenar sem mexer no array original */
    Objeto *ptrs[MAX_OBJETOS];
    for (int i = 0; i < n_objetos; i++) ptrs[i] = &objetos[i];
    qsort(ptrs, n_objetos, sizeof(Objeto *), cmp_area_dec_ptr);

    /* Calcula bounding box de cada objeto selecionado */
    int min_l[3], max_l[3], min_c[3], max_c[3];
    int largura_total = 0;
    int altura_max    = 0;
    for (int i = 0; i < k; i++) {
        bbox(entrada, rotulo, ptrs[i]->id, &min_l[i], &max_l[i], &min_c[i], &max_c[i]);
        int w = max_c[i] - min_c[i] + 1;
        int h = max_l[i] - min_l[i] + 1;
        largura_total += w;
        if (h > altura_max) altura_max = h;
    }

    /* Margem entre objetos */
    int margem = 5;
    largura_total += margem * (k - 1);

    ImagemPGM *saida = pgm_criar(largura_total, altura_max, 255);
    if (!saida) return;
    /* ja foi inicializado com branco por pgm_criar */

    int offset = 0;
    for (int i = 0; i < k; i++) {
        int w = max_c[i] - min_c[i] + 1;
        int h = max_l[i] - min_l[i] + 1;
        /* Copia pixels do objeto para a saida */
        for (int l = min_l[i]; l <= max_l[i]; l++) {
            for (int c = min_c[i]; c <= max_c[i]; c++) {
                if (rotulo[l * entrada->largura + c] == ptrs[i]->id) {
                    int dl = l - min_l[i];
                    int dc = c - min_c[i] + offset;
                    if (dl < saida->altura && dc < saida->largura)
                        PGM_PIXEL(saida, dl, dc) = 0;
                }
            }
        }
        offset += w + margem;
        (void)h;
    }

    pgm_salvar(saida, arquivo);
    pgm_liberar(saida);
}

/* ------------------------------------------------------------------ */
/* Imprime lista ordenada por area (decrescente)                       */
/* ------------------------------------------------------------------ */
void imprimir_objetos(const Objeto *objetos, int n_objetos)
{
    /* Copia para ordenar */
    Objeto *copia = (Objeto *)malloc(n_objetos * sizeof(Objeto));
    if (!copia) return;
    memcpy(copia, objetos, n_objetos * sizeof(Objeto));
    qsort(copia, n_objetos, sizeof(Objeto), cmp_area_dec);

    printf("Total de objetos encontrados: %d\n", n_objetos);
    for (int i = 0; i < n_objetos; i++) {
        printf("Objeto %d (Posicao x=%d, y=%d): Area = %d pixels\n",
               copia[i].id, copia[i].x, copia[i].y, copia[i].area);
    }

    free(copia);
}

/* ------------------------------------------------------------------ */
/* Comparacao de formas e atribuicao de cores iguais                   */
/* ------------------------------------------------------------------ */

/* Retorna 1 se os dois objetos sao EXATAMENTE iguais:
   mesma largura, mesma altura, mesma area, e mesmos pixels */
static int mesma_forma(const ImagemPGM *src, const int *rotulo,
    int id_a, int id_b,
    int minl_a, int minc_a, int w_a, int h_a,
    int minl_b, int minc_b, int w_b, int h_b)
{
    /* Bounding box diferente -> objetos diferentes */
    if (w_a != w_b || h_a != h_b) return 0;

    /* Compara pixel a pixel dentro do bounding box */
    int col = src->largura;
    for (int l = 0; l < h_a; l++) {
        for (int c = 0; c < w_a; c++) {
            int pa = (rotulo[(minl_a + l) * col + (minc_a + c)] == id_a);
            int pb = (rotulo[(minl_b + l) * col + (minc_b + c)] == id_b);
            if (pa != pb) return 0;
        }
    }
    return 1;
}

void atribuir_cores(const ImagemPGM *entrada, const int *rotulo, Objeto *objetos, int n_objetos)
{
    /* Calcula bounding box de cada objeto */
    int *minl = (int *)malloc(n_objetos * sizeof(int));
    int *maxl = (int *)malloc(n_objetos * sizeof(int));
    int *minc = (int *)malloc(n_objetos * sizeof(int));
    int *maxc = (int *)malloc(n_objetos * sizeof(int));
    if (!minl || !maxl || !minc || !maxc) { 
        free(minl); 
        free(maxl); 
        free(minc); 
        free(maxc);
        return;
    }

    for (int i = 0; i < n_objetos; i++)
        bbox(entrada, rotulo, objetos[i].id, &minl[i], &maxl[i], &minc[i], &maxc[i]);

    /* Atribui cores: percorre objetos em ordem e agrupa iguais */
    int proxima_cor = 0;
    for (int i = 0; i < n_objetos; i++)
        objetos[i].cor = -1; /* nao atribuida */

    for (int i = 0; i < n_objetos; i++) {
        if (objetos[i].cor != -1) continue; /* ja tem cor */
        objetos[i].cor = proxima_cor;

        int w_i = maxc[i] - minc[i] + 1;
        int h_i = maxl[i] - minl[i] + 1;

        /* Procura objetos seguintes com a mesma forma */
        for (int j = i + 1; j < n_objetos; j++) {
            if (objetos[j].cor != -1) continue;
            int w_j = maxc[j] - minc[j] + 1;
            int h_j = maxl[j] - minl[j] + 1;
            if (mesma_forma(entrada, rotulo, objetos[i].id, objetos[j].id, minl[i], minc[i], w_i, h_i, minl[j], minc[j], w_j, h_j)) {
                objetos[j].cor = proxima_cor;
            }
        }
        proxima_cor++;
    }

    free(minl); free(maxl); free(minc); free(maxc);
}

void uso(const char *prog)
{
    fprintf(stderr, "Uso: %s -i entrada.pgm -o saida.ppm [-a maiores.pgm]\n", prog);
    exit(1);
}
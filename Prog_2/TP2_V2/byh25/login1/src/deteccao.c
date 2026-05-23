#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "deteccao.h"
#include "pgm.h"

/* ------------------------------------------------------------------ */
/* Fila simples para BFS                                               */
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
/* Deteccao por BFS (flood fill)                                       */
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
                objetos[n-1].cor  = (n - 1) % 8;

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
/* Cores RGB para rotulagem (8 cores distintas, excluindo branco)      */
/* ------------------------------------------------------------------ */
static unsigned char cores[8][3] = {
    {139,   0,   0}, /* vermelho escuro */
    {  0, 128,   0}, /* verde           */
    {  0,   0, 205}, /* azul            */
    {184, 134,  11}, /* dourado escuro  */
    {148,   0, 211}, /* roxo            */
    {  0, 139, 139}, /* ciano escuro    */
    {139,  69,  19}, /* marrom          */
    {  0,   0,   0}, /* preto           */
};

/* ------------------------------------------------------------------ */
/* Gera imagem de saida colorida (PPM P6)                              */
/* ------------------------------------------------------------------ */
void gerar_saida_colorida(const ImagemPGM *entrada, const int *rotulo,
                          const Objeto *objetos, int n_objetos,
                          const char *arquivo_saida)
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
            int c = objetos[obj_idx].cor % 8;
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

/* Tamanho do grid de normalizacao para comparacao invariante a escala */
#define GRID 32

/* Preenche grid[GRID*GRID] com a mascara do objeto normalizada para GRID x GRID.
   Cada celula do grid vale 1 se a maioria dos pixels originais correspondentes
   sao do objeto, 0 caso contrario. */
static void normalizar_forma(const ImagemPGM *src, const int *rotulo,
                             int obj_id,
                             int minl, int minc, int w, int h,
                             unsigned char *grid)
{
    int col = src->largura;
    int g;
    for (g = 0; g < GRID * GRID; g++) grid[g] = 0;

    for (int gl = 0; gl < GRID; gl++) {
        for (int gc = 0; gc < GRID; gc++) {
            /* Intervalo de pixels originais que esta celula representa */
            int l0 = minl + gl * h / GRID;
            int l1 = minl + (gl + 1) * h / GRID;
            int c0 = minc + gc * w / GRID;
            int c1 = minc + (gc + 1) * w / GRID;
            if (l1 <= l0) l1 = l0 + 1;
            if (c1 <= c0) c1 = c0 + 1;

            int total = 0, cheios = 0;
            for (int l = l0; l < l1 && l < src->altura; l++) {
                for (int c = c0; c < c1 && c < src->largura; c++) {
                    total++;
                    if (rotulo[l * col + c] == obj_id) cheios++;
                }
            }
            grid[gl * GRID + gc] = (total > 0 && cheios * 2 >= total) ? 1 : 0;
        }
    }
}

/* Retorna quantos dos 36 pixels de canto (3x3 em cada canto) pertencem ao objeto.
   36 = borda reta, 0 = borda arredondada. */
static int contar_cantos(const ImagemPGM *src, const int *rotulo,
                         int obj_id, int minl, int minc, int w, int h)
{
    int col = src->largura;
    int maxl = minl + h - 1;
    int maxc = minc + w - 1;
    int count = 0;
    for (int dl = 0; dl < 3; dl++) {
        for (int dc = 0; dc < 3; dc++) {
            if (rotulo[(minl+dl)*col + (minc+dc)] == obj_id) count++;
            if (rotulo[(minl+dl)*col + (maxc-dc)] == obj_id) count++;
            if (rotulo[(maxl-dl)*col + (minc+dc)] == obj_id) count++;
            if (rotulo[(maxl-dl)*col + (maxc-dc)] == obj_id) count++;
        }
    }
    return count; /* max = 36 */
}

/* Retorna 1 se os dois objetos tem a mesma forma */
static int mesma_forma(const ImagemPGM *src, const int *rotulo,
                       int id_a, int id_b,
                       int minl_a, int minc_a, int w_a, int h_a,
                       int minl_b, int minc_b, int w_b, int h_b)
{
    /* Calcula area (fill) de cada objeto */
    int area_a = 0, area_b = 0;
    for (int l = minl_a; l < minl_a + h_a; l++)
        for (int c = minc_a; c < minc_a + w_a; c++)
            if (rotulo[l * src->largura + c] == id_a) area_a++;
    for (int l = minl_b; l < minl_b + h_b; l++)
        for (int c = minc_b; c < minc_b + w_b; c++)
            if (rotulo[l * src->largura + c] == id_b) area_b++;

    int bbox_a = w_a * h_a;
    int bbox_b = w_b * h_b;

    /* Objeto e "solido" se fill > 0.97 */
    int solido_a = (area_a * 100 >= bbox_a * 97);
    int solido_b = (area_b * 100 >= bbox_b * 97);

    /* Se um e solido e outro nao, formas diferentes */
    if (solido_a != solido_b) return 0;

    /* Verifica cantos: borda reta tem cantos preenchidos, arredondada nao */
    int cantos_a = contar_cantos(src, rotulo, id_a, minl_a, minc_a, w_a, h_a);
    int cantos_b = contar_cantos(src, rotulo, id_b, minl_b, minc_b, w_b, h_b);
    /* Limiar: > 18 = borda reta, <= 18 = borda arredondada */
    int reta_a = (cantos_a > 18);
    int reta_b = (cantos_b > 18);
    if (reta_a != reta_b) return 0;

    /* Se ambos sao solidos E ambos tem mesma borda (reta ou arredondada) */
    if (solido_a && solido_b) {
        /* Distingue quadrado (ar~1.0) de retangulo (ar>1.1):
           quadrado: max(w,h)/min(w,h) < 1.1
           retangulo: max(w,h)/min(w,h) >= 1.1
           Usa inteiros: quadrado se 10*min >= 9*max (ou seja, max/min < 1.11) */
        int max_a = w_a > h_a ? w_a : h_a;
        int min_a = w_a < h_a ? w_a : h_a;
        int max_b = w_b > h_b ? w_b : h_b;
        int min_b = w_b < h_b ? w_b : h_b;
        int quadrado_a = (min_a * 10 >= max_a * 9); /* max/min < 1.11 */
        int quadrado_b = (min_b * 10 >= max_b * 9);
        /* quadrado != retangulo -> formas diferentes */
        if (quadrado_a != quadrado_b) return 0;
        return 1;
    }

    /* Ambos nao solidos: usa grid normalizado */
    unsigned char ga[GRID * GRID], gb[GRID * GRID];
    normalizar_forma(src, rotulo, id_a, minl_a, minc_a, w_a, h_a, ga);
    normalizar_forma(src, rotulo, id_b, minl_b, minc_b, w_b, h_b, gb);

    int diff = 0;
    for (int i = 0; i < GRID * GRID; i++)
        if (ga[i] != gb[i]) diff++;

    return diff <= GRID * GRID / 10;
}

void atribuir_cores(const ImagemPGM *entrada, const int *rotulo,
                    Objeto *objetos, int n_objetos)
{
    /* Calcula bounding box de cada objeto */
    int *minl = (int *)malloc(n_objetos * sizeof(int));
    int *maxl = (int *)malloc(n_objetos * sizeof(int));
    int *minc = (int *)malloc(n_objetos * sizeof(int));
    int *maxc = (int *)malloc(n_objetos * sizeof(int));
    if (!minl || !maxl || !minc || !maxc) return;

    for (int i = 0; i < n_objetos; i++)
        bbox(entrada, rotulo, objetos[i].id,
             &minl[i], &maxl[i], &minc[i], &maxc[i]);

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
            if (mesma_forma(entrada, rotulo,
                            objetos[i].id, objetos[j].id,
                            minl[i], minc[i], w_i, h_i,
                            minl[j], minc[j], w_j, h_j)) {
                objetos[j].cor = proxima_cor;
            }
        }
        proxima_cor++;
    }

    free(minl); free(maxl); free(minc); free(maxc);
}

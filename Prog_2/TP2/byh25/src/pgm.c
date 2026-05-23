#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pgm.h"

/* Le uma linha do cabecalho ignorando comentarios */
static int ler_linha_header(FILE *fp, char *buf, int tam)
{
    while (fgets(buf, tam, fp)) {
        if (buf[0] != '#')
            return 1;
    }
    return 0;
}

ImagemPGM *pgm_ler(const char *arquivo)
{
    FILE *fp = fopen(arquivo, "rb");
    if (!fp) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", arquivo);
        return NULL;
    }

    char buf[256];
    /* Primeira linha: magic P2 ou P5 */
    if (!ler_linha_header(fp, buf, sizeof(buf))) {
        fclose(fp); return NULL;
    }
    int is_p5 = (buf[0] == 'P' && buf[1] == '5');
    int is_p2 = (buf[0] == 'P' && buf[1] == '2');
    if (!is_p5 && !is_p2) {
        fprintf(stderr, "Erro: formato PGM invalido ('%s')\n", buf);
        fclose(fp); return NULL;
    }

    /* Largura e altura */
    if (!ler_linha_header(fp, buf, sizeof(buf))) {
        fclose(fp); return NULL;
    }
    int largura, altura;
    sscanf(buf, "%d %d", &largura, &altura);

    /* MAXVAL */
    if (!ler_linha_header(fp, buf, sizeof(buf))) {
        fclose(fp); return NULL;
    }
    int maxval;
    sscanf(buf, "%d", &maxval);

    ImagemPGM *img = pgm_criar(largura, altura, maxval);
    if (!img) { fclose(fp); return NULL; }

    int total = largura * altura;

    if (is_p5) {
        if (maxval < 256) {
            if ((int)fread(img->pixels, 1, total, fp) != total) {
                fprintf(stderr, "Erro ao ler pixels binarios\n");
                pgm_liberar(img); fclose(fp); return NULL;
            }
        } else {
            /* 2 bytes por pixel, big-endian - armazena so byte alto (simplificado) */
            for (int i = 0; i < total; i++) {
                int hi = fgetc(fp);
                int lo = fgetc(fp);
                img->pixels[i] = (unsigned char)((hi * 256 + lo) * 255 / maxval);
            }
            img->maxval = 255;
        }
    } else {
        /* P2: valores ASCII */
        for (int i = 0; i < total; i++) {
            int v;
            if (fscanf(fp, "%d", &v) != 1) {
                fprintf(stderr, "Erro ao ler pixel ASCII\n");
                pgm_liberar(img); fclose(fp); return NULL;
            }
            img->pixels[i] = (unsigned char)v;
        }
    }

    fclose(fp);
    return img;
}

int pgm_salvar(const ImagemPGM *img, const char *arquivo)
{
    FILE *fp = fopen(arquivo, "wb");
    if (!fp) {
        fprintf(stderr, "Erro: nao foi possivel criar '%s'\n", arquivo);
        return 0;
    }
    fprintf(fp, "P5\n%d %d\n%d\n", img->largura, img->altura, img->maxval);
    int total = img->largura * img->altura;
    fwrite(img->pixels, 1, total, fp);
    fclose(fp);
    return 1;
}

ImagemPGM *pgm_criar(int largura, int altura, int maxval)
{
    ImagemPGM *img = (ImagemPGM *)malloc(sizeof(ImagemPGM));
    if (!img) return NULL;
    img->largura = largura;
    img->altura  = altura;
    img->maxval  = maxval;
    img->pixels  = (unsigned char *)malloc(largura * altura);
    if (!img->pixels) { free(img); return NULL; }
    memset(img->pixels, 255, largura * altura); /* fundo branco */
    return img;
}

void pgm_liberar(ImagemPGM *img)
{
    if (img) {
        free(img->pixels);
        free(img);
    }
}

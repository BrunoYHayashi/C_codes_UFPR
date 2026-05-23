#ifndef PGM_H
#define PGM_H

typedef struct {
    int largura;
    int altura;
    int maxval;
    unsigned char *pixels; /* linha * largura + coluna */
} ImagemPGM;

ImagemPGM *pgm_ler(const char *arquivo);
int pgm_salvar(const ImagemPGM *img, const char *arquivo);
ImagemPGM *pgm_criar(int largura, int altura, int maxval);
void pgm_liberar(ImagemPGM *img);

/* Acesso ao pixel (linha, col) */
#define PGM_PIXEL(img, lin, col) ((img)->pixels[(lin) * (img)->largura + (col)])

#endif

#ifndef ENTITIES
#define ENTITIES
#include "fila.h"
#include "fprio.h"
#include "environment.h"
#include "conjunto.h"
#include <stdbool.h>

/*Estrutura da base*/
struct base{
    int ID; //id da base
    int capacity; //capacidade total
    struct cjto_t *presents; //presentes na base
    struct fila_t *waitLine; //fila de heróis
    struct coordinates location; //local
    int maxLine; //tamanho máximo que a fila alcançou
    int missionCounter; //quantas missões a base participou
};

/*Estrutura do herói*/
struct hero {
    int ID; //id do herói
    struct cjto_t *skills; //cjto de habilidades do herói
    int pacience; //paciencia
    int speed; //velocidade
    int xp; //experienca
    struct base *base; //a base em que está
    bool alive; //se está vivo
};

/*Estrutura da missão*/
struct mission {
    int ID; //id da missão
    struct cjto_t *skillsRequired; //skills necessárias para completá-la
    struct coordinates location; //local da missão
    int attempts; //quantidade de tentativas
    bool done; //se já foi feita
};

/*Estrutura do mundo*/
struct world{
    int Nheroes; //Número de heróis
    struct hero **heroes; //vetor de heróis

    int Nbases; //Número de bases
    struct base **bases; //vetor de bases

    int Nmissions; //Número de missões
    struct mission **missions; //vetor de missões

    struct fprio_t *lef; //fila de prioridades lef
    int Nskills; //número de habilidades
    int NVcomposts; //número de compostos V
    struct coordinates worldSize; //Tamanho do mundo
    int time; //tempo
};

/*Inicia o mundo*/
struct world *initialize_world();

/*Inicia o herói*/
struct hero *initialize_hero(int id);

/*Inicia a base*/
struct base *initialize_base(int id);

/*Inicia a missão*/
struct mission *initialize_mission(int id);

/*Destroi o mundo*/
struct world *destroy_world(struct world *w);

/*Destrói o herói*/
struct hero *destroy_hero(struct hero *h);

/*Destrói a base*/
struct base *destroy_base(struct base *b);

/*Destrói a missão*/
struct mission *destroy_mission(struct mission *m);

#endif
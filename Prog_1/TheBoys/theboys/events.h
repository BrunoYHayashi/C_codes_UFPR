#ifndef EVENTS
#define EVENTS

#include "entities.h"

/*definição dos (int tipo) dos eventos*/
#define EVENT_CHEGA 1
#define EVENT_ESPERA 2
#define EVENT_DESISTE 3
#define EVENT_AVISA 4
#define EVENT_ENTRA 5
#define EVENT_SAI 6
#define EVENT_VIAJA 7
#define EVENT_MORRE 8
#define EVENT_MISSAO 9
#define EVENT_FIM 10

/*Estrutura dos eventos, nem todo evento conterá as 3 structs de composição*/
struct event {
    struct hero *hero;
    struct base *base;
    struct mission *mission;
};

/*Funções dos eventos, todos recebem o ponteiro para o mundo, o tempo e uma struct event*/
void event_chega (struct world *w, int time, struct event *data);
void event_espera (struct world*w, int time, struct event *data);
void event_desiste (struct world*w, int time, struct event *data);
void event_avisa (struct world*w, int time, struct event *data);
void event_entra (struct world*w, int time, struct event *data);
void event_sai (struct world*w, int time, struct event *data);
void event_viaja (struct world*w, int time, struct event *data);
void event_morre (struct world*w, int time, struct event *data);
void event_missao (struct world*w, int time, struct event *data);
void event_fim (int time);

/*Funções auxiliares*/
void schedule_event (struct world *w, int time, int tipo, struct hero *hero, struct base *base, struct mission *mission); //Agenda um evento, facilitando a escrita do código

/*Estrutura que carrega uma base e a distância dela para uma missão*/
struct base_dist{
    struct base *base;
    int dist;
};

/*SelectSort*/
void selectSort(struct base_dist *baseDist, int n);

#endif
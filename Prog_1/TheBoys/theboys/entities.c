#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "entities.h"
#include "environment.h"
#include "events.h"

/*Inicia o mundo*/
struct world *initialize_world(){
    struct world *newWorld;

    if(!(newWorld=malloc(sizeof(struct world)))) //Aloca memória para o mundo
        return NULL;

    newWorld->lef= fprio_cria(); //Cria a fila de prioridades lef
    if (!newWorld->lef){ //verifica se fprio_cria foi bem sucedido
        free(newWorld);
        newWorld= NULL;
        return NULL;
    }

    /*Atribui as constantes à data do mundo*/
    newWorld->Nheroes=N_HEROES; 
    newWorld->Nbases=N_BASES;
    newWorld->Nmissions=N_MISSIONS;
    newWorld->Nskills=N_SKILLS;
    newWorld->NVcomposts=N_VComposts;
    newWorld->time=STARTER_TIME;
    newWorld->worldSize.x=WORLD_SIZE;
    newWorld->worldSize.y=WORLD_SIZE;

    /*Cria os vetores de heróis, bases e missões*/
    struct hero **vetHeroes;
    struct base **vetBases;
    struct mission **vetMissions;

    /*Vetores do mundo vazios*/
    newWorld->heroes= NULL;
    newWorld->bases= NULL;
    newWorld->missions= NULL;

    /*Aloca memórias para os vetores*/
    if(!(vetHeroes=malloc(sizeof(struct hero*)*(N_HEROES))))
        return NULL;
    newWorld->heroes= vetHeroes;

    if(!(vetBases=malloc(sizeof(struct base*)*(N_BASES))))
        return NULL;
    newWorld->bases= vetBases;

     if(!(vetMissions=malloc(sizeof(struct mission*)*(N_MISSIONS))))
        return NULL;
    newWorld->missions= vetMissions;

    int i;

    /*Preenche os vetores com os heróis, bases e missões*/
    for(i=0; i<N_HEROES; i++)
        newWorld->heroes[i]= initialize_hero(i);

    for(i=0; i<N_BASES;i++)
        newWorld->bases[i]= initialize_base(i);
    
    for(i=0;i<N_MISSIONS;i++)
        newWorld->missions[i]= initialize_mission(i);

    return newWorld;
}

/*Inicia o herói*/
struct hero *initialize_hero(int id){
    struct hero *newHero;

    if (!(newHero=malloc(sizeof(struct hero)))) //Aloca memória para o herói
        return NULL;

    /*Atribuições da data do herói*/
    newHero->ID= id;
    newHero->xp= 0;
    newHero->pacience= aleat(0,100);
    newHero->speed= aleat(50,5000);
    newHero->skills= cjto_aleat(aleat(1,3),N_SKILLS);
    newHero->base= NULL;
    newHero->alive= true;

    return newHero;
}

/*Inicia a base*/
struct base *initialize_base(int id){
    struct base *newBase;

    if(!(newBase=malloc(sizeof(struct base)))) //Aloca memória para a base
        return NULL;
    
    /*Atribuições da data da base*/
    newBase->location.x= aleat(0, WORLD_SIZE-1);
    newBase->location.y= aleat(0, WORLD_SIZE-1);
    newBase->ID= id;
    newBase->capacity=aleat(3,10);
    newBase->presents=cjto_cria(N_HEROES); 
    if (!newBase->presents){ //Verifica se cjto_cria foi bem sucedido
        free(newBase);
        return NULL;
    }
    newBase->waitLine= fila_cria(); 
    if(!newBase->waitLine){ //Verifica se fila_cria foi bem sucedido
        cjto_destroi(newBase->presents);
        free(newBase);
        return NULL;
    }

    newBase->maxLine = 0;
    newBase->missionCounter = 0;

    return newBase;
}

/*Inicializa a missão*/
struct mission *initialize_mission(int id){
    struct mission *newMission;

    if(!(newMission=malloc(sizeof(struct mission)))) //Aloca memória para a missão
        return NULL;

    newMission->ID= id;
    newMission->skillsRequired= cjto_aleat(aleat(6, 10), N_SKILLS);
    newMission->location.x= aleat(0, WORLD_SIZE-1);
    newMission->location.y= aleat(0, WORLD_SIZE-1);
    newMission->attempts=0;
    newMission->done= false;
    
    return newMission;
};

/*Destroi o mundo*/
struct world *destroy_world(struct world *w){
    if(!w) //verifica se w é um ponteiro válido
        return NULL;

    fprio_destroi(w->lef); //destrói a lef

    int i;

    if (w->heroes){ //libera cada herói
        for (i=0; i< w->Nheroes; i++)
            destroy_hero(w->heroes[i]);
        
        free(w->heroes); //libera o vetor de heróis
        w->heroes = NULL;
    }

    if (w->bases){ //libera cada base
        for (i=0; i< w->Nbases; i++)
            destroy_base(w->bases[i]);

        free(w->bases); //libera o vetor de bases
        w->bases = NULL;
    }

    if (w->missions){ //libera cada missão
        for (i=0; i < w->Nmissions; i++)
            destroy_mission(w->missions[i]);
        
        free(w->missions); //libera o vetor de missões
        w->missions = NULL;
    }

    free(w); //libera o mundo
    w = NULL;
    
    return w;
}

/*Destrói o herói*/
struct hero *destroy_hero(struct hero *h){
    if(!h) //verifica se h é um ponteiro válido
        return NULL;
    
    cjto_destroi(h->skills); //libera o cjto de skills do herói
    free(h); //libera o herói
    h = NULL;

    return h;
}

/*Destrói a base*/
struct base *destroy_base(struct base *b){
    if(!b) //verifica se b é um ponteiro válido
        return NULL;

    cjto_destroi(b->presents); //libera o conjunto de presentes na base
    fila_destroi(b->waitLine); //libera a fila de espera da base
    
    free(b); //libera a base
    b = NULL;

    return b;
}

/*Destrói a missão*/
struct mission *destroy_mission(struct mission *m){
    if(!m) //verifica se m é um ponteiro válido
        return NULL;

    cjto_destroi(m->skillsRequired); //libera o conjunto de habilidades requeridas

    free(m); //libera a missão
    m = NULL;
    return m;
}
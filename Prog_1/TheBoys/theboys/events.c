#include <stdio.h>  
#include <stdlib.h>
#include <math.h>

#include "events.h"
#include "fprio.h"
#include "fila.h"
#include "entities.h"
#include "conjunto.h"
#include "environment.h"

void schedule_event (struct world *w, int time, int tipo, struct hero *hero, struct base *base, struct mission *mission){
    struct event *event;

    if(!(event=malloc(sizeof(struct event)))) //Aloca memória para o evento
        return;

    /*Atribui ao evento o herói afetado, a base afetada, ou a missão*/
    event->hero = hero;
    event->base = base;
    event->mission = mission;

    fprio_insere(w->lef, event, tipo, time); //Insere na fila de prioridades o evento
}

void selectSort(struct base_dist *list, int n){
    int i, j, min;

    for (i=0; i< n-1; i++){
        min = i;
        for (j= i+1; j<n; j++){
            if (list[j].dist < list[min].dist)
                min = j;
        }

        if (min != i){
            struct base_dist AUX = list[i];
            list[i] = list[min];
            list[min] = AUX;
        }
    }

}

void event_chega (struct world *w, int time, struct event *data) {
    struct hero *hero = data->hero;
    struct base *base = data->base;

    hero->base=base; //Base em que o herói está

    bool decision; //decisao do herói
    int queue = fila_tamanho(base->waitLine); //Heróis na fila
    int occupy = cjto_card(base->presents); //Heróis que estão na base

    /*Verificação da decisão do herói:
    Se a base não estiver lotada e a fila estiver vazia, ele decide ficar sem pensar
    Se não, ele fica apenas se a paciência dele for maior que 10* a fila*/
    if (occupy < base->capacity && queue == 0) 
        decision = true;
    else
        decision = (hero->pacience > (10*queue));

    printf("%6d: CHEGA HEROI %2d BASE %d (%2d/%2d) ", time, hero->ID, base->ID, occupy, base->capacity);
    printf("%s \n", decision ? "ESPERA" : "DESISTE");

    /*Define o próximo evento
    Se a decisão for ficar, ocorre o evento espera
    Se não, ocorre o evento desiste*/
    if (decision)
        schedule_event(w, time, EVENT_ESPERA, hero, base, NULL);
    else 
        schedule_event(w, time, EVENT_DESISTE, hero, base, NULL);
}

void event_espera(struct world *w, int time, struct event *data){
    struct hero *hero = data->hero;
    struct base *base = data->base;

    int *idPtr; //cria um ponteiro para o id do herói
    if (!(idPtr=malloc(sizeof(int))))
        return;
    
    *idPtr = hero->ID; 
    fila_insere(base->waitLine, idPtr); //insere o ponteiro do id do herói na fila

    int queue = fila_tamanho(base->waitLine); //"queue" recebe o tamanho da fila

    if (queue > base->maxLine){
        base->maxLine = queue;
    }

    printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", time, hero->ID, base->ID, queue -1);

    schedule_event(w, time, EVENT_AVISA, NULL, base, NULL); //Agenda o próximo evento (AVISA)
}

void event_desiste (struct world *w, int time, struct event *data){
    struct hero *hero = data->hero;
    struct base *base = data->base;

    int random_baseID = aleat(0, w->Nbases -1); //gera o ID de uma nova base aleatória
    struct base *newDestiny = w->bases[random_baseID]; //atribui para "newDestiny" essa nova base

    printf("%6d: DESIST HEROI %2d BASE %d\n", time, hero->ID, base->ID);

    schedule_event(w, time, EVENT_VIAJA, hero, newDestiny, NULL); //agenda o próximo evento, a viagem do herói para a nova base
}

void event_avisa (struct world *w, int time, struct event *data){
    struct base *base = data->base;

    int occupy = cjto_card(base->presents); //atribui a "occupy" o número de presentes na base

    printf("%6d: AVISA PORTEIRO BASE %d (%2d/%2d) FILA [ ", time, base->ID, occupy, base->capacity);
    fila_imprime(base->waitLine);
    printf("]\n");

    while (occupy < base->capacity && fila_tamanho(base->waitLine) > 0){ //enquanto a base não estiver lotada e tiver gente na fila
        int *idPtr = fila_retira(base->waitLine); //tira o primeiro herói da fila
        int heroId = *idPtr;
        free(idPtr); //libera a memória criada no event_espera

        cjto_insere(base->presents, heroId); //adiciona o herói no conjunto de presentes na base
        occupy++; //atualiza contagem pro loop

        struct hero *hero = w->heroes[heroId]; //recupera o ponteiro do herói para passar no "agendador"

        printf("%6d: AVISA PORTEIRO BASE %d ADMITE %2d\n", time, base->ID, heroId);

        schedule_event(w, time, EVENT_ENTRA, hero, base, NULL); //agenda o próximo evento (entrada)
    }
}

void event_entra (struct world *w, int time, struct event *data){
    struct hero *hero = data->hero;
    struct base *base = data->base;

    int stayTime = 15 + hero->pacience * aleat(1,20); //Tempo de permanência do herói
    int exitTime = time +stayTime;

    printf("%6d: ENTRA HEROI %2d BASE %d (%2d/%2d) SAI %d\n", time, hero->ID, base->ID, cjto_card(base->presents), base->capacity, exitTime);

    schedule_event(w, exitTime, EVENT_SAI, hero, base, NULL); //Agenda o próximo evento (SAÍDA)
}

void event_sai (struct world *w, int time, struct event *data){
    struct hero *hero = data->hero;

    if(!hero->alive)
        return;

    struct base *base = data->base;

    cjto_retira(base->presents, hero->ID); //tira o herói da base

    int destinyId = aleat(0,w->Nbases -1); //novo destino aleatório do herói
    struct base *destiny = w->bases[destinyId]; 

    printf("%6d: SAI HEROI %2d BASE %d (%2d/%2d)\n", time, hero->ID, base->ID, cjto_card(base->presents), base->capacity);

    schedule_event(w, time, EVENT_VIAJA, hero, destiny, NULL); //agenda o próximo evento (viaja)
    schedule_event(w, time, EVENT_AVISA, NULL, base, NULL); //avisa que abriu uma vaga na base
}

void event_viaja (struct world *w, int time, struct event *data){
    struct hero *hero = data->hero;

    if (!hero->alive || hero->base == NULL)
        return;

    struct base *destiny = data->base; //base destino
    struct base *origin = hero->base; //base origem

    int distX = destiny->location.x - origin->location.x; //distância do destino e da origem (x2-x1)
    int distY = destiny->location.y - origin->location.y; //distância do destino e da origem (y2-y1)
    int distance = (int)sqrt((distX*distX) + (distY*distY)); //distância cartesiana = sqrt((x2-x1)² + (y2-y1)²)

    int duration = distance / hero->speed; //duração (distancia/velocidade)
    int arrivalTime = time + duration; //hora de chegada

    printf("%6d: VIAJA HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", time, hero->ID, origin->ID, destiny->ID, distance, hero->speed, arrivalTime);

    schedule_event(w, arrivalTime, EVENT_CHEGA, hero, destiny, NULL); //agenda próximo evento (CHEGA na próxima base)
}

void event_morre (struct world *w, int time, struct event *data){
    struct hero *hero = data->hero;
    struct base *base = data->base;

    cjto_retira(base->presents, hero->ID); //retira o herói da base

    hero->alive = false; //coloca o estado do herói como não vivo

    hero->base = NULL; //base vira nula

    printf("%6d: MORRE HEROI %2d MISSAO %d\n", time, hero->ID, data->mission ? data->mission->ID : -1); //faz uma condicional para não dar seg fault

    schedule_event(w, time, EVENT_AVISA, NULL, base, NULL); //agenda o próximo evento (AVISA) para alertar que abriu uma vaga na base
}

void event_fim (int time){
    printf("%6d: FIM\n", time);
}

void event_missao (struct world *w, int time, struct event *data){
    struct mission *mission = data->mission;

    mission->attempts++; //Incrementa o número de tentativas


//Separarei em comentários para não me perder no código

//Impressão das informações da missão 
//=======================================================================================

    printf("%6d: MISSAO %d TENT %d HAB REQ: [ ", time, mission->ID, mission->attempts);
    cjto_imprime(mission->skillsRequired);
    printf(" ]\n");

//=======================================================================================


//Criação de um vetor de distâncias de todas as bases para a missão
//=======================================================================================
    
    struct base_dist *list;
    if (!(list=malloc(sizeof(struct base_dist)*(w->Nbases)))) //Cria um vetor de distância da missão para todas as bases
        return;

    int i;

    for (i=0; i< w->Nbases; i++){ //preenche o vetor com as bases e suas respectivas distâncias da missão
        struct base *base = w->bases[i];
        int distX = base->location.x - mission->location.x; //calcula a distância da base[i] para a missão em x
        int distY = base->location.y - mission->location.y; //calcula a distância da base[i] para a missão em y

        list[i].base = base; //base da lista = base do vetor do mundo
        list[i].dist = (int)sqrt((distX*distX)+(distY*distY)); //dist da base até a missão é atribuída na estrutura da lista
    }

    selectSort(list, w->Nbases); //ordena a lista pela distância

//=======================================================================================


    int bestBase = -1; //define aqui um bestBase -1 para facilitar na verificação das condicionais
    struct cjto_t *teamSkills = NULL; //define aqui um teamskills NULL para facilitar na verificação das condicionais


//Encontra a base para cumprir a missão
//=======================================================================================

    for (i=0; i< w->Nbases; i++){ //procura a primeira base capaz de cumprir a missão
        struct base *base = list[i].base;

        printf("%6d: MISSAO %d BASE %d DIST %d HEROIS [ ", time, mission->ID, base->ID, list[i].dist); //imprime os heróis participantes da missão

        struct cjto_t *currentSkills = cjto_cria(w->Nskills); //cria um conjunto para as habilidades da equipe

        int heroes = 0, heroId;

        for (heroId = 0; heroId < w->Nheroes; heroId++){ //percorre todos os heróis da base
            if (cjto_pertence(base->presents, heroId)){ //verifica se herói está na base
                if (heroes > 0)
                    printf(" ");
                printf("%d", heroId);
                heroes++;

                struct hero *hero = w->heroes[heroId];
                struct cjto_t *AUX = cjto_uniao(currentSkills, hero->skills); //Atribui a AUX a união do conjunto de habilidades atuais + habilidades do novo herói
                cjto_destroi(currentSkills); //Esvazia o conjunto de habilidades anterior
                currentSkills = AUX; //Recebe o conjunto atualizado
            }
        }

        printf(" ]\n");

        for (heroId=0; heroId< w->Nheroes; heroId++){ //Imprime as habilidades de cada herói
            if (cjto_pertence(base->presents, heroId)){
                printf("%6d: MISSAO %d HAB HEROI %2d: [", time, mission->ID, heroId);
                cjto_imprime(w->heroes[heroId]->skills);
                printf(" ]\n");
            }
        }

        printf("%6d: MISSAO %d UNIAO HAB BASE %d: [ ", time, mission->ID, base->ID); //imprime a união das habilidades da equipe
        cjto_imprime(currentSkills);
        printf(" ]\n");

        if (cjto_contem(currentSkills, mission->skillsRequired)){ //verifica se as habilidades da equipe são suficientes para a missão
            bestBase = i; //se forem suficientes, define essa como sendo a melhor base para cumprir a missão
            teamSkills = currentSkills; //seta as habilidades do time como as da base
            break;
        }
        else
            cjto_destroi(currentSkills); //se não, destrói as habilidades atuais para verificar a próxima base
    }

//=======================================================================================


//Casos das missões 
//Caso 1 = missão bem sucedida - encontrada base com habilidades suficientes
//=======================================================================================
    
    if (bestBase != -1){
        struct base *base = list[bestBase].base;

        base->missionCounter++; //incrementa o contador de missão da base

        printf("%6d: MISSAO %d CUMPRIDA BASE %d HABS: [ ", time, mission->ID, base->ID); //imprime as habilidades do time
        cjto_imprime(teamSkills);
        printf(" ]\n");

        int heroId;

        for (heroId=0; heroId< w->Nheroes; heroId++){ //atribui xp aos heróis participantes
            if (cjto_pertence(base->presents, heroId))
                w->heroes[heroId]->xp++;
        }

        mission->done = true; //atribui a missão como feita
        cjto_destroi(teamSkills); //destrói o conjunto de habilidades do time
        
    }

//=======================================================================================

//Caso 2 = missão fracassada na primeira tentativa
//=======================================================================================

    else{
    //Caso 2.1 = O tempo é múltiplo de 2500 e existem compostos V disponíveis para uso
    //=======================================================================================
        if (time % 2500 == 0 && w->NVcomposts > 0){
            struct base *base = list[0].base; //pega a base mais próxima

            if (cjto_card(base->presents) > 0){ //verifica se há heróis nessa base
                base->missionCounter++; //incrementa o contador de missão da basae
                w->NVcomposts--; //decrementa o número de compostos V no mundo

                printf("%6d: MISSAO %d CUMPRIDA BASE %d COMPOSTO V\n", time, mission->ID, base->ID);

                struct hero *compostV_user; //Inicializa como NULL para poder fazer a verificação do compost
                int maxXp = -1; //Inicializa o maior xp como -1 para poder fazer a busca pelo herói com maior XP
                int heroId;

                for (heroId=0; heroId< w->Nheroes; heroId++){ //busca o herói que vai consumir o composto v
                    if (cjto_pertence(base->presents, heroId)){
                        struct hero *hero = w->heroes[heroId];
                        if(hero->xp > maxXp){
                            maxXp = hero->xp;
                            compostV_user = hero;
                        }
                    }
                }

                for (heroId=0; heroId < w->Nheroes; heroId++){ //atribui xp aos heróis participantes com exceção do usuário
                    if (cjto_pertence(base->presents, heroId)){
                        if (w->heroes[heroId] != compostV_user)
                            w->heroes[heroId]->xp++;
                    }
                }

                schedule_event(w, time, EVENT_MORRE, compostV_user, base, mission); //agenda próximo evento (morte de quem utilizou o composto)
                
                mission->done = true; //atribui a missão como concluída
            }
            else { //Não existe herói na base mais próxima
                printf("%6d: MISSAO %d IMPOSSIVEL\n", time, mission->ID); //missão impossível
                schedule_event(w, time +24*60, EVENT_MISSAO, NULL, NULL, mission); //agenda próximo evento (MISSAO) com o tempo que passou
            }
        }
        
    //Caso 2.2 = Não existe composto V ou o tempo não é múltiplo de 2500
    //=======================================================================================    
        else{
            printf("%6d: MISSAO %d IMPOSSIVEL\n", time, mission->ID); //missão impossível
            schedule_event(w, time + 24*60, EVENT_MISSAO, NULL, NULL, mission); //agenda o próximo evento (MISSAO) com o tempo que passou
        }
    }

//=======================================================================================
    
    free(list); //libera o vetor
}
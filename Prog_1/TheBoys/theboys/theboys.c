// programa principal do projeto "The Boys - 2024/2"
// Autor: xxxxx, GRR xxxxxx

// seus #includes vão aqui
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "entities.h"
#include "events.h"
#include "fprio.h"
#include "fila.h"
#include "conjunto.h"
#include "environment.h"

// seus #defines vão aqui

// minimize o uso de variáveis globais

// programa principal
int main ()
{
  // iniciar o mundo
  //=======================================================================================

  srand(0); //seed do número aleatório

  struct world *w = initialize_world(); //inicializa o mundo
  if (!w){ //caso de erro
    printf("Erro ao inicializar o mundo.\n");
    return 1;
  }
  //=======================================================================================
  int i;

  //Chegada dos heróis nos 3 primeiros dias
  //=======================================================================================

  for (i=0; i< w->Nheroes; i++){
    struct hero *h = w->heroes[i];
    int baseId = aleat(0, w->Nbases -1); //escolhe uma base aleatória para o herói ir
    struct base *b = w->bases[baseId]; 
    int arrivalTime = aleat(0, 4320); //Tempo de chegada do herói até a base

    schedule_event(w, arrivalTime, EVENT_CHEGA, h, b, NULL); //agenda a chegada do herói
  }

  //=======================================================================================

  //Agendamento das missões
  //=======================================================================================

  for (i=0; i< w->Nmissions; i++){
    struct mission *m = w->missions[i];
    int missionTime = aleat(0, ENDING_TIME); //tempo aleatório de ocorrência da missão

    schedule_event(w, missionTime, EVENT_MISSAO, NULL, NULL, m); //agenda a missão
  }

  //=======================================================================================

  //Evento de fim do mundo
  //=======================================================================================

  schedule_event(w, ENDING_TIME, EVENT_FIM, NULL, NULL, NULL);

  //=======================================================================================


  // executar o laço de simulação
  int simulationTime = 0, type, prio;
  struct event *data;
  int totalEventos = 0;

  //Execução dos eventos
  //=======================================================================================

  while (simulationTime < ENDING_TIME) { //Enquanto o tempo de simulação for menor que o tempo de fim do mundo
    data = fprio_retira(w->lef, &type, &prio); //retira o evento da fila de prioridades
    if (!data) //enquanto tiver evento
      break; 

    totalEventos++;
    simulationTime = prio; //atualiza o relógio
    w->time = simulationTime;

    switch (type) { //seletor de eventos de acordo com o tipo (type)
      case EVENT_CHEGA:
        event_chega(w, simulationTime, data);
        break;
      case EVENT_ESPERA:
        event_espera(w, simulationTime, data);
        break;
      case EVENT_DESISTE:
        event_desiste(w, simulationTime, data);
        break;
      case EVENT_AVISA:
        event_avisa(w, simulationTime, data);
        break;
      case EVENT_ENTRA:
        event_entra(w, simulationTime, data);
        break;
      case EVENT_SAI:
        event_sai(w, simulationTime, data);
        break;
      case EVENT_VIAJA:
        event_viaja(w, simulationTime, data);
        break;
      case EVENT_MORRE:
        event_morre(w, simulationTime, data);
        break;
      case EVENT_MISSAO:
        event_missao(w, simulationTime, data);
        break;
      case EVENT_FIM:
        event_fim(simulationTime);
        break;
      default:
        printf("Evento desconhecido (%d) ignorado.\n", type);
    }
  
    free(data); //libera o evento
  }
  //=======================================================================================
  
  
  // Impressão status dos heróis
  //=======================================================================================

  for (i=0; i< w->Nheroes; i++){ 
    struct hero *h = w->heroes[i];
    printf("HEROI %2d %s PAC %3d VEL %4d EXP %4d HABS [ ", h->ID, (h->alive ? "VIVO " : "MORTO"), h->pacience, h->speed, h->xp);
    cjto_imprime(h->skills);
    printf(" ]\n");
  }

  //=======================================================================================

  // Impressão status das bases
  //=======================================================================================

  for (i=0; i< w->Nbases; i++){
    struct base *b = w->bases[i];
    printf("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", b->ID, b->capacity, b->maxLine, b->missionCounter);
  }

  //=======================================================================================

  //Estatísticas das missões
  //=======================================================================================

  int compMissions = 0, minTries = -1, maxTries = 0;
  double totalAttempts = 0;

  for (i=0; i< w->Nmissions; i++){ //Acumula o total de tentativas para as missões serem cumpridas
    struct mission *m = w->missions[i];
    if (m->done){
      compMissions++;
      totalAttempts = totalAttempts + m->attempts; 

      if (minTries == -1 || m->attempts < minTries) //Pega o menor número de tentativas de uma missão
        minTries = m->attempts;

      if (m->attempts > maxTries) //Pega o maior número de tentativas de uma missão
        maxTries = m->attempts;
    }
  }

  int deadHeroes = 0; //Acumula o número de heróis falecidos
  for (i=0; i< w->Nheroes; i++)
    if (!w->heroes[i]->alive)
      deadHeroes++;
  
  printf("EVENTOS TRATADOS: %d\n", totalEventos); 
  printf("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", compMissions, w->Nmissions, (100.0*compMissions)/w->Nmissions); //Imprime as missões cumpridas

  if (compMissions > 0){
    printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", minTries, maxTries, totalAttempts/compMissions); //Imprime o menor número de tentativas, o maior e a média de tentativas por missão
  }
  else{
    printf("TENTATIVAS/MISSAO: MIN 0, MAX 0, MEDIA 0.0\n"); //caso nenhuma missão tenha sido completada
  }

  printf("TAXA MORTALIDADE: %.1f%%\n", (100.0*deadHeroes)/w->Nheroes); //taxa de mortalidade dos heróis

  //=======================================================================================

  //Destrói o mundo
  //=======================================================================================

  w = destroy_world(w);

  //======================================================================================

  return (0) ;
}


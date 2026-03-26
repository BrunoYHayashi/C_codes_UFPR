// TAD Fila de prioridades (FPRIO) genérica
// Carlos Maziero, DINF/UFPR, Out 2024
// Marcos Castilho, inclui as structs no arquivo.c, Out 2025
// Implementação com lista encadeada simples
#include <stdio.h>
#include <stdlib.h>
// descreve um nodo da fila de prioridades
// NAO altere estas estruturas
struct fpnodo_t
{
  void *item ;          // item associado ao nodo
  int   tipo ;          // tipo do item
  int   prio ;          // prioridade do item
  struct fpnodo_t *prox;    // próximo nodo
};

// descreve uma fila de prioridades
struct fprio_t
{
  struct fpnodo_t *prim ;   // primeiro nodo da fila
  int num ;         // número de itens na fila
} ;

// A COMPLETAR

struct fprio_t *fprio_cria (){
  struct fprio_t *fprioCriada;
  if (!(fprioCriada=malloc (sizeof(struct fprio_t)))) //tenta alocar de forma dinâmica uma memória para a fila. 
    return NULL; //Caso falhe, retorna NULL

  fprioCriada->prim = NULL; //Primeiro nodo da fila é NULL
  fprioCriada->num = 0; //Número de ítens da fila é 0
  
  return fprioCriada;
}

struct fprio_t *fprio_destroi (struct fprio_t *f){

  if (!f) //verifica se f é válido
    return NULL; //se não for, retorna NULL

  struct fpnodo_t *atual, *prox;

  atual = f->prim;
  
  while (atual != NULL){ //da free para todos os nodos da fila até que atual (nodo atual) seja NULL, nesse caso, não existe mais nodos na fila
    prox = atual->prox;
    free (atual->item); //da free no item do nodo
    free (atual); //da free no atual
    atual = prox;
  }

  free(f); //da free na fila
  return NULL;
}

int fprio_insere (struct fprio_t *f, void *item, int tipo, int prio){
  if (!f || !item) //verifica se fila é válida e se item é válido
    return (-1);

  struct fpnodo_t *verifica = f->prim;
  while (verifica){ //verifica se já existe o item na fila
    if (verifica->item == item)
      return (-1);
    verifica = verifica->prox;
  }

  struct fpnodo_t *novoNodo;

  if (!(novoNodo = malloc (sizeof (struct fpnodo_t)))){
    return (-1);
  }

  novoNodo->item = item;
  novoNodo->tipo = tipo;
  novoNodo->prio= prio;
  novoNodo->prox= NULL;

  if ((!f->prim) || f->prim->prio > prio){ //Verifica se a fila está vazia ou se a prioridade do primeiro é maior que a prioridade nova
    novoNodo->prox= f->prim; //o novo nodo recebe como próximo o primeiro, para o caso dele existir
    f->prim= novoNodo; //o primeiro nodo da fila vira o novo
  }
  else {
    struct fpnodo_t *atual = f->prim; 
    while ((atual->prox) && (atual->prox->prio <= prio)) //enquanto existe um nodo depois do atual, e enquanto a prioridade do próximo do atual for menor, atual vira o próximo
      atual= atual->prox;

    novoNodo->prox= atual->prox; //novoNodo aponta para o de prioridade maior ou para o nada (no caso de ser o último da fila)
    atual->prox= novoNodo; //atual aponta para o novoNodo
  }

  f->num++; //aumenta o tamanho da fila

  return (f->num);
}

void *fprio_retira (struct fprio_t *f, int *tipo, int *prio){
  if ((!f) || (!tipo) || (!prio)) //verifica se os ponteiros de f, tipo e prio são válidos
    return NULL;

  if (!f->prim)
  return NULL;

  struct fpnodo_t *nodoRemovido = f->prim;
  void *item = nodoRemovido->item;

  nodoRemovido = f->prim; 
  f->prim = f->prim->prox;

  *tipo = nodoRemovido->tipo;
  *prio = nodoRemovido->prio;

  free(nodoRemovido);
  f->num--;
  
  return (item);
}

int fprio_tamanho (struct fprio_t *f){
  if (!f)
    return (-1);

  return (f->num);
}

void fprio_imprime (struct fprio_t *f){
  if (!f)
    return;

  struct fpnodo_t *atual = f->prim;

  while (atual) {
    printf ("(%d %d)", atual->tipo, atual->prio);
    atual= atual->prox;
    if (atual)
      printf(" ");
  }
}

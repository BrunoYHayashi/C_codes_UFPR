#include <stdio.h>
#include <stdlib.h>
#include "fprio.h"

// Cria uma fila vazia.
// Retorno: ponteiro para a fila criada ou NULL se erro.
struct fprio_t *fprio_cria (){
    struct fprio_t *filaNova;

    if (!(filaNova=malloc(sizeof(struct fprio_t))))
        return NULL;
    
    filaNova->prim = NULL;
    filaNova->fim = NULL;
    filaNova->num = 0;

    return filaNova;
}

// Libera todas as estruturas de dados da fila, inclusive os itens.
// Retorno: NULL.
struct fprio_t *fprio_destroi (struct fprio_t *f){
    if (!f)
        return NULL;

    struct fpnodo_t *atual, *prox;

    atual = f->prim;

    while (atual != NULL){
        prox= atual->prox;
        free(atual->item);
        atual->item= NULL;
        free(atual);
        atual= prox;
    }

    free(f);
    f= NULL;

    return f;
}



// Insere o item na fila, mantendo-a ordenada por prioridades crescentes.
// Itens com a mesma prioridade devem respeitar a politica FIFO (retirar
// na ordem em que inseriu).
// Inserir duas vezes o mesmo item (o mesmo ponteiro) é um erro.
// Retorno: número de itens na fila após a operação ou -1 se erro.
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

    if(!novoNodo->prox)
        f->fim= novoNodo;

    if (f->num== 0)
        f->fim= f->prim;

    f->num++; //aumenta o tamanho da fila

    return (f->num);
}

// Retira o primeiro item da fila e o devolve; o tipo e a prioridade
// do item são devolvidos nos parâmetros "tipo" e "prio".
// Retorno: ponteiro para o item retirado ou NULL se fila vazia ou erro.
void *fprio_retira (struct fprio_t *f, int *tipo, int *prio){
    if ((!f) || (!tipo) || (!prio)) //verifica se os ponteiros de f, tipo e prio são válidos
        return NULL;

    if (!f->prim) //verifica se o primeiro nodo da lista existe
        return NULL;

    struct fpnodo_t *nodoRemovido = f->prim;
    void *item = nodoRemovido->item;
 
    f->prim = f->prim->prox;

    *tipo = nodoRemovido->tipo;
    *prio = nodoRemovido->prio;

    free(nodoRemovido);
    f->num--;

    if(f->num==0)
        f->fim=NULL;
    
    return (item);
}

// Informa o número de itens na fila.
// Retorno: N >= 0 ou -1 se erro.
int fprio_tamanho (struct fprio_t *f){
    if (!f)
        return (-1);

    return (f->num);
}

// Imprime o conteúdo da fila no formato "(tipo prio) (tipo prio) ..."
// Para cada item deve ser impresso seu tipo e sua prioridade, com um
// espaço entre valores, sem espaços antes ou depois e sem nova linha.
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
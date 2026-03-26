#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
/* coloque demais includes aqui */

/* ------------------- Nao altere estas structs ------------------------- */
struct nodo {
    int chave;
    struct nodo *prox;
};

struct lista {
    struct nodo *ini;
    struct nodo *ptr; /* ponteiro para algum nodo da lista (iterador) */
    int tamanho;

};
/* ---------------------------------------------------------------------- */

struct lista *lista_cria (){
    struct lista *listaNova;
    if (!(listaNova=malloc (sizeof(struct lista))))
        return NULL;

    if (!(listaNova->ini = malloc(sizeof(struct nodo))))
        return NULL;

    listaNova->ini->prox = NULL; 
    listaNova->tamanho = 0;
    listaNova->ptr = NULL;

    return listaNova;
}

void lista_destroi (struct lista **lista){

    struct nodo *atual, *seguinte;

    atual = (*lista)->ini;

    while(atual != NULL){
        seguinte= atual->prox;
        free(atual);
        atual = seguinte;
    }

    free(*lista);
    *lista= NULL;
}

int lista_insere_inicio (struct lista *lista, int chave){
    struct nodo *inicio;

    if (!(inicio=malloc(sizeof(struct nodo))))
        return 0;

    inicio->chave= chave;
    inicio->prox= lista->ini->prox;

    lista->ini->prox= inicio;
    lista->tamanho= lista->tamanho+1;

    return 1;
}

int lista_insere_fim (struct lista *lista, int chave){
    struct nodo *fim;

    if (!(fim=malloc(sizeof(struct nodo))))
        return 0;

    fim->chave=chave;
    fim->prox=NULL;

    lista->ptr = lista->ini;

    while (lista->ptr->prox != NULL)
        lista->ptr = lista->ptr->prox;

    lista->ptr->prox = fim;
    lista->tamanho= lista->tamanho +1;

    return 1;
}

int lista_insere_ordenado (struct lista *lista, int chave){
    struct nodo *novoNodo;

    if (!(novoNodo = malloc(sizeof(struct nodo))))
        return 0;

    novoNodo->chave = chave; 
    lista->ptr = lista->ini;   
    
    while (lista->ptr->prox != NULL && lista->ptr->prox->chave < chave)
        lista->ptr = lista->ptr->prox;

    novoNodo->prox = lista->ptr->prox;
    lista->ptr->prox = novoNodo;

    lista->tamanho = lista->tamanho +1;

    return 1;
}

int lista_remove_inicio (struct lista *lista, int *chave){
    struct nodo *nodoRemovido;

    if (lista->ini->prox == NULL)
        return 0;

    nodoRemovido = lista->ini->prox;
    *chave = nodoRemovido->chave;
    lista->ini->prox = nodoRemovido->prox;
    lista->tamanho = lista->tamanho -1;

    free(nodoRemovido);

    return (1);
}

int lista_remove_fim(struct lista *lista, int *chave) {
    if (!lista || lista->ini->prox == NULL)  
        return 0;

    struct nodo *atual = lista->ini;
    struct nodo *nodoRemovido;

    if (atual->prox->prox == NULL) {
        nodoRemovido = atual->prox;
        *chave = nodoRemovido->chave;
        atual->prox = NULL;  
        free(nodoRemovido);
        lista->tamanho--;
        return 1;
    }

    while (atual->prox->prox != NULL)
        atual = atual->prox;

    nodoRemovido = atual->prox;
    *chave = nodoRemovido->chave;
    atual->prox = NULL;
    free(nodoRemovido);
    lista->tamanho--;

    return 1;
}


int lista_remove_ordenado (struct lista *lista, int chave){

    if ((lista->ini->prox== NULL))
        return 0;

    struct nodo *nodoRemovido;

    lista->ptr = lista->ini;

    while (lista->ptr->prox->chave != chave)
        lista->ptr = lista->ptr->prox;

    nodoRemovido = lista->ptr->prox;
    lista->ptr->prox = nodoRemovido->prox;
    lista->tamanho = lista->tamanho -1;

    free (nodoRemovido);

    return 1;

}

int lista_vazia (struct lista *lista){
    if (lista->ini->prox == NULL)
        return 1;

    return 0;
}

int lista_tamanho (struct lista *lista){
    return (lista->tamanho);
}

int lista_pertence (struct lista *lista, int chave){
    lista->ptr = lista->ini->prox;
    while ((lista->ptr != NULL) ) {
        if (lista->ptr->chave == chave)
            return 1;

        lista->ptr = lista->ptr->prox;
    }

    return 0;
}

void lista_inicia_iterador (struct lista *lista){

    lista->ptr = lista->ini->prox;

}

int lista_incrementa_iterador (struct lista *lista, int *chave){
    
    if (!lista->ptr)
        return 0;

    *chave = lista->ptr->chave;
    lista->ptr = lista->ptr->prox; 

    return 1;
}

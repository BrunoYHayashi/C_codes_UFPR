#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

// Cria uma fila vazia.
// Retorno: ponteiro para a fila criada ou NULL se erro.
struct fila_t *fila_cria (){
	struct fila_t *novaFila;

	if (!(novaFila=malloc(sizeof(struct fila_t)))) //verifica se foi possível alocar uma memória para nova fila
		return NULL;
	
	novaFila->prim = NULL; //ponteiros de novaFila recebem NULL, pois está vazia
	novaFila->fim = NULL; 
	novaFila->num = 0; 

	return novaFila;
}

// Libera todas as estruturas de dados da fila, inclusive os itens.
// Retorno: NULL.
struct fila_t *fila_destroi (struct fila_t *f){

	if(!f) //verifica se f é vazia
		return NULL;

	struct fila_nodo_t *atual= f->prim, *proximo; 

	while (atual != NULL){ //enquanto atual for diferente de nulo (for um nodo da fila)
		proximo= atual->prox;
		free(atual->item); //libera ponteiro do item
		atual->item = NULL;
		free(atual); //libera nodo
		atual= NULL;
		atual= proximo;
	};

	free(f); //libera f
	f = NULL;

	return f;
}

// Insere o item na fila
// Inserir duas vezes o mesmo item (o mesmo ponteiro) é um erro.
// Retorno: número de itens na fila após a operação ou -1 se erro.
int fila_insere (struct fila_t *f, void *item){
	if (!f || !item) //verifica se fila é vazia ou item existe
		return -1;

	struct fila_nodo_t *atual= f->prim; 

	while ((atual!=NULL)){ //Enquanto não encontrar um item igual ao passado ou novoNodo for um nodo existente
		if (atual->item == item)
			return -1;
		
		atual = atual->prox;
	};

	struct fila_nodo_t *novoNodo;

	if (!(novoNodo=malloc(sizeof(struct fila_nodo_t)))) //aloca uma memória para o novoNodo
		return -1;

	novoNodo->item = item;
	novoNodo->prox = NULL;

	if (f->fim != NULL)
		f->fim->prox = novoNodo;
	else
		f->prim = novoNodo;

	f->fim = novoNodo;
	f->num++;

	return f->num;
}

// Retira o primeiro item da fila e o devolve
// Retorno: ponteiro para o item retirado ou NULL se fila vazia ou erro.
void *fila_retira (struct fila_t *f){
	if (!f || !f->prim)
		return NULL;
	
	struct fila_nodo_t *nodoRemovido;
	void *itemRemovido;

	nodoRemovido= f->prim;
	f->prim= f->prim->prox;
	itemRemovido = nodoRemovido->item;

	if (f->prim==NULL)
		f->fim= NULL;

	free(nodoRemovido);
	f->num--;

	return itemRemovido;
}


// Informa o número de itens na fila.
// Retorno: N >= 0 ou -1 se erro.
int fila_tamanho (struct fila_t *f){
	if (!f)
		return -1;
	
	return f->num;
}

/*DÚVIDA SOBRE IMPRESSÃO DE VOID*/
// Imprime o conteúdo da fila 
void fila_imprime (struct fila_t *f){
	if (!f) {
		printf("fila nula ");
		return;
	}

	if (!f->prim){
		printf("fila vazia ");
		return;
	}

	struct fila_nodo_t *atual=f->prim;
	int valor;

	while (atual != NULL){
		valor= *(int *)atual->item;
		printf("%d ", valor);

		atual=atual->prox;
	}
}
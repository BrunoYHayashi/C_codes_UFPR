#include <stdio.h>
#include <stdlib.h>
#include "investimento.h"

int main(int argc, char *argv[]) {

//Le o comando (capital inicial(-c), anos(-i, -f), janelas(-w))
    /*
    Percorre argv; 
    transforma strings em números (int e double); 
    valida se as regras foram seguidas;
    Devolve um ponteiro para struct (Config) que guarda as requisições do usuário
    */
    Config *cfg = obter_configuracao(argc, argv);
//======================================================================================

//Calcula rendimentos mensais, compara desempenhos totais e implementa janelas móveis=
    /*
    Funções para abrir arquivos e carregar dados na memória;
    Define se fará: Comparação Total(0) ou Comparação por Janelas(1); 
    Imprime na tela o resultado;
    */
    executar_programa_juros(cfg);
//======================================================================================

//Liberação de memória e fechamento de arquivos
    liberar_config(cfg);
//======================================================================================

    return 0;
}
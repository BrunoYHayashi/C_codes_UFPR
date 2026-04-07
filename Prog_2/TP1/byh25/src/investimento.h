#ifndef INVESTIMENTO_H
#define INVESTIMENTO_H

#include "config.h"

//Estruturas
//================================================================================
//Representa uma linha do arquivo original (Mes, ano e Taxa)
typedef struct {
    int mes;
    int ano;
    double valor;
} Registro;

//Informações de um arquivo (Nome, se era .tax ou .cot, os registros contidos, quantidade de registros)
typedef struct {
    char nome[100]; //nome do arquivo
    char tipo; //t para .tax, c para .cot
    Registro *registros; //vetor de registros
    int num_registros; //quantidade de registros
} Investimento;
//================================================================================

//Funções auxiliares
//================================================================================
//Traduz o texto mês (jan, fev, mar...) para números (1, 2, 3...)
int mes_para_int(char *s);
//================================================================================

//Funções que lidam com um investimento por vez
//================================================================================
//Abre o arquivo de texto e preenche o vetor de registros
Investimento* carregar_investimento(char *file);

//Libera a memória de um arquivo
void liberar_investimento(Investimento *inv);

//Arquivos estão de 2025 para 2000, inverte de 2000 para 2025, assim o cálculo de juros faz sentido
void inverter_registros(Investimento *inv);

//Aplica a fórmula de juros compostos ou câmbio para o período atual
double calcular_rendimento(Investimento *inv, double capital_inicial, int ano_i, int ano_f);

//Faz o mesmo cálculo, mas focado em uma fração do tempo (janelas)
double calcular_janela_especifica(Investimento *inv, double capital, int ano_base, int mes_offset, int tam_janela);
//================================================================================

//Funções que lidam com mais de um investimento por vez
//================================================================================
//Cria um vetor de ponteiros (Investimento) para que o programa consiga comparar dois ao mesmo tempo
Investimento** carregar_lista_investimentos(Config *cfg, int *carregados);

//Gera a saída visual
void exibir_relatorio_total(Investimento **lista, int num_inv, Config *cfg);

//Libera a memória da lista de investimentos
void liberar_lista_investimentos(Investimento **lista, int num_inv);

//"Desliza" período mês a mês e conta quem venceu em cada janela
void processar_janelas(Investimento **lista, int num_inv, Config *cfg);

//Função chamada pela main, decide se será Relatório Total ou Janelas Temporais
void executar_programa_juros(Config *cfg);
//================================================================================

#endif
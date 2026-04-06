#ifndef INVESTIMENTO_H
#define INVESTIMENTO_H

//Struct para um mês de dados
typedef struct {
    int mes;
    int ano;
    double valor;
} Registro;

//Struct para o investimento completo
typedef struct {
    char nome[100]; //nome do arquivo
    char tipo; //t para .tax, c para .cot
    Registro *registros; //vetor de registros
    int num_registros; //quantidade de meses no arquivo
} Investimento;

//Funções de carregar e liberar investimento
Investimento* carregar_investimento(char *file);
void liberar_investimento(Investimento *inv);
void inverter_registros(Investimento *inv);
double calcular_rendimento(Investimento *inv, double capital_inicial, int ano_i, int ano_f);

#endif
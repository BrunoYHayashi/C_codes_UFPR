#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "investimento.h"

//Função para converter nome do mês para número
int mes_para_int(char *s) {
    if (strncasecmp(s, "jan", 3) == 0) return 1; 
    if (strncasecmp(s, "fev", 3) == 0) return 2; 
    if (strncasecmp(s, "mar", 3) == 0) return 3; 
    if (strncasecmp(s, "abr", 3) == 0) return 4; 
    if (strncasecmp(s, "mai", 3) == 0) return 5; 
    if (strncasecmp(s, "jun", 3) == 0) return 6; 
    if (strncasecmp(s, "jul", 3) == 0) return 7; 
    if (strncasecmp(s, "aho", 3) == 0) return 8; 
    if (strncasecmp(s, "set", 3) == 0) return 9; 
    if (strncasecmp(s, "out", 3) == 0) return 10; 
    if (strncasecmp(s, "nov", 3) == 0) return 11; 
    if (strncasecmp(s, "dez", 3) == 0) return 12; 
    return 0;
}

Investimento* carregar_investimento(char *file) {
    FILE *f;
    if (!(f = fopen(file, "r"))) 
        return NULL;
    
    Investimento *inv;
    inv = malloc (sizeof(Investimento));
    strncpy(inv->nome, file, 99);

    //Identifica o tipo pela extensão (.tax/ .cot)
    if (strstr(file, ".tax"))
        inv->tipo = 't';
    else
        inv->tipo = 'c';

    inv->num_registros = 0;
    inv->registros = NULL;

    char linha[256];
    fgets(linha, sizeof(linha), f); //pula cabeçalho

    char m_str[20], valor_str[50];
    int ano;

    while(fscanf(f, " %[^/]/%d %s", m_str, &ano, valor_str) == 3) {
        //Aloca espaço para mais um registro
        inv->num_registros++;
        inv->registros = realloc(inv->registros, sizeof(Registro) * inv->num_registros);

        //Trata o ano (25 vira 2025...)
        if (ano < 100)
            ano += 2000;

        //Trata a vírgula no valor
        for (int i = 0; valor_str[i]; i++)
            if (valor_str[i] == ',') valor_str[i] = '.';

        //Salva dados na struct
        int idx = inv->num_registros -1;
        inv->registros[idx].mes= mes_para_int(m_str);
        inv->registros[idx].ano = ano;
        inv->registros[idx].valor = atof(valor_str);
    }

    fclose(f);
    return inv;
}

void liberar_investimento(Investimento *inv) {
    //VErificação se o ponteiro é nulo
    if (inv == NULL) {
        return;
    }

    //Liberar o vetor de registros; liberar dados internos antes de liberar struct pai
    if (inv->registros != NULL) {
        free(inv->registros);
    }

    free(inv);
}

// Inverte o vetor para ficar Jan/2000 -> Dez/2025
void inverter_registros(Investimento *inv) {
    int i = 0;
    int j = inv->num_registros - 1;
    while (i < j) {
        Registro temp = inv->registros[i];
        inv->registros[i] = inv->registros[j];
        inv->registros[j] = temp;
        i++; j--;
    }
}

double calcular_rendimento(Investimento *inv, double capital_inicial, int ano_i, int ano_f) {
    double capital_atual = capital_inicial;
    double cotacao_inicial = -1.0;
    double cotacao_final = 0.0;

    for (int i = 0; i < inv->num_registros; i++) {
        Registro r = inv->registros[i];

        // Filtra pelo período selecionado pelo usuário
        if (r.ano >= ano_i && r.ano <= ano_f) {
            
            if (inv->tipo == 't') {
                // Cálculo de juros compostos: capital = capital * (1 + taxa_mensal)
                // taxa_mensal = (1 + taxa_anual/100)^(1/12) - 1
                double taxa_mensal = pow(1.0 + (r.valor / 100.0), 1.0/12.0) - 1.0;
                capital_atual *= (1.0 + taxa_mensal);
            } 
            else {
                // Cálculo de cotação (.cot)
                // Se for o primeiro registro do período, guarda a cotação de "compra"
                if (cotacao_inicial < 0) {
                    cotacao_inicial = r.valor;
                }
                // Guarda sempre a última cotação vista no período (cotação de "venda")
                cotacao_final = r.valor;
            }
        }
    }

    // Se for cotação, faz a conversão final: (Capital / Compra) * Venda
    if (inv->tipo == 'c' && cotacao_inicial > 0) {
        capital_atual = (capital_inicial / cotacao_inicial) * cotacao_final;
    }

    return capital_atual;
}
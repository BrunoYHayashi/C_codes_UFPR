#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

//Aloca vetor de ponteiros para strings
char** alocar_lista_arquivos(int n) {
    char **lista = malloc(sizeof(char *) * n);
    if (!lista) {
        fprintf(stderr, "Erro fatal: Falha na alocação de memória.\n");
        exit(1);
    }
    return lista;
}

//Valida comandos
void validar_config(Config *cfg) {
    //Verifica se há capital inicial (obrigatório)
    if (cfg->capital_inicial < 0) {
        fprintf(stderr, "Erro: Capital inicial (-c) é obrigatório.\n");
        liberar_config(cfg);
        exit(1);
    }

    //Verifica se pelo menos um arquivo é informado (obrigatório)
    if (cfg->num_arquivos == 0) {
        fprintf(stderr, "Erro: Informe pelo menos um arquivo de investimento.\n");
        liberar_config(cfg);
        exit(1);
    }

    // Regras da Janela (se -w: pelo menos dois arquivos informados; tamanho da janela entre 6 e 12 meses)
    if (cfg->janela > 0) {
        if (cfg->num_arquivos < 2) {
            fprintf(stderr, "Erro: O uso de janelas (-w) exige pelo menos dois arquivos.\n");
            liberar_config(cfg);
            exit(1);
        }
    }
}

Config* processar_argumentos(int argc, char *argv[], Config *cfg){
    cfg->capital_inicial = -1.0;
    cfg->ano_inicio = 2000;
    cfg->ano_fim = 2025;
    cfg->janela = 0;
    cfg->num_arquivos = 0;
    cfg->escritaEnable = 0;

    cfg->arquivos = alocar_lista_arquivos(argc);
    cfg->escrita = alocar_lista_arquivos(argc);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && (i + 1 < argc)) {
            cfg->capital_inicial = atof(argv[++i]);
        } else if (strcmp(argv[i], "-i") == 0 && (i + 1 < argc)) {
            cfg->ano_inicio = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0 && (i + 1 < argc)) {
            cfg->ano_fim = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-w") == 0 && (i + 1 < argc)) {
            cfg->janela = atoi(argv[++i]);   
        } else if (strcmp(argv[i], "-a") == 0 && (i + 1 < argc)) {
            cfg->escrita[0] = argv[++i];   
            cfg->escritaEnable = 1;
        }
        else {
            cfg->arquivos[cfg->num_arquivos++] = argv[i];
        }
    }

    validar_config(cfg);

    return cfg;
}

void liberar_config(Config *cfg) {
    if (cfg == NULL) return;

    if (cfg->arquivos != NULL) {
        free(cfg->arquivos);
        cfg->arquivos = NULL;
    }

    free(cfg);
}

Config* obter_configuracao(int argc, char *argv[]){
    Config *cfg = malloc(sizeof(Config));

    cfg = processar_argumentos(argc, argv, cfg);

    return cfg;
}

int obter_valor_janela(Config *cfg) {
    return cfg->janela;
}

double obter_capital_inicial(Config *cfg) {
    return cfg->capital_inicial;
}
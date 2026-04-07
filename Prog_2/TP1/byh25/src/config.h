#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    double capital_inicial;
    int ano_inicio;
    int ano_fim;
    int janela;
    char **arquivos;
    int num_arquivos;
} Config;

//
char** alocar_lista_arquivos(int n);

//
void validar_config(Config *cfg);

//Função para processar argumentos e retornar struct preenchida
Config* processar_argumentos(int argc, char *argv[], Config *cfg);

//Função para liberar vetor de nomes de arquivos dentro da Config
void liberar_config(Config *cfg);

//
Config* obter_configuracao(int argc, char *argv[]);

//
int obter_valor_janela(Config *cfg);

//
double obter_capital_inicial(Config *cfg);
#endif
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double capital_inicial;
    int ano_inicio;
    int ano_fim;
    int janela;
    char **arquivos;
    int num_arquivos
} Config;

int main(int argc, char *argv[]) {
    //Cria e inicializa struct com valores padrão
    Config cfg;
    cfg.capital_inicial = 0.0;
    cfg.ano_inicio = 2000;
    cfg.ano_fim = 2025;
    cfg.janela = 0;
    cfg.num_arquivos = 0;

    //Aloca espaço para os nomes do arquivo (até 'argc')
    cfg.arquivos = malloc(sizeof(char *) *argc);

    //Loop para percorrer argv
    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && (i + 1 < argc)) {
            cfg.capital_inicial = atof(argv[i++]);
        }

        else if (strcmp(argv[i], "-i") == 0 && (i + 1 < argc)) {
            cfg.ano_inicio = atoi(argv[i++]);
        }

        else if (strcmp(argv[i], "-f") == 0 && (i + 1 < argc)) {
            cfg.ano_fim = atoi(argv[++i]);
        } 

        else if (strcmp(argv[i], "-w") == 0 && (i + 1 < argc)) {
            cfg.janela = atoi(argv[++i]);
        } 

        else {
            cfg.arquivos[cfg.num_arquivos] = argv[i];
            cfg.num_arquivos++;
        }
    }

    //Validação 
    //Capital inicial (-c) é obrigatório
    if (cfg.capital_inicial < 0) {
        printf("Erro: Capital inicial (-c) é obrigatório.\n");
        free(cfg.arquivos);
        return 1;
    }

    //É necessário pelo menos 1 arquivo para qualquer cálculo
    if (cfg.num_arquivos == 0) {
        printf("Erro: Pelo menos um arquivo deve ser informado.\n");
        free(cfg.arquivos);
        return 1;
    }

    //Regra da Janela (-w), se usada, precisa de 2 ou mais arquivos
    if (cfg.janela > 0) {
        if (cfg.num_arquivos < 2) {
            fprintf(stderr, "Erro: Para usar janelas (-w), informe pelo menos dois arquivos. \n");
            free(cfg.arquivos);
            return 1;
        }

        //Validação extra (janela entre 6 e 12 meses)
        if (cfg.janela < 6 || cfg.janela > 12) {
            fprintf(stderr, "Erro: o tamanho da janela (-w) deve ser entre 6 e 12 meses. \n");
            free(cfg.arquivos);
            return 1;
        }
    }

    //TESTE 
    printf("Capital: %.2f | Inicio: %d | Fim: %d | Janela: %d\n", 
            cfg.capital_inicial, cfg.ano_inicio, cfg.ano_fim, cfg.janela);
    printf("Arquivos encontrados: %d\n", cfg.num_arquivos);
    for(int j=0; j < cfg.num_arquivos; j++) {
        printf(" - %s\n", cfg.arquivos[j]);
    }

    free(cfg.arquivos);
    return 0;
}
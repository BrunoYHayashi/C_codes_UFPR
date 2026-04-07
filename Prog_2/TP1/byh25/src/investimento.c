#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "investimento.h"

//Função para converter nome do mês para número
int mes_para_int(char *s) {

    if (strncmp(s, "jan", 3) == 0) return 1; 
    if (strncmp(s, "fev", 3) == 0) return 2; 
    if (strncmp(s, "mar", 3) == 0) return 3; 
    if (strncmp(s, "abr", 3) == 0) return 4; 
    if (strncmp(s, "mai", 3) == 0) return 5; 
    if (strncmp(s, "jun", 3) == 0) return 6; 
    if (strncmp(s, "jul", 3) == 0) return 7; 
    if (strncmp(s, "ago", 3) == 0) return 8; 
    if (strncmp(s, "set", 3) == 0) return 9; 
    if (strncmp(s, "out", 3) == 0) return 10; 
    if (strncmp(s, "nov", 3) == 0) return 11; 
    if (strncmp(s, "dez", 3) == 0) return 12; 
    return 0;
}

//Inverte o vetor para ficar Jan/2000 -> Dez/2025
void inverter_registros(Investimento *inv) {
    int i = 0;
    int j = inv->num_registros -1;
    while (i < j) {
        Registro temp = inv->registros[i];
        inv->registros[i] = inv->registros[j];
        inv->registros[j] = temp;
        i++; 
        j--;
    }
}

//Abre o arquivo de texto e preenche o vetor de registros
Investimento* carregar_investimento(char *file) {
    FILE *f;
     
    //tenta abrir o arquivo
    if (!(f = fopen(file, "r"))) 
        return NULL;
    
    //Aloca memória para um investimento (arquivo inteiro)
    Investimento *inv = malloc(sizeof(Investimento));
    inv->num_registros = 0;
    inv->registros = NULL;

    //Copia o nome do arquivo para o campo (nome) do investimento
    strcpy(inv->nome, file);

    //Verifica se é .tax ou .cot e envia o tipo para a struct
    if (strstr(file, ".tax") != NULL) 
        inv->tipo = 't';
    else 
        inv->tipo = 'c';

    //Lê uma linha do arquivo
    char linha[256];
    fgets(linha, sizeof(linha), f);

    //Variáveis para guardar o mês, valor e ano
    char m_str[5], valor_str[50];
    int ano;

    //Le linha por linha
    while (fgets(linha, sizeof(linha), f)) {
        //%[a-zA-Z.] lê letras e o ponto (jan.)
        //%*[/ ] pula a barra OU o espaço se houver
        //Se conseguiu ler os 3 valores corretamente, aumenta o número e vetor de registros.
        if (sscanf(linha, " %4[a-zA-Z.]%*[/ ]%d %s", m_str, &ano, valor_str) >= 3) {
            inv->num_registros++;
            inv->registros = realloc(inv->registros, sizeof(Registro) * inv->num_registros);

            //Se ano vier com duas casas decimais, vira 20XX
            if (ano < 100) ano += 2000;

            //Conversão de "," para "."
            for (int i = 0; valor_str[i]; i++)
                if (valor_str[i] == ',') valor_str[i] = '.';

            //Pega a posição do último registro
            int idx = inv->num_registros -1;
            inv->registros[idx].mes = mes_para_int(m_str); //Transforma mes para inteiro
            inv->registros[idx].ano = ano; //guarda o ano
            inv->registros[idx].valor = atof(valor_str); //Converte string para double
        }
    }

    //Inverte a ordem dos registros, para facilidar cálculo financeiro
    inverter_registros(inv);

    fclose(f);
    return inv;
}

//Libera a memória de um arquivo
void liberar_investimento(Investimento *inv) {
    if (inv == NULL) {
        return;
    }

    if (inv->registros != NULL) {
        free(inv->registros);
    }

    free(inv);
}

//Aplica a fórmula de juros compostos ou câmbio para o período atual
double calcular_rendimento(Investimento *inv, double capital_inicial, int ano_i, int ano_f) {
    double capital_atual = capital_inicial;
    double cotacao_inicial = -1.0; //preço da compra, -1 serve para "ainda não definido"
    double cotacao_final = 0.0; //preço da venda

    for (int i = 0; i < inv->num_registros; i++) {
        Registro r = inv->registros[i];

        //Valido apenas no período informado
        if (r.ano >= ano_i && r.ano <= ano_f) {
            
            if (inv->tipo == 't') {
                //Cálculo de juros compostos: capital = capital * (1 + taxa_mensal)
                //taxa_mensal = (1 + taxa_anual/100)^(1/12) - 1
                double taxa_mensal; 
                taxa_mensal = pow(1.0 + r.valor, 1.0/12.0) - 1.0;
                capital_atual *= (1.0 + taxa_mensal);
            } 
            else {
                //Cálculo de cotação (.cot)
                //Se for o primeiro registro do período, guarda a cotação de "compra"
                if (cotacao_inicial < 0) {
                    cotacao_inicial = r.valor;
                }
                //Guarda sempre a última cotação vista no período (cotação de "venda")
                cotacao_final = r.valor;
            }
        }
    }

    //Se for cotação, faz a conversão final: (Capital / Compra) * Venda
    if (inv->tipo == 'c' && cotacao_inicial > 0) {
        capital_atual = (capital_inicial / cotacao_inicial) * cotacao_final;
    }

    return capital_atual;
}

//Rendimento em uma janela móvel de meses
double calcular_janela_especifica(Investimento *inv, double capital, int ano_base, int mes_offset, int tam_janela) {
    double cap_atual = capital;
    double cot_ini = -1.0;
    int meses_contados = 0;

    for (int i = 0; i < inv->num_registros; i++) {
        //Calcula quantos meses este registro está distante do início do período (Jan/ano_base)
        int m_relativo = (inv->registros[i].ano - ano_base) * 12 + (inv->registros[i].mes - 1);

        if (m_relativo >= mes_offset && m_relativo < (mes_offset + tam_janela)) {
            if (inv->tipo == 't') {
                double tx_m = pow(1.0 + inv->registros[i].valor, 1.0/12.0) - 1.0;
                cap_atual *= (1.0 + tx_m);
            } else {
                if (cot_ini < 0) cot_ini = inv->registros[i].valor;
                cap_atual = (capital / cot_ini) * inv->registros[i].valor;
            }
            meses_contados++;
        }
    }
    //Se não encontrou dados suficientes para a janela completa, retorna um valor nulo
    if (meses_contados < tam_janela) return -1.0;
    return cap_atual;
}

Investimento** carregar_lista_investimentos(Config *cfg, int *carregados) {
    Investimento **lista = malloc(sizeof(Investimento*) * cfg->num_arquivos);
    *carregados = 0;
    for (int i = 0; i < cfg->num_arquivos; i++) {
        Investimento *inv = carregar_investimento(cfg->arquivos[i]);
        if (inv) lista[(*carregados)++] = inv;
    }
    return lista;
}

void exibir_relatorio_total(Investimento **lista, int num_inv, Config *cfg) {
    printf("----------------------------------------\n");
    printf("Período: Janeiro %d a Dezembro %d\n", cfg->ano_inicio, cfg->ano_fim);
    printf("Capital Inicial: R$ %.2f\n", cfg->capital_inicial);
    
    for (int i = 0; i < num_inv; i++) {
        double res = calcular_rendimento(lista[i], cfg->capital_inicial, cfg->ano_inicio, cfg->ano_fim);
        
        char nome_limpo[100];

        char *inicio = strrchr(lista[i]->nome, '/');
        if (inicio) 
            inicio ++;
        else
            inicio = lista[i]->nome;
            
        strcpy(nome_limpo, inicio);

        char *ponto = strchr(nome_limpo, '.');
        if (ponto) *ponto = '\0';

        nome_limpo[0] = toupper(nome_limpo[0]);

        printf("%s: R$ %.2f\n", nome_limpo, res);
    }
    printf("----------------------------------------\n");
}

void liberar_lista_investimentos(Investimento **lista, int num_inv) {
    for (int i = 0; i < num_inv; i++) {
        liberar_investimento(lista[i]);
    }
    free(lista);
}

void processar_janelas(Investimento **lista, int num_inv, Config *cfg) {
    int vitorias[num_inv];
    for (int i = 0; i < num_inv; i++) vitorias[i] = 0;

    int meses_totais = (cfg->ano_fim - cfg->ano_inicio + 1) * 12;

    //Desliza a janela mês a mês (sobrepostas)
    for (int m = 0; m <= meses_totais - cfg->janela; m++) {
        int vencedor = -1;
        double melhor_resultado = -1.0;

        for (int i = 0; i < num_inv; i++) {
            double res = calcular_janela_especifica(lista[i], cfg->capital_inicial, cfg->ano_inicio, m, cfg->janela);
            
            if (res > melhor_resultado) {
                melhor_resultado = res;
                vencedor = i;
            }
        }
        if (vencedor != -1) vitorias[vencedor]++;
    }

    //Saída formatada (Item 8 do PDF)
    printf("----------------------------------------\n");
    printf("Período: Janeiro %d a Dezembro %d\n", cfg->ano_inicio, cfg->ano_fim);
    printf("Tamanho da janela: %d meses\n", cfg->janela);
    for (int i = 0; i < num_inv; i++) {
        //Remove a extensão para imprimir o nome limpo
        char nome[100];
        strcpy(nome, lista[i]->nome);
        char *p = strchr(nome, '.'); if (p) *p = '\0';
        printf("%s: %d janelas\n", nome, vitorias[i]);
    }
    printf("----------------------------------------\n");
}

void executar_programa_juros(Config *cfg) {
    int carregados = 0;
    Investimento **lista = carregar_lista_investimentos(cfg, &carregados);

    if (cfg->janela == 0)
        exibir_relatorio_total(lista, carregados, cfg);
    else
        processar_janelas(lista, carregados, cfg);

    liberar_lista_investimentos(lista, carregados);
}
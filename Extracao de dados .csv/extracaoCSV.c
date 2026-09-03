#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAM_LINHA_CSV 2048
#define MAX_CIDADES_ESTADO 1000
#define MAX_COLUNAS 9

#define COLUNA_UF 2
#define COLUNA_CIDADE 4
#define COLUNA_VALOR 8

char *remover_aspas(char *texto) {
    if (texto[0] == '"')
        texto++;

    int tamanho = strlen(texto);

    if (tamanho > 0 && texto[tamanho - 1] == '"') {
        texto[tamanho - 1] = '\0';
    }

    return texto;
}

double extrair_dados_linha(char *linha_csv, char *uf_destino, char *cidade_destino) {

    linha_csv[strcspn(linha_csv, "\r\n")] = '\0';

    char *colunas[MAX_COLUNAS];
    char *texto = linha_csv;
    int qtd_colunas = 0;

    while (qtd_colunas < MAX_COLUNAS) {

        char *separador = strchr(texto, ';');

        if (separador != NULL)
            *separador = '\0';

        colunas[qtd_colunas++] = remover_aspas(texto);

        if (separador == NULL)
            break;

        texto = separador + 1;
    }

    if (qtd_colunas < MAX_COLUNAS) {
        return 0.0;
    }

    if (uf_destino != NULL)
        strcpy(uf_destino, colunas[COLUNA_UF]);

    if (cidade_destino != NULL)
        strcpy(cidade_destino, colunas[COLUNA_CIDADE]);

    char valor_str[32];

    strcpy(valor_str, colunas[COLUNA_VALOR]);

    for (int i = 0; valor_str[i] != '\0'; i++) {
        if (valor_str[i] == ',')
            valor_str[i] = '.';
    }

    return atof(valor_str);
}

int obter_indice_cidade(const char *cidade, char nomes[][100], int *total_cidades) {

    for (int i = 0; i < *total_cidades; i++) {

        if (strcmp(nomes[i], cidade) == 0) {
            return i;
        }
    }

    int nova_posicao = *total_cidades;

    strcpy(nomes[nova_posicao], cidade);

    (*total_cidades)++;

    return nova_posicao;
}

void escrever_relatorio_arquivo(
    FILE *arquivo,
    const char *uf_alvo,
    const char *cidade_alvo,
    int qtd_br,
    double soma_br,
    int qtd_uf,
    double soma_uf,
    int qtd_cid,
    double soma_cid,
    char nomes_cidades[][100],
    double somas_cidades[],
    int qtds_cidades[],
    int total_cidades
) {

    fprintf(arquivo, "=== BRASIL ===\n");

    fprintf(
        arquivo,
        "Total Pagamentos: %d | Total Pago: R$ %.2f | Media: R$ %.2f\n\n",
        qtd_br,
        soma_br,
        qtd_br > 0 ? soma_br / qtd_br : 0.0
    );

    fprintf(arquivo, "=== ESTADO E CIDADE ===\n");

    if (qtd_uf > 0) {

        fprintf(
            arquivo,
            "Estado (%s) -> Total Pagamentos: %d | Total Pago: R$ %.2f | Media: R$ %.2f\n",
            uf_alvo,
            qtd_uf,
            soma_uf,
            soma_uf / qtd_uf
        );

    } else {

        fprintf(
            arquivo,
            "Estado (%s) -> Nenhum registro encontrado.\n",
            uf_alvo
        );
    }

    if (qtd_cid > 0) {

        fprintf(
            arquivo,
            "Cidade (%s) -> Total Pagamentos: %d | Total Pago: R$ %.2f | Media: R$ %.2f\n",
            cidade_alvo,
            qtd_cid,
            soma_cid,
            soma_cid / qtd_cid
        );

    } else {

        fprintf(
            arquivo,
            "Cidade (%s) -> Nenhum registro encontrado.\n",
            cidade_alvo
        );
    }

    if (total_cidades > 0) {

        int idx_maior = 0;
        int idx_menor = 0;

        for (int i = 1; i < total_cidades; i++) {

            if (somas_cidades[i] > somas_cidades[idx_maior])
                idx_maior = i;

            if (somas_cidades[i] < somas_cidades[idx_menor])
                idx_menor = i;
        }

        fprintf(
            arquivo,
            "\n=== RANKING DO ESTADO (%s) ===\n",
            uf_alvo
        );

        fprintf(
            arquivo,
            "CIDADE QUE MAIS RECEBEU: %s\n",
            nomes_cidades[idx_maior]
        );

        fprintf(
            arquivo,
            "Total: R$ %.2f | Qtd Pagamentos: %d | Media: R$ %.2f\n\n",
            somas_cidades[idx_maior],
            qtds_cidades[idx_maior],
            somas_cidades[idx_maior] / qtds_cidades[idx_maior]
        );

        fprintf(
            arquivo,
            "CIDADE QUE MENOS RECEBEU: %s\n",
            nomes_cidades[idx_menor]
        );

        fprintf(
            arquivo,
            "Total: R$ %.2f | Qtd Pagamentos: %d | Media: R$ %.2f\n",
            somas_cidades[idx_menor],
            qtds_cidades[idx_menor],
            somas_cidades[idx_menor] / qtds_cidades[idx_menor]
        );
    }
}

void gerar_relatorio(
    const char *nome_arquivo_csv,
    FILE *arquivo_resultado
) {

    char uf_alvo[3];
    char cidade_alvo[100];

    printf("Digite a UF (ex: BA): ");
    scanf(" %2s", uf_alvo);

    printf("Digite a Cidade (MAIUSCULO e sem acento): ");
    scanf(" %99[^\n]", cidade_alvo);

    FILE *arquivo_csv = fopen(nome_arquivo_csv, "r");

    if (!arquivo_csv) {
        printf("Erro ao abrir o arquivo %s!\n", nome_arquivo_csv);
        return;
    }

    char linha_csv[TAM_LINHA_CSV];
    char uf_atual[3];
    char cidade_atual[100];

    double soma_brasil = 0.0;
    double soma_estado = 0.0;
    double soma_cidade = 0.0;

    int qtd_brasil = 0;
    int qtd_estado = 0;
    int qtd_cidade = 0;

    char nomes_cidades[MAX_CIDADES_ESTADO][100];

    double somas_cidades[MAX_CIDADES_ESTADO] = {0.0};

    int qtds_cidades[MAX_CIDADES_ESTADO] = {0};

    int contador_cidades = 0;

    /* Ignora a primeira linha, que normalmente é o cabecalho */
    fgets(linha_csv, sizeof(linha_csv), arquivo_csv);

    while (fgets(linha_csv, sizeof(linha_csv), arquivo_csv) != NULL) {

        double valor = extrair_dados_linha(
            linha_csv,
            uf_atual,
            cidade_atual
        );

        soma_brasil += valor;
        qtd_brasil++;

        if (strcmp(uf_atual, uf_alvo) == 0) {

            soma_estado += valor;
            qtd_estado++;

            if (strcmp(cidade_atual, cidade_alvo) == 0) {

                soma_cidade += valor;
                qtd_cidade++;
            }

            int pos = obter_indice_cidade(
                cidade_atual,
                nomes_cidades,
                &contador_cidades
            );

            somas_cidades[pos] += valor;
            qtds_cidades[pos]++;
        }
    }

    fclose(arquivo_csv);

    escrever_relatorio_arquivo(
        arquivo_resultado,
        uf_alvo,
        cidade_alvo,
        qtd_brasil,
        soma_brasil,
        qtd_estado,
        soma_estado,
        qtd_cidade,
        soma_cidade,
        nomes_cidades,
        somas_cidades,
        qtds_cidades,
        contador_cidades
    );

    if (qtd_estado == 0) {

        printf(
            "\nAtencao: Nenhum registro encontrado para a UF '%s'.\n",
            uf_alvo
        );

    } else if (qtd_cidade == 0) {

        printf(
            "\nAtencao: Encontrados registros para UF '%s', mas nenhum para '%s'.\n",
            uf_alvo,
            cidade_alvo
        );

    } else {

        printf("\nExtracao concluida em tempo recorde!\n");
    }

    printf("Verifique o arquivo saida.txt para o relatorio completo.\n");
}

int main() {

    const char *arquivo_dados = "BolsaFamilia.csv";

    FILE *arquivo_resultado = fopen("saida.txt", "w");

    if (arquivo_resultado) {

        gerar_relatorio(
            arquivo_dados,
            arquivo_resultado
        );

        fclose(arquivo_resultado);

    } else {

        printf("Erro ao criar o arquivo saida.txt!\n");
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BD "dados.csv"
#define ARQUIVO_ID "last_id.id"

// Estrutura de dados para cada registro
typedef struct {
    double qtd_litros;
    int id;
    char data_criado[11]; // Formato DD/MM/YYYY
    char data_lote[50];   // Nome do arquivo de lote
} Registro;

int total_registros = 0;
long tamanho_arquivo;
const long TAM_STRUCT = sizeof(Registro);

// Limpa a tela do console (compatível com Windows e Linux/Mac)
void clear() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Lista arquivos .txt no diretório atual
void listarArquivos() {
    printf("Arquivos de lote (.txt) disponíveis:\n");
    #ifdef _WIN32
        system("dir *.txt");
    #else
        system("ls *.txt");
    #endif
}

// Exibe logs e mensagens de erro
void syslog(int erro, char *complemento) {
    switch (erro) {
        case 1: printf("\nErro %d: Arquivo não encontrado ou corrompido (%s)", erro, complemento); break;
        case 2: printf("\nErro %d: Não foi possível fazer a leitura (%s)", erro, complemento); break;
        case 5: printf("\nErro %d: GRAVE! (%s)", erro, complemento); break;
        case 6: printf("\nLOG %d: Criando um novo arquivo (%s)", erro, complemento); break;
        default: printf("\nErro Desconhecido: %s", complemento); break;
    }
}

// Carrega o arquivo de dados e conta os registros
int carregarArquivoDados() {
    FILE *bd;
    bd = fopen(BD, "r");
    total_registros = 0;

    if (bd == NULL) {
        syslog(1, BD);
        syslog(6, BD);
        bd = fopen(BD, "w");
        if (bd == NULL) {
            syslog(5, BD);
            return 0;
        }
    } else {
        fseek(bd, 0, SEEK_END);
        tamanho_arquivo = ftell(bd);
        rewind(bd);
        char c;
        while ((c = fgetc(bd)) != EOF) {
            if (c == '\n') total_registros++;
        }
    }
    fclose(bd);
    return 1;
}

// Lê e atualiza o último ID utilizado
int verificarUltimoId(int acrescentar) {
    FILE *arquivo_id;
    int id = 1;
    arquivo_id = fopen(ARQUIVO_ID, "r");
    if (arquivo_id != NULL) {
        if (fscanf(arquivo_id, "%d", &id) != 1) {
            syslog(2, ARQUIVO_ID);
            exit(1);
        }
        fclose(arquivo_id);
    }
    if (acrescentar != 0) {
        arquivo_id = fopen(ARQUIVO_ID, "w");
        if (arquivo_id == NULL) {
            syslog(5, ARQUIVO_ID);
            exit(1);
        } else {
            fprintf(arquivo_id, "%d", id + acrescentar);
            fclose(arquivo_id);
        }
    }
    return id;
}

// Cadastra um novo lote de dados a partir de um arquivo .txt
void cadastrarLote() {
    char nome_arquivo_lote[50];
    int c, linhas = 0;
    FILE *arquivo_lote;
    FILE *arquivo_bd;

    do {
        clear();
        listarArquivos();
        printf("\nDigite o nome do arquivo a ser importado: ");
        scanf("%49s", nome_arquivo_lote);
        arquivo_lote = fopen(nome_arquivo_lote, "r");
        if (arquivo_lote == NULL) {
            printf("\nArquivo '%s' não encontrado. Tente novamente.", nome_arquivo_lote);
            getchar();
            getchar();
        }
    } while (arquivo_lote == NULL);

    clear();
    while ((c = fgetc(arquivo_lote)) != EOF) {
        if (c == '\n') linhas++;
    }

    rewind(arquivo_lote);
    Registro *temp = malloc(linhas * sizeof(Registro));

    int ultimoIdSalvo = verificarUltimoId(0);
    int i = 0;
    while (i < linhas && fscanf(arquivo_lote, "%10s %lf", temp[i].data_criado, &temp[i].qtd_litros) == 2) {
        temp[i].id = ultimoIdSalvo;
        strcpy(temp[i].data_lote, nome_arquivo_lote);
        i++;
        ultimoIdSalvo++;
    }
    fclose(arquivo_lote);

    verificarUltimoId(i);

    arquivo_bd = fopen(BD, "a");
    if (arquivo_bd == NULL) {
        syslog(5, BD);
        exit(1);
    }

    for (int j = 0; j < i; j++) {
        fprintf(arquivo_bd, "%s %d %s %.2lf\n",
                temp[j].data_criado,
                temp[j].id,
                temp[j].data_lote,
                temp[j].qtd_litros);
    }

    fclose(arquivo_bd);
    free(temp);
    carregarArquivoDados();
}

// Função para excluir registros de um lote específico
void excluirPorLote() {
    char lote_para_excluir[50];
    FILE *arquivo_bd, *arquivo_temp;
    Registro registro;
    int registros_removidos = 0;

    printf("\nDigite o nome do lote que deseja excluir (ex: lote1.txt): ");
    scanf("%49s", lote_para_excluir);

    arquivo_bd = fopen(BD, "r");
    if (arquivo_bd == NULL) {
        syslog(1, BD);
        return;
    }

    arquivo_temp = fopen("temp.csv", "w");
    if (arquivo_temp == NULL) {
        syslog(5, "temp.csv");
        fclose(arquivo_bd);
        return;
    }

    while (fscanf(arquivo_bd, "%10s %d %49s %lf",
                  registro.data_criado, &registro.id, registro.data_lote, &registro.qtd_litros) == 4) {
        if (strcmp(registro.data_lote, lote_para_excluir) != 0) {
            fprintf(arquivo_temp, "%s %d %s %.2lf\n",
                    registro.data_criado, registro.id, registro.data_lote, registro.qtd_litros);
        } else {
            registros_removidos++;
        }
    }

    fclose(arquivo_bd);
    fclose(arquivo_temp);

    remove(BD);
    rename("temp.csv", BD);

    if (registros_removidos > 0) {
        printf("\n%d registro(s) do lote '%s' foram removidos com sucesso!\n", registros_removidos, lote_para_excluir);
    } else {
        printf("\nNenhum registro encontrado para o lote '%s'.\n", lote_para_excluir);
    }

    carregarArquivoDados();
}

// Exemplo de um menu principal simples
int main() {
    int opcao;
    carregarArquivoDados();
    do {
        printf("\n=== Menu Principal ===\n");
        printf("1 - Cadastrar Lote\n");
        printf("2 - Listar Arquivos de Lote\n");
        printf("3 - Excluir por Lote\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                cadastrarLote();
                break;
            case 2:
                listarArquivos();
                break;
            case 3:
                excluirPorLote();
                break;
            case 0:
                printf("\nEncerrando o programa...\n");
                break;
            default:
                printf("\nOpção inválida!\n");
        }
    } while (opcao != 0);

    return 0;
}

#include "../../include/interactive_utils.h"
#include "../../include/Queries/querie1.h"
#include "../../include/Queries/querie2.h"
#include <stdio.h>
#include <stdlib.h>   
#include <stdbool.h>
#include <unistd.h>  
#include <time.h>
#include <string.h>

void clear_screen() {
    printf("\033[H\033[J");
}

char *get_valid_id() {
    char *id = malloc(256);
    printf("Introduza o ID do utilizador ou artista (ex: U0000001, A0000001): ");
    fgets(id, 256, stdin);
    id[strcspn(id, "\n")] = '\0'; 
    return id;
}

char *get_valid_id6() {
    char *id = malloc(256);
    printf("Introduza o ID do utilizador ou artista (ex: U0000001): ");
    fgets(id, 256, stdin);
    id[strcspn(id, "\n")] = '\0'; 
    return id;
}

int get_valid_int(const char *prompt) {
    char input[16];
    int number;
    while (true) {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0'; // Remove newline residual
            if (sscanf(input, "%d", &number) == 1 && number > 0) {
                return number;
            } else {
                printf("\033[1;31mErro: Entrada inválida. Insira um número maior que 0.\033[0m\n");
            }
        }
    }
}

char *get_country_filter() {
    char *country = malloc(128);
    printf("Introduza o filtro de país (pressione Enter para ignorar): ");
    fgets(country, 128, stdin);
    country[strcspn(country, "\n")] = '\0'; 
    if (strlen(country) == 0) {
        free(country);
        return NULL;
    }
    return country;
}

void print_border(const char *title) {
    int length = 50;
    printf("\n");
    for (int i = 0; i < length; i++) printf("=");
    printf("\n");
    printf("|| %-44s ||\n", title);
    for (int i = 0; i < length; i++) printf("=");
    printf("\n");
}

void show_menu() {
    clear_screen();
    print_border("MENU INTERATIVO");
    printf("\033[1;36m");
    printf("1. Query 1 - Resumo de Utilizador/Artista\n");
    printf("2. Query 2 - Top N Artistas por Discografia\n");
    printf("3. Query 3 - Géneros mais Populares por Faixa Etária\n");
    printf("4. Query 4 - Artista no Top 10 mais vezes\n");
    printf("5. Query 5 - Recomendação de Utilizadores\n");
    printf("6. Query 6 - Resumo Anual de Utilizador\n");
    printf("7. Ajuda\n");
    printf("0. Sair\n");
    printf("\033[0m");
    printf("-----------------------\n");
}

bool load_data(STORAGE *storage, const char *dataset_path) {
    (void) storage;
    (void) dataset_path;

    clear_screen();
    print_border("A CARREGAR DADOS");
    
    printf("A preparar o dataset. Por favor aguarde...\n");
    countdown(3);

    printf("\033[1;32mDados carregados com sucesso!\033[0m\n");
    return true;
}

void process_query(int query, STORAGE *storage) {
    (void) storage; 
    printf("DEBUG: process_query chamado com query = %d\n", query); 
    clear_screen();

    switch (query) {
        case 1: { 
            print_border("A PROCESSAR A QUERY 1");
            char *id = get_valid_id();

            if (!id || strlen(id) < 2 || (id[0] != 'U' && id[0] != 'A')) {
                printf("\033[1;31mErro: ID inválido! Tente novamente.\033[0m\n");
                free(id);
                break;
            }

            printf("\033[1;32mA processar o resumo para o ID: %s\033[0m\n", id);

            query_q1(storage, id, true, stdout);

            free(id);
            break;
        }
        case 2: { 
            print_border("A PROCESSAR A QUERY 2");
            int N = get_valid_int("Introduza o número de artistas a listar (N): ");
            char *country = get_country_filter();

            char temp_file[] = "temp_query2_output.txt";
            query_q2(storage, N, country, false, temp_file);

            FILE *file = fopen(temp_file, "r");
            if (file) {
                char line[256];
                printf("\n\033[1;33mResultados da Query 2:\033[0m\n");
                while (fgets(line, sizeof(line), file)) {
                    printf("%s", line);
                }
                fclose(file);
                remove(temp_file); 
            } else {
                printf("\033[1;31mErro ao abrir o arquivo temporário para leitura.\033[0m\n");
            }

            if (country) free(country);
            break;
        }
        case 3: { 
            print_border("A PROCESSAR A QUERY 3");

            int min_age = get_valid_int("Introduza a idade mínima: ");
            int max_age = get_valid_int("Introduza a idade máxima: ");

            char temp_file[] = "temp_query3_output.txt";

            query_q3(storage, min_age, max_age, false, temp_file);

            FILE *file = fopen(temp_file, "r");
            if (file) {
                char line[256];
                printf("\n\033[1;33mResultados da Query 3:\033[0m\n");
                while (fgets(line, sizeof(line), file)) {
                    printf("%s", line);
                }
                fclose(file);
                remove(temp_file); 
            printf("\033[1;31mErro ao abrir o arquivo temporário para leitura.\033[0m\n");
            }
            break;
        }   
        case 4:
            printf("DEBUG: Entrou no caso 4 (Query 4)\n");
            print_border("A PROCESSAR QUERY 4");
            printf("Mock: A processar Query 4...\n");
            break;
        case 5:
            printf("DEBUG: Entrou no caso 5 (Query 5)\n");
            print_border("A PROCESSAR QUERY 5");
            printf("Mock: A processar Query 5...\n");
            break;
        case 6: { 
            printf("DEBUG: Entrou no caso 6 (Query 6)\n");
            print_border("A PROCESSAR QUERY 6");

            char *user_id = get_valid_id6();

            if (!user_id || strlen(user_id) < 2 || user_id[0] != 'U') {
                printf("\033[1;31mErro: ID inválido! Deve começar com 'U'.\033[0m\n");
                free(user_id);
                break;
            }

            int year = get_valid_int("Introduza o ano (ex: 2023): ");

            int top_n = -1;
            printf("Introduza o número máximo de artistas no top (pressione Enter para ignorar): ");
            char input[16];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = '\0'; 
                if (strlen(input) > 0 && sscanf(input, "%d", &top_n) != 1) {
                    printf("\033[1;31mErro: Número inválido para artistas no top.\033[0m\n");
                    free(user_id);
                    break;
                }
            }

            char output_file[] = "temp_query6_output.txt";
            execute_querie6(storage, user_id, year, top_n, false, output_file);

            FILE *file = fopen(output_file, "r");
            if (file) {
                char line[256];
                printf("\n\033[1;33mResultados da Query 6:\033[0m\n");
                while (fgets(line, sizeof(line), file)) {
                    printf("%s", line);
                }
                fclose(file);
                remove(output_file); 
            } else {
                printf("\033[1;31mErro ao abrir o arquivo temporário para leitura.\033[0m\n");
            }

            free(user_id);
            break;
        }
        case 7:
            printf("DEBUG: Entrou no caso 7 (Ajuda)\n");
            print_border("AJUDA");
            printf("1. Query 1: Mostra o resumo (email/name;first_name/type;last_name/country;age/num_albums_individual;country/total recipe) de um utilizador ou de um artista, consoante o ID fornecido.\n");
            printf("2. Query 2: Lista os top N (número fornecido pelo utilizador) artistas por discografia, podendo ser filtrados por país.\n");
            printf("3. Query 3: Determina os géneros musicais mais populares por faixa etária.\n");
            printf("4. Query 4: Identifica o artista que esteve no top 10 mais vezes, consoante o intervalo de tempo desejado pelo utilizador.\n");
            printf("5. Query 5: Recomenda utilizadores com gostos semelhantes.\n");
            printf("6. Query 6: Mostra um resumo anual de um utilizador.\n");
            printf("0. Sair: Encerra o programa.\n");
            break;
        case 0:
            printf("DEBUG: Entrou no caso 0 (Sair)\n");
            print_border("SAIR");
            printf("A encerrar o programa...\n");
            return; 
        default:
            printf("DEBUG: Entrou no caso padrão (Erro)\n");
            print_border("ERRO");
            printf("\033[1;31mOpção inválida. Tente novamente.\033[0m\n");
            break;
    }

    printf("\n\033[1;36mPressione ENTER para voltar ao menu...\033[0m\n");
    char buffer[4];
    fgets(buffer, sizeof(buffer), stdin);
}

void countdown(int seconds) {
    for (int i = seconds; i > 0; i--) {
        printf("\rFaltam %d segundo(s)... ", i);
        fflush(stdout);  
        sleep(1);
    }
    printf("\n");
}

int get_valid_number() {
    char input[16]; 
    int number;

    while (true) {
        printf("Escolha uma opção: ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';
            if (sscanf(input, "%d", &number) == 1) {
                printf("DEBUG: Número válido inserido: %d\n", number); 
                return number;
            } else {
                printf("DEBUG: Entrada inválida recebida: %s\n", input); 
                printf("\033[1;31mErro: Entrada inválida.\033[0m\n");
            }
        } else {
            printf("DEBUG: Erro ao ler a entrada.\n");
        }
    }
}


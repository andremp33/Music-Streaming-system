#include "../include/parser.h"
#include "../include/management.h"
#include "../include/structs.h"
#include "../include/utils.h"
#include "../include/interactive.h"
#include "../include/progRunner.h"
#include "../include/Queries/querie2.h"
#include "../include/Queries/querie1.h"
#include "../include/Queries/querie3.h"
#include "../include/Queries/querie4.h"
#include "../include/Queries/querie5.h"
#include "../include/Queries/querie6.h"
#include "../include/testRunner.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#define OUTPUT_DIR "resultados/"
#define MAX_COMMAND_SIZE 256

void process_command(const char *command, STORAGE *storage, int command_number) {
    if (!command || strlen(command) == 0) {
        fprintf(stderr, "Erro: Comando vazio ou nulo.\n");
        return;
    }

    int query_type = -1;
    char id[256] = {0};
    bool use_separator_equals = strstr(command, "S") != NULL;

    if (sscanf(command, "%d", &query_type) != 1 || query_type <= 0) {
        fprintf(stderr, "Erro: Tipo de Query inválido no comando: %s\n", command);
        return;
    }

    char output_file[256];
    snprintf(output_file, sizeof(output_file), "%s/command%d_output.txt", OUTPUT_DIR, command_number);

    switch (query_type) {
        case 1: {
            if (use_separator_equals) {
                if (sscanf(command, "1S = %255s", id) == 1 || sscanf(command, "1S %255s", id) == 1) {
                    FILE *output = fopen(output_file, "w");
                    if (!output) {
                        perror("Erro ao abrir arquivo de saída para Query 1");
                        return;
                    }
                    query_q1(storage, id, use_separator_equals, output);
                    fclose(output);
                } else {
                    fprintf(stderr, "Erro: Formato inválido para Query 1 com S: %s\n", command);
                }
            } else {
                if (sscanf(command, "1 %255s", id) == 1) {
                    FILE *output = fopen(output_file, "w");
                    if (!output) {
                        perror("Erro ao abrir arquivo de saída para Query 1");
                        return;
                    }
                    query_q1(storage, id, use_separator_equals, output);
                    fclose(output);
                } else {
                    fprintf(stderr, "Erro: Formato inválido para Query 1: %s\n", command);
                }
            }
            break;
        }

        case 2: {
            int N = 0;
            char country[100] = {0};
            const char *country_ptr = NULL;

            if (sscanf(command, "2S %d \"%99[^\"]\"", &N, country) == 2 || 
                sscanf(command, "2S %d %99s", &N, country) == 2 || 
                sscanf(command, "2 %d \"%99[^\"]\"", &N, country) == 2 || 
                sscanf(command, "2 %d %99s", &N, country) == 2) {
                country_ptr = country;
            } else if (sscanf(command, "2S %d", &N) == 1 || sscanf(command, "2 %d", &N) == 1) {
                country_ptr = NULL;
            } else {
                fprintf(stderr, "Erro: Formato de comando inválido para Query 2: %s\n", command);
                return;
            }

            query_q2(storage, N, country_ptr, use_separator_equals, output_file);
            break;
        }
/*
        case 3: {
            int min_age = 0, max_age = 0;

            if (sscanf(command, "3S %d %d", &min_age, &max_age) == 2 || 
                sscanf(command, "3 %d %d", &min_age, &max_age) == 2) {
                //printf("Debug: Processando Query 3 com min_age=%d e max_age=%d.\n", min_age, max_age);
                query_q3(storage, min_age, max_age, use_separator_equals, output_file);
            } else {
                fprintf(stderr, "Erro: Formato de comando inválido para Query 3: %s\n", command);
            }
            break;
        }

        case 4: {
            char begin_date[11] = {0};
            char end_date[11] = {0};
            const char *begin_ptr = NULL, *end_ptr = NULL;

            if (sscanf(command, "4S \"%10[^\"]\" \"%10[^\"]\"", begin_date, end_date) == 2) {
                begin_ptr = begin_date;
                end_ptr = end_date;
            } else if (sscanf(command, "4S \"%10[^\"]\"", begin_date) == 1) {
                begin_ptr = begin_date;
            }

            execute_querie4(OUTPUT_DIR, storage, begin_ptr, end_ptr, use_separator_equals);
            break;
        }

        case 5: {
            char user_id[256] = {0};
            int num_recommendations = 0;

            if (sscanf(command, "5S \"%255[^\"]\" %d", user_id, &num_recommendations) == 2 || 
                sscanf(command, "5 \"%255[^\"]\" %d", user_id, &num_recommendations) == 2) {
                execute_querie5(OUTPUT_DIR, storage, user_id, num_recommendations, use_separator_equals);
            } else {
                fprintf(stderr, "Erro: Formato inválido para Query 5: %s\n", command);
            }
            break;
        }
*/
       case 6: { 
    char user_id[256] = {0};
    int year = 0, top_n = 0;
    bool use_separator_equals = strstr(command, "S") != NULL;

    if (use_separator_equals) {
        if (sscanf(command, "6S %255s %d %d", user_id, &year, &top_n) >= 2 || 
            sscanf(command, "6S %255s %d", user_id, &year) == 2) {
            char output_file[512];
            snprintf(output_file, sizeof(output_file), "%s/command%d_output.txt", OUTPUT_DIR, command_number);
            execute_querie6(storage, user_id, year, top_n, true, output_file);
        } else {
            fprintf(stderr, "Erro: Formato inválido para Query 6 com S: %s\n", command);
        }
    } else {
        if (sscanf(command, "6 %255s %d %d", user_id, &year, &top_n) >= 2 || 
            sscanf(command, "6 %255s %d", user_id, &year) == 2) {
            char output_file[512];
            snprintf(output_file, sizeof(output_file), "%s/command%d_output.txt", OUTPUT_DIR, command_number);
            execute_querie6(storage, user_id, year, top_n, false, output_file);
        } else {
            fprintf(stderr, "Erro: Formato inválido para Query 6: %s\n", command);
        }
    }
    break;
}


        default:
            //fprintf(stderr, "Erro: Tipo de Query não suportado: %d\n", query_type);
            break;
    }
}


void run_program_mode(const char *csv_path, const char *commands_file) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    FILE *commands = fopen(commands_file, "r");
    if (!commands) {
        perror("Erro ao abrir o arquivo de comandos");
        return;
    }

    STORAGE *storage = newStorage();
    if (!storage) {
        fprintf(stderr, "Erro ao criar a estrutura de armazenamento.\n");
        fclose(commands);
        return;
    }
    if (!parse_artists(csv_path, OUTPUT_DIR, getArtists(storage), getAlbums(storage), getMusics(storage), getHistory(storage)) ||
        !parse_albums(csv_path, OUTPUT_DIR, getAlbums(storage), getArtists(storage)) ||
        !parse_musics(csv_path, OUTPUT_DIR, getMusics(storage), getArtists(storage), getAlbums(storage)) ||
        !parse_users(csv_path, OUTPUT_DIR, getUsers(storage), getMusics(storage)) ||
        !parse_history(csv_path, OUTPUT_DIR, getHistory(storage), getUsers(storage), getMusics(storage))) {
        fprintf(stderr, "Erro ao carregar os dados.\n");
        free_storage(storage);
        fclose(commands);
        return;
    }

    char command[MAX_COMMAND_SIZE];
    int command_number = 1;

    while (fgets(command, sizeof(command), commands)) {
        command[strcspn(command, "\n")] = '\0';
        process_command(command, storage, command_number++);
    }

    fclose(commands);
    free_storage(storage);

    clock_gettime(CLOCK_REALTIME, &end);
    double total_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);

    printf("== Program Summary ==\n");
    printf("Total execution time: %.1f s\n", total_time);
    printf("Memory usage: %ld MB\n", r_usage.ru_maxrss / 1024);
}
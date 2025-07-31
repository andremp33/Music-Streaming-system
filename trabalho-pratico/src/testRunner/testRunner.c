#include "../../include/parser.h"
#include "../../include/management.h"
#include "../../include/structs.h"
#include "../../include/utils.h"
#include "../../include/interactive.h"
#include "../../include/Queries/querie2.h"
#include "../../include/Queries/querie1.h"
#include "../../include/Queries/querie3.h"
#include "../../include/Queries/querie4.h"
#include "../../include/Queries/querie5.h"
#include "../../include/Queries/querie6.h"
#include "../../include/testRunner.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#define OUTPUT_DIR "resultados/"
#define MAX_COMMAND_SIZE 256

void compare_output_files(const char *generated_file, const char *expected_file, int *correct_count, int *first_incorrect_line) {
    FILE *gen = fopen(generated_file, "r");
    FILE *exp = fopen(expected_file, "r");

    if (!gen || !exp) {
        fprintf(stderr, "Erro ao abrir arquivos para comparação: %s ou %s\n", generated_file, expected_file);
        if (gen) fclose(gen);
        if (exp) fclose(exp);
        *correct_count = 0;
        *first_incorrect_line = -1;
        return;
    }

    char gen_line[MAX_COMMAND_SIZE], exp_line[MAX_COMMAND_SIZE];
    int line_number = 1;
    *correct_count = 0;
    *first_incorrect_line = -1;

    while (fgets(gen_line, sizeof(gen_line), gen) && fgets(exp_line, sizeof(exp_line), exp)) {
        if (strcmp(gen_line, exp_line) == 0) {
            (*correct_count)++;
        } else if (*first_incorrect_line == -1) {
            *first_incorrect_line = line_number;
        }
        line_number++;
    }

    fclose(gen);
    fclose(exp);
}

void run_test_mode(const char *csv_path, const char *commands_file, const char *expected_output_dir) {
    FILE *commands = fopen(commands_file, "r");
    if (!commands) {
        fprintf(stderr, "Erro ao abrir arquivo de comandos.\n");
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
        fclose(commands);
        free_storage(storage);
        return;
    }

    char command[MAX_COMMAND_SIZE];
    int command_number = 1;
    struct rusage r_usage;
    double total_execution_time = 0.0;

    while (fgets(command, sizeof(command), commands)) {
        command[strcspn(command, "\n")] = '\0';

        int query_type;
        sscanf(command, "%d", &query_type);

        struct timespec query_start, query_end;
        clock_gettime(CLOCK_REALTIME, &query_start);

        process_command(command, storage, command_number);

        char generated_file[MAX_COMMAND_SIZE], expected_file[MAX_COMMAND_SIZE];
        snprintf(generated_file, sizeof(generated_file), "%s/command%d_output.txt", OUTPUT_DIR, command_number);
        snprintf(expected_file, sizeof(expected_file), "%s/command%d_output.txt", expected_output_dir, command_number);

        int correct_count = 0, first_incorrect_line = -1;
        compare_output_files(generated_file, expected_file, &correct_count, &first_incorrect_line);

        clock_gettime(CLOCK_REALTIME, &query_end);
        double query_time = (query_end.tv_sec - query_start.tv_sec) + (query_end.tv_nsec - query_start.tv_nsec) / 1e9;
        total_execution_time += query_time;

        printf("Q%d: %d linhas corretas\n", query_type, correct_count);
        if (first_incorrect_line != -1) {
            printf("Descrepância na query %d: linha %d do arquivo %s\n", query_type, first_incorrect_line, generated_file);
        }
        printf("Tempo de execução da Q%d: %.1f ms\n", query_type, query_time * 1000);

        command_number++;
    }

    fclose(commands);
    free_storage(storage);

    getrusage(RUSAGE_SELF, &r_usage);
    printf("\nMemória utilizada: %ld MB\n", r_usage.ru_maxrss / 1024);
    printf("Tempo total: %.1f s\n", total_execution_time);
}
#include "../../include/interactive.h"
#include "../../include/interactive_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_DATASET_PATH "/home/andre/dataset/sem_erros"

void run_interactive_mode() {
    char dataset_path[512];
    clear_screen();
    print_border("BEM-VINDO AO PROGRAMA INTERATIVO");
    printf("Introduza o caminho do dataset (pressione Enter para usar o default): ");
    fgets(dataset_path, sizeof(dataset_path), stdin);
    dataset_path[strcspn(dataset_path, "\n")] = '\0'; // Remover newline

    if (strlen(dataset_path) == 0) {
        strcpy(dataset_path, DEFAULT_DATASET_PATH);
    }

    printf("DEBUG: Caminho do dataset recebido: %s\n", dataset_path); // Debug
    fflush(stdout); // Força a exibição imediata desta linha

    // ---------------------------------------------------------------------
    // 1. Exibe uma mensagem de "Carregando" e chama a contagem decrescente
    clear_screen();
    print_border("A CARREGAR O DATASET");
    printf("Aguarde por favor...\n");
    fflush(stdout);        // Garante que o texto sai no terminal
    countdown(3);          // Mostra a contagem (3, 2, 1)
    // ---------------------------------------------------------------------

    // 2. Agora sim, carrega o dataset
    STORAGE *storage = newStorage();
    if (!populate_storage(storage, dataset_path, "resultados/")) {
        printf("\033[1;31mErro ao carregar os dados.\033[0m\n");
        free_storage(storage);
        return;
    }

    printf("DEBUG: Dados carregados com sucesso.\n"); // Debug

    // 3. Entra no loop do menu
    int query;
    do {
        printf("DEBUG: A entrar no ciclo do menu.\n"); // Debug
        show_menu();
        query = get_valid_number(); // Certifica-se de que a entrada é válida
        printf("DEBUG: Valor de query recebido: %d\n", query); // Debug
        process_query(query, storage); // Chama a função para processar a query
    } while (query != 0);

    printf("DEBUG: A sair do loop principal.\n"); // Debug
    free_storage(storage);
}








#ifndef INTERACTIVE_UTILS_H
#define INTERACTIVE_UTILS_H

#include <stdbool.h>
#include "../include/management.h"

/**
 * @brief Limpa o terminal.
 */
void clear_screen();

/**
 * @brief Imprime uma borda estilizada com um título.
 * @param title O título a ser exibido dentro da borda.
 */
void print_border(const char *title);

/**
 * @brief Exibe o menu interativo no terminal.
 */
void show_menu();

/**
 * @brief Processa a query escolhida pelo utilizador.
 * @param query Número da query.
 * @param storage Estrutura de dados carregada.
 */
void process_query(int query, STORAGE *storage);

/**
 * @brief Carrega os dados do dataset nas estruturas apropriadas.
 * @param storage Estrutura de armazenamento.
 * @param dataset_path Caminho para o dataset.
 * @return true se os dados foram carregados com sucesso, false caso contrário.
 */
bool load_data(STORAGE *storage, const char *dataset_path);

/**
 * @brief Mostra uma contagem regressiva.
 * @param seconds Número de segundos para a contagem regressiva.
 */
void countdown(int seconds);

/**
 * @brief Valida se a entrada é um número válido.
 * @return Número válido inserido pelo utilizador.
 */
int get_valid_number();

#endif // INTERACTIVE_UTILS_H


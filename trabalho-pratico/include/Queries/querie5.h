#ifndef QUERIE5_H
#define QUERIE5_H

#include "../management.h"
#include "../utils.h"
#include "../structs.h"
#include <glib.h>

/**
 * @brief Preenche a matriz de classificação com base no histórico de audições.
 * @param users Tabela de utilizadores.
 * @param history Tabela de histórico.
 * @param genres Tabela de géneros.
 * @param classification_matrix Matriz de classificação a ser preenchida.
 * @param user_ids Array de IDs dos utilizadores.
 * @param genre_names Array de nomes dos géneros.
 */

void populate_classification_matrix(GHashTable *users, GHashTable *history, GHashTable *musics,
                                    int **classification_matrix, const char **user_ids, const char **genre_names);
/**
 * @brief Executa a Query 5 - Recomendação de utilizadores com gostos parecidos.
 * @param outputDir Diretório onde o ficheiro de output será salvo.
 * @param usersTable Tabela de utilizadores.
 * @param historyTable Tabela de histórico.
 * @param musicsTable Tabela de músicas.
 * @param user_id ID do utilizador alvo.
 * @param num_recommendations Número de utilizadores a recomendar.
 */
void execute_querie5(const char *outputDir, STORAGE *storage, const char *user_id, int num_recommendations, bool use_separator_equals);
#endif

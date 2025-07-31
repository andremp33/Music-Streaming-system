#ifndef QUERIE6_H
#define QUERIE6_H

#include "../../include/management.h"

typedef struct UserSummary UserSummary;
/**
 * @brief Executa a Query 6.
 * 
 * @param outputDir Diretório onde o resultado será salvo.
 * @param storage Estrutura de armazenamento contendo as tabelas de dados.
 * @param user_id ID do utilizador para o qual se deseja calcular o resumo.
 * @param year Ano para o qual as estatísticas devem ser calculadas.
 * @param top_n Número opcional de artistas mais ouvidos.
 */
void execute_querie6(STORAGE *storage, const char *user_id, int year, int top_n, bool use_separator_equals, const char *output_file);

#endif

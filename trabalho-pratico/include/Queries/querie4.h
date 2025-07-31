#ifndef QUERIE4_H
#define QUERIE4_H

#include "../structs.h"
#include "../management.h"
#include <glib.h>

typedef struct ArtistTime ArtistTime;

/**
 * @brief Executa a Query 4, identificando o artista com mais aparições no top 10.
 * @param outputDir Diretório onde o resultado será salvo.
 * @param historyTable Tabela com os dados de histórico.
 * @param artistTable Tabela com os dados de artistas.
 * @param begin_date Data inicial do intervalo (opcional).
 * @param end_date Data final do intervalo (opcional).
 */

void execute_querie4(const char *outputDir, STORAGE *storage, const char *begin_date, const char *end_date, bool use_separator_equals);

gint compare_artist_time(gconstpointer a, gconstpointer b);

#endif

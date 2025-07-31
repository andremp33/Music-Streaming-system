#ifndef STATISTICS_H
#define STATISTICS_H

#include <glib.h>

/**
 * @brief Calcula o número de álbuns associados a um artista específico.
 * @param artist_id ID único do artista.
 * @param albums_table Ponteiro para a tabela de álbuns.
 * @return Número de álbuns associados ao artista.
 */
int calculate_num_albums_for_artist(const char *artist_id, AlbumTable *albums_table);

/**
 * @brief Calcula a receita total gerada por um artista, considerando as músicas associadas.
 * @param artist_id ID único do artista.
 * @param artists_table Ponteiro para a tabela de artistas.
 * @param musics_table Ponteiro para a tabela de músicas.
 * @param history_table Ponteiro para a tabela de histórico de reproduções.
 * @return Receita total gerada pelo artista.
 */
double calculate_total_revenue_for_artist(const char *artist_id, ArtistTable *artists_table, MusicTable *musics_table, HistoryTable *history_table);

/**
 * @brief Calcula a receita de participação de um artista em grupos coletivos.
 * @param artist_id ID único do artista.
 * @param artists_table Ponteiro para a tabela de artistas.
 * @param musics_table Ponteiro para a tabela de músicas.
 * @param history_table Ponteiro para a tabela de histórico de reproduções.
 * @return Receita de participação do artista em grupos coletivos.
 */
double calculate_participation_revenue_for_artist(const char *artist_id, ArtistTable *artists_table, MusicTable *musics_table, HistoryTable *history_table);

/**
 * @brief Processa o top 10 semanal de artistas baseado no tempo de reprodução.
 * @param weekly_top10 Tabela hash contendo os dados semanais do top 10.
 * @param artist_appearances Tabela hash para contabilizar aparições de artistas.
 * @param artist_type Tipo de artista (individual ou coletivo).
 */
void process_weekly_top10(GHashTable *weekly_top10, GHashTable *artist_appearances, const char *artist_type);

#endif

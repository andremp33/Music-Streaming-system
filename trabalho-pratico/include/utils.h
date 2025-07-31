#ifndef UTILS_H
#define UTILS_H

#define MAX_BUFFER_SIZE 1024

#include <stdio.h>
#include <glib.h>
#include <stdbool.h>
#include "structs.h"
#include "management.h"

typedef struct ArtistStat ArtistStat;
typedef struct ArtistTime ArtistTime;

// Funções auxiliares

/**
 * @brief Converte uma estrutura Date para uma string no formato "YYYY/MM/DD".
 * @param date Ponteiro para a estrutura Date a ser convertida.
 * @return String representando a data ou NULL em caso de erro.
 */
char* date_to_string(struct Date *date);

/**
 * @brief Inicializa um ficheiro para registrar erros.
 * @param outputDir Diretório onde o ficheiro será criado.
 * @param error_dataset Nome do ficheiro de erros.
 * @return Ponteiro para o ficheiro inicializado.
 */
FILE *initialize_error_registery(const char *outputDir, const char *error_dataset);

/**
 * @brief Registra uma linha de erro no ficheiro especificado.
 * @param file Ficheiro onde o erro será registrado.
 * @param line Linha contendo o erro a ser registrado.
 */
void register_error_line(FILE *file, const char *line);

/**
 * @brief Fecha o ficheiro de registro de erros.
 * @param file Ficheiro a ser fechado.
 */
void close_error_registery(FILE *file);

/**
 * @brief Salva o resultado de uma query em um ficheiro específico.
 * @param line Número da query.
 * @param res Resultado da query.
 * @param outputDirectory Diretório onde o resultado será salvo.
 */
void save_result(int line, const char *res, const char *outputDirectory);

/**
 * @brief Inicializa um ficheiro para salvar resultados.
 * @param line Número da query.
 * @param outputDirectory Diretório onde o ficheiro será criado.
 * @return Ponteiro para o ficheiro inicializado.
 */
FILE *initialize_file_saving(int line, const char *outputDirectory);

/**
 * @brief Salva uma linha no ficheiro especificado.
 * @param file Ficheiro onde a linha será salva.
 * @param line Linha a ser salva no ficheiro.
 */
void save_line(FILE *file, const char *line);

/**
 * @brief Fecha um ficheiro de resultados.
 * @param file Ficheiro a ser fechado.
 */
void close_file_saving(FILE *file);

/**
 * @brief Valida se uma string representa uma lista no formato esperado.
 * @param list String contendo a lista.
 * @param has_error Ponteiro para um inteiro onde será registrado se há erro.
 */
void validate_csv_list(const char *list, int *has_error);

// Funções relacionadas a Artistas

/**
 * @brief Obtém o nome de um artista a partir de seu ID.
 * @param artist_id ID do artista.
 * @param artistTable Tabela de artistas.
 * @return Nome do artista ou NULL se não encontrado.
 */
const char* get_artist_name_from_id(const char *artist_id, ArtistTable *artistTable);

/**
 * @brief Obtém o tipo de um artista a partir de seu ID.
 * @param artist_id ID do artista.
 * @param artistTable Tabela de artistas.
 * @return Tipo do artista ou NULL se não encontrado.
 */
const char* get_artist_type_from_id(const char *artist_id, ArtistTable *artistTable);

/**
 * @brief Obtém o país de um artista a partir de seu ID.
 * @param artist_id ID do artista.
 * @param artistTable Tabela de artistas.
 * @return País do artista ou NULL se não encontrado.
 */
const char* get_artist_country_from_id(const char *artist_id, ArtistTable *artistTable);

/**
 * @brief Liberta a memória associada a uma estrutura ArtistTime.
 * @param artist_time Ponteiro para a estrutura ArtistTime a ser liberada.
 */
void free_artist_time(ArtistTime *artist_time);

// Funções relacionadas à classificação

/**
 * @brief Preenche uma matriz de classificação com dados de utilizadores e géneros.
 * @param users Tabela de utilizadores.
 * @param history Tabela de histórico.
 * @param musics Tabela de músicas.
 * @param classification_matrix Matriz de classificação a ser preenchida.
 * @param user_ids IDs dos utilizadores.
 * @param genre_names Nomes dos géneros.
 */
void populate_classification_matrix(GHashTable *users, GHashTable *history, GHashTable *musics,
                                    int **classification_matrix, const char **user_ids, const char **genre_names);

/**
 * @brief Valida se uma string representa uma data válida no formato "YYYY/MM/DD".
 * @param date String contendo a data.
 * @return true se a data for válida, false caso contrário.
 */
bool is_valid_date(const char *date);

/**
 * @brief Calcula a semana correspondente a um timestamp.
 * @param timestamp Timestamp no formato "YYYY/MM/DD HH:MM:SS".
 * @return String representando a semana no formato "YYYY-WW".
 */
char* get_week_from_timestamp(const char *timestamp);

// Funções relacionadas à música

/**
 * @brief Obtém o ID do artista associado a uma música.
 * @param music_id ID da música.
 * @param musicTable Tabela de músicas.
 * @param artistTable Tabela de artistas.
 * @return ID do artista associado ou NULL se não encontrado.
 */
const char* get_artist_from_music(const char *music_id, MusicTable *musicTable, ArtistTable *artistTable);

/**
 * @brief Calcula o tempo total de reprodução a partir de uma tabela de histórico.
 * @param user_history Tabela de histórico.
 * @return String representando o tempo total no formato "hh:mm:ss".
 */
char* calculate_total_listening_time(GHashTable *user_history);

/**
 * @brief Determina o artista mais ouvido pelo tempo acumulado de reprodução.
 * @param user_history Tabela de histórico filtrada por utilizador e ano.
 * @param musics_table Tabela de músicas.
 * @param artists_table Tabela de artistas.
 * @return ID do artista mais ouvido.
 */
char* get_top_artist(GHashTable *user_history, MusicTable *musics_table, ArtistTable *artists_table);

/**
 * @brief Identifica o dia com mais músicas ouvidas.
 * @param user_history Tabela de histórico filtrada por utilizador e ano.
 * @return Data no formato "YYYY/MM/DD" do dia com mais reproduções.
 */
char* get_day_with_most_listens(GHashTable *user_history);

/**
 * @brief Determina o gênero mais ouvido pelo tempo acumulado de reprodução.
 * @param user_history Tabela de histórico filtrada por utilizador e ano.
 * @param musics_table Tabela de músicas.
 * @return Nome do gênero mais ouvido.
 */
char* get_top_genre(GHashTable *user_history, MusicTable *musics_table);

/**
 * @brief Determina o álbum favorito com base no tempo acumulado de reprodução.
 * @param user_history Tabela de histórico filtrada por utilizador e ano.
 * @param musics_table Tabela de músicas.
 * @param albums_table Tabela de álbuns.
 * @return ID do álbum favorito.
 */
char* get_favorite_album(GHashTable *user_history, MusicTable *musics_table, AlbumTable *albums_table);

/**
 * @brief Identifica a hora do dia em que o utilizador costuma ouvir mais música.
 * @param user_history Tabela de histórico filtrada por utilizador e ano.
 * @return Hora do dia representada como string de dois dígitos ("00" a "23").
 */
char* get_peak_listening_hour(GHashTable *user_history);

// Estatísticas de Artistas

/**
 * @brief Calcula estatísticas de artistas com base no histórico do utilizador.
 * @param user_history Tabela de histórico.
 * @param musics_table Tabela de músicas.
 * @param artists_table Tabela de artistas.
 * @return GHashTable* com estatísticas por artista.
 */
GHashTable* calculate_artist_stats(GHashTable *user_history, MusicTable *musics_table, ArtistTable *artists_table);

/**
 * @brief Ordena estatísticas de artistas por tempo de reprodução.
 * @param artist_stats Tabela de estatísticas dos artistas.
 * @return Lista ordenada de ArtistStat.
 */
GList* sort_artist_stats(GHashTable *artist_stats);

/**
 * @brief Liberta a memória de uma estrutura ArtistStat.
 * @param data Estrutura ArtistStat a ser liberada.
 */
void free_artist_stat(gpointer data);

/**
 * @brief Verifica se um artista pertence a algum grupo.
 * @param artist Ponteiro para a estrutura Artist.
 * @return true se o artista pertence a um grupo, false caso contrário.
 */
bool artist_belongs_to_any_group(const Artist *artist);

/**
 * @brief Conta músicas distintas no histórico do utilizador.
 * @param user_history Tabela de histórico.
 * @return Número de músicas distintas.
 */
unsigned int count_distinct_musics(GHashTable *user_history);

#endif

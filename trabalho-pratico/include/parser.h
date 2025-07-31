#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdbool.h>
#include "management.h"

/**
 * @brief Faz o parsing e validação dos dados de músicas a partir de um ficheiro CSV.
 * @param datasetDir Diretório onde o ficheiro de músicas está localizado.
 * @param outputDir Diretório onde os ficheiros de erros serão armazenados.
 * @param musics_table Ponteiro para a tabela de músicas a ser populada.
 * @param artists_table Ponteiro para a tabela de artistas utilizada para validação.
 * @param albums_table Ponteiro para a tabela de álbuns utilizada para validação.
 * @return true se o parsing for bem-sucedido, false caso contrário.
 */
bool parse_musics(const char *datasetDir, const char *outputDir, MusicTable *musics_table, ArtistTable *artists_table, AlbumTable *albums_table);

/**
 * @brief Faz o parsing e validação dos dados de artistas a partir de um ficheiro CSV.
 * @param datasetDir Diretório onde o ficheiro de artistas está localizado.
 * @param outputDir Diretório onde os ficheiros de erros serão armazenados.
 * @param artists_table Ponteiro para a tabela de artistas a ser populada.
 * @param albums_table Ponteiro para a tabela de álbuns utilizada para validação.
 * @param musics_table Ponteiro para a tabela de músicas utilizada para validação.
 * @param history_table Ponteiro para a tabela de histórico utilizada para validação.
 * @return true se o parsing for bem-sucedido, false caso contrário.
 */
bool parse_artists(const char *datasetDir, const char *outputDir, ArtistTable *artists_table, AlbumTable *albums_table, MusicTable *musics_table, HistoryTable *history_table);

/**
 * @brief Faz o parsing e validação dos dados de utilizadores a partir de um ficheiro CSV.
 * @param datasetDir Diretório onde o ficheiro de utilizadores está localizado.
 * @param outputDir Diretório onde os ficheiros de erros serão armazenados.
 * @param user_table Ponteiro para a tabela de utilizadores a ser populada.
 * @param musics_table Ponteiro para a tabela de músicas utilizada para validação.
 * @return true se o parsing for bem-sucedido, false caso contrário.
 */
bool parse_users(const char *datasetDir, const char *outputDir, UserTable *user_table, MusicTable *musics_table);

/**
 * @brief Faz o parsing e validação dos dados de álbuns a partir de um ficheiro CSV.
 * @param datasetDir Diretório onde o ficheiro de álbuns está localizado.
 * @param outputDir Diretório onde os ficheiros de erros serão armazenados.
 * @param albums_table Ponteiro para a tabela de álbuns a ser populada.
 * @param artists_table Ponteiro para a tabela de artistas utilizada para validação.
 * @return true se o parsing for bem-sucedido, false caso contrário.
 */
bool parse_albums(const char *datasetDir, const char *outputDir, AlbumTable *albums_table, ArtistTable *artists_table);

/**
 * @brief Faz o parsing e validação dos dados de histórico a partir de um ficheiro CSV.
 * @param datasetDir Diretório onde o ficheiro de histórico está localizado.
 * @param outputDir Diretório onde os ficheiros de erros serão armazenados.
 * @param history_table Ponteiro para a tabela de histórico a ser populada.
 * @param users_table Ponteiro para a tabela de utilizadores utilizada para validação.
 * @param musics_table Ponteiro para a tabela de músicas utilizada para validação.
 * @return true se o parsing for bem-sucedido, false caso contrário.
 */
bool parse_history(const char *datasetDir, const char *outputDir, HistoryTable *history_table, UserTable *users_table, MusicTable *musics_table);

#endif

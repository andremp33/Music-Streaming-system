#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <glib.h>
#include <stdbool.h>

typedef struct Artist Artist;
typedef struct ArtistTable ArtistTable;
typedef struct History History;
typedef struct HistoryTable HistoryTable;
typedef struct Music Music;
typedef struct MusicTable MusicTable;
typedef struct AlbumTable AlbumTable;
typedef struct User User;
typedef struct UserTable UserTable;
typedef struct Date Date;
typedef struct Album Album;
typedef struct AlbumTable AlbumTable;

// Funções relacionadas à estrutura Artist

/**
 * @brief Cria uma instância de Artist a partir de uma linha do CSV.
 * @param line Linha do CSV contendo os dados do artista.
 * @param error_registry Ficheiro onde serão registrados os erros encontrados.
 * @param artistTable Tabela de artistas para validações.
 * @return Ponteiro para a instância criada ou NULL em caso de erro.
 */
Artist* create_artist_from_line(const char *line, FILE *error_registry, ArtistTable *artistTable);

// Setters para Artist
void set_artist_id(Artist *artist, const char *id);
void set_artist_name(Artist *artist, const char *name);
void set_artist_description(Artist *artist, const char *description);
void set_artist_recipe_per_stream(Artist *artist, const char *recipe_str);
void set_artist_country(Artist *artist, const char *country);
void set_artist_type(Artist *artist, const char *type);
void set_artist_id_constituent(Artist *artist, const char *id_constituent_str, const char *type, FILE *error_registry, int *has_error);

// Getters para Artist
const char* get_artist_id(Artist *artist);
const char* get_artist_name(Artist *artist);
const char* get_artist_description(Artist *artist);
const char* get_artist_recipe_per_stream(Artist *artist);
GArray* get_artist_id_constituent(Artist *artist);
const char* get_artist_country(Artist *artist);
const char* get_artist_type(Artist *artist);

/**
 * @brief Libera a memória associada a uma instância de Artist.
 * @param artist_ptr Ponteiro para a instância de Artist a ser liberada.
 */
void free_artist(void *artist_ptr);

// Funções relacionadas à estrutura Music

/**
 * @brief Cria uma instância de Music a partir de uma linha do CSV.
 * @param line Linha do CSV contendo os dados da música.
 * @param error_registry Ficheiro onde serão registrados os erros encontrados.
 * @param musicTable Tabela de músicas para validações.
 * @param artistTable Tabela de artistas para validações.
 * @param albumTable Tabela de álbuns para validações.
 * @return Ponteiro para a instância criada ou NULL em caso de erro.
 */
Music* create_music_from_line(const char *line, FILE *error_registry, MusicTable *musicTable, ArtistTable *artistTable, AlbumTable *albumTable);

// Setters para Music
void set_music_id(Music *music, const char *id);
void set_music_title(Music *music, const char *title);
void set_music_artist_id(Music *music, const char *artist_id_str, ArtistTable *artistTable, FILE *error_registry, int *has_error);
void set_music_duration(Music *music, const char *duration, int *has_error);
void set_music_genre(Music *music, const char *genre);
void set_music_year(Music *music, const char *year_str);
void set_music_lyrics(Music *music, const char *lyrics);
void set_music_album_id(Music *music, const char *album_id_str, AlbumTable *albumTable, FILE *error_registry, int *has_error);

// Getters para Music
const char* get_music_id(Music *music);
const char* get_music_title(Music *music);
GArray* get_music_artist_id(Music *music);
const char* get_music_duration(Music *music);
const char* get_music_genre(Music *music);
const char* get_music_album_id(Music *music);
int get_music_year(Music *music);
const char* get_music_lyrics(Music *music);

/**
 * @brief Libera a memória associada a uma instância de Music.
 * @param music_ptr Ponteiro para a instância de Music a ser liberada.
 */
void free_music(void *music_ptr);

// Funções relacionadas à estrutura User

/**
 * @brief Cria uma instância de User a partir de uma linha do CSV.
 * @param line Linha do CSV contendo os dados do utilizador.
 * @param error_registry Ficheiro onde serão registrados os erros encontrados.
 * @param userTable Tabela de utilizadores para validações.
 * @param musicTable Tabela de músicas para validações.
 * @return Ponteiro para a instância criada ou NULL em caso de erro.
 */
User* create_user_from_line(const char *line, FILE *error_registry, UserTable *userTable, MusicTable *musicTable);

// Setters para User
void set_user_username(User *user, const char *username);
void set_user_email(User *user, const char *email, int *has_error);
void set_user_first_name(User *user, const char *first_name);
void set_user_last_name(User *user, const char *last_name);
void set_user_birth_date(User *user, const char *birth_date, int *has_error);
void set_user_country(User *user, const char *country);
void set_user_subscription_type(User *user, const char *subscription_type, int *has_error);
void set_user_liked_songs_id(User *user, const char *liked_songs_id, MusicTable *musicTable, FILE *error_registry, int *has_error);

// Getters para User
const char* get_user_username(User *user);
const char* get_user_email(User *user);
const char* get_user_first_name(User *user);
const char* get_user_last_name(User *user);
Date* get_user_birth_date(User *user);
const char* get_user_country(User *user);
const char* get_user_subscription_type(User *user);
GArray* get_user_liked_songs_id(User *user);

/**
 * @brief Libera a memória associada a uma instância de User.
 * @param user_ptr Ponteiro para a instância de User a ser liberada.
 */
void free_user(void *user_ptr);

// Funções relacionadas à estrutura History

/**
 * @brief Cria uma instância de History a partir de uma linha do CSV.
 * @param line Linha do CSV contendo os dados do histórico.
 * @param error_registry Ficheiro onde serão registrados os erros encontrados.
 * @param historyTable Tabela de histórico para validações.
 * @param userTable Tabela de utilizadores para validações.
 * @param musicTable Tabela de músicas para validações.
 * @return Ponteiro para a instância criada ou NULL em caso de erro.
 */
History* create_history_from_line(const char *line, FILE *error_registry, HistoryTable *historyTable, UserTable *userTable, MusicTable *musicTable);

// Setters para History
void set_history_id(History *history, const char *id);
void set_history_user_id(History *history, const char *user_id, UserTable *userTable, int *has_error);
void set_history_music_id(History *history, const char *music_id, MusicTable *musicTable, int *has_error);
void set_history_timestamp(History *history, const char *timestamp);
void set_history_duration(History *history, const char *duration, int *has_error);
void set_history_platform(History *history, const char *platform, int *has_error);

// Getters para History
const char* get_history_id(History *history);
const char* get_history_user_id(History *history);
const char* get_history_music_id(History *history);
const char* get_history_timestamp(History *history);
const char* get_history_duration(History *history);
const char* get_history_platform(History *history);

/**
 * @brief Libera a memória associada a uma instância de History.
 * @param history_ptr Ponteiro para a instância de History a ser liberada.
 */
void free_history(void *history_ptr);

// Funções relacionadas à estrutura Album

/**
 * @brief Cria uma instância de Album a partir de uma linha do CSV.
 * @param line Linha do CSV contendo os dados do álbum.
 * @param error_registry Ficheiro onde serão registrados os erros encontrados.
 * @param albumTable Tabela de álbuns para validações.
 * @param artistTable Tabela de artistas para validações.
 * @return Ponteiro para a instância criada ou NULL em caso de erro.
 */
Album* create_album_from_line(const char *line, FILE *error_registry, AlbumTable *albumTable, ArtistTable *artistTable);

// Setters para Album
void set_album_id(Album *album, const char *id);
void set_album_title(Album *album, const char *title);
void set_album_year(Album *album, const char *year_str);
void set_album_artist_id(Album *album, const char *artist_id_str, ArtistTable *artistTable, FILE *error_registry, int *has_error);
void set_album_producers(Album *album, const char *producers_str);

// Getters para Album
const char* get_album_id(Album *album);
const char* get_album_title(Album *album);
int get_album_year(Album *album);
GArray* get_album_artist_id(Album *album);
GArray* get_album_producers(Album *album);

/**
 * @brief Libera a memória associada a uma instância de Album.
 * @param album_ptr Ponteiro para a instância de Album a ser liberada.
 */
void free_album(void *album_ptr);

/**
 * @brief Filtra registros de histórico por utilizador e ano.
 * @param history_table Tabela de histórico.
 * @param user_id ID do utilizador a ser filtrado.
 * @param year Ano para filtrar os registros.
 * @return Tabela de histórico filtrada.
 */
GHashTable* filter_history_by_user_and_year(GHashTable *history_table, const char *user_id, int year);

#endif

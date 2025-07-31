#ifndef MANAGEMENT_H
#define MANAGEMENT_H

#include <glib.h>
#include "structs.h"


typedef struct User User;
typedef struct Date Date;
typedef struct History History;
typedef struct Album Album;
typedef struct Music Music;
typedef struct Artist Artist;
typedef struct HistoryTable HistoryTable;
typedef struct ArtistTable ArtistTable;
typedef struct MusicTable MusicTable;
typedef struct UserTable UserTable;
typedef struct storage STORAGE;
typedef struct AlbumTable AlbumTable;

ArtistTable* create_artists_table();
void free_artist_wrapper(void *artist_ptr);
void add_artist_to_table(ArtistTable* artistTable, Artist* artist);
void populate_artists_table(ArtistTable* artistTable, GHashTable* artists_table);
bool artist_table_contains(ArtistTable *artistTable, const char *artist_id);
void free_artist_table(ArtistTable* artistTable);
void print_artist_table(ArtistTable *artists_table);



MusicTable* create_music_table();
void free_music_wrapper(void *music_ptr);
void add_music_to_table(MusicTable* musicTable, Music* music);
bool music_table_contains(MusicTable *musicTable, const char *music_id);
void populate_music_table(MusicTable* musicTable, GHashTable* musics_table);
void free_music_table(MusicTable* musicTable);
void print_music_table(MusicTable *musics_table);
bool parse_groups_and_artists(ArtistTable *artists_table);

UserTable* create_user_table();
void free_user_wrapper(void *user_ptr);
void add_user_to_table(UserTable* userTable, User* user);
bool user_table_contains(UserTable *userTable, const char *user_id);
void populate_user_table(UserTable* userTable, GHashTable* users_table);
void print_user_table(UserTable *users_table);
void free_user_table(UserTable* userTable);



AlbumTable* create_album_table();
void free_album_wrapper(void *album_ptr);
void add_album_to_table(AlbumTable* albumTable, Album* album);
void populate_album_table(AlbumTable* albumTable, GHashTable* albums_table);
bool album_table_contains(AlbumTable *albumTable, const char *album_id);
void free_album_table(AlbumTable* albumTable);
void print_album_table(AlbumTable *albums_table);



HistoryTable* create_history_table();
void free_history_wrapper(void *history_ptr);
void add_history_to_table(HistoryTable* historyTable, History* history);
bool history_table_contains(HistoryTable *historyTable, const char *history_id);
void populate_history_table(HistoryTable    *historyTable,  GHashTable *history_table);
void free_history_table(HistoryTable* historyTable);
void print_history_table(HistoryTable *history_table);

void free_storage(STORAGE *s);
bool populate_storage(STORAGE* storage, const char* datasetDir, const char* outputDir);
STORAGE *newStorage();

UserTable *getUsers(STORAGE *s); 
ArtistTable *getArtists(STORAGE *s); 
MusicTable *getMusics(STORAGE *s);
AlbumTable *getAlbums(STORAGE *s);
HistoryTable *getHistory(STORAGE *s);


#endif
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/management.h"
#include "../../include/parser.h"
#include "../../include/utils.h"
#include "../../include/structs.h"

#define MAX_LINE_LENGTH 16384

struct MusicTable {
    GHashTable *mtable;
};

struct Music {
    char *id;           
    char *title;         
    GArray *artist_id_str;
    char *album_id_str;   
    char *duration;      
    char *genre;         
    int year_str;           
    char *lyrics;        
};

MusicTable* create_music_table() {
    MusicTable* musicTable = g_new(MusicTable, 1);
    musicTable->mtable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)free_music);
    return musicTable;
}


void add_music_to_table(MusicTable* musicTable, Music* music) {
    if (music_table_contains(musicTable, get_music_id(music))) {
        fprintf(stderr, "Error: Duplicate music ID: %s\n", get_music_id(music));
        return;
    }
    char *music_id_copy = g_strdup(get_music_id(music));
    g_hash_table_insert(musicTable->mtable, music_id_copy, music);
}

void free_music_table(MusicTable* musicTable) {
    if (musicTable) {
        g_hash_table_destroy(musicTable->mtable);
        g_free(musicTable);
    }
}

bool music_table_contains(MusicTable *musicTable, const char *music_id) {
    return g_hash_table_contains(musicTable->mtable, music_id);
}

void print_music_table(MusicTable *musicTable) {
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, musicTable->mtable);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Music *music = (Music *)value;
        printf("Music ID: %s\n", get_music_id(music));
        printf("Title: %s\n", get_music_title(music));
        printf("Duration: %s\n", get_music_duration(music));
        printf("Genre: %s\n", get_music_genre(music));
        printf("Year: %d\n", get_music_year(music));

        printf("Artist IDs: ");
        for (guint i = 0; i < music->artist_id_str->len; i++) {
            printf("%s ", g_array_index(music->artist_id_str, char*, i));
        }
        printf("\n");
   }
}
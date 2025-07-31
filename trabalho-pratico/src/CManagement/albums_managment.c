#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/management.h"
#include "../../include/parser.h"
#include "../../include/utils.h"
#include "../../include/structs.h"
#include <stdbool.h>
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/management.h"
#include "../../include/parser.h"
#include "../../include/utils.h"
#include "../../include/structs.h"

#define MAX_LINE_LENGTH 16384

struct AlbumTable {
    GHashTable *altable;
};

struct Album {
    char *id;
    char *title;
    GArray *artist_ids;
    int year;
    GArray *producers;
};

AlbumTable* create_album_table() {
    AlbumTable* albumTable = g_new(AlbumTable, 1);
    albumTable->altable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)free_album);
    return albumTable;
}

void free_album_table(AlbumTable* albumTable) {
    if (albumTable) {
        g_hash_table_destroy(albumTable->altable);
        g_free(albumTable);
    }
}

bool album_table_contains(AlbumTable *albumTable, const char *album_id) {
    return g_hash_table_contains(albumTable->altable, album_id);
}

void add_album_to_table(AlbumTable* albumTable, Album* album) {
    if (album_table_contains(albumTable, get_album_id(album))) {
        fprintf(stderr, "Error: Duplicate album ID: %s\n", get_album_id(album));
        return;
    }
    char *album_id_copy = g_strdup(get_album_id(album));
    g_hash_table_insert(albumTable->altable, album_id_copy, album);
}

void print_album_table(AlbumTable *albumTable) {
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, albumTable->altable);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Album *album = (Album *)value;
        printf("Album ID: %s\n", get_album_id(album));
        printf("Title: %s\n", get_album_title(album));
        printf("Year: %d\n", get_album_year(album));

        printf("Artist IDs: ");
        for (guint i = 0; i < album->artist_ids->len; i++) {
            printf("%s ", g_array_index(album->artist_ids, char*, i));
        }
        printf("\n");

        printf("Producers: ");
        for (guint i = 0; i < album->producers->len; i++) {
            printf("%s ", g_array_index(album->producers, char*, i));
        }
        printf("\n\n");
    }
}

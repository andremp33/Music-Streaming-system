#include "../../include/structs.h"
#include "../../include/management.h"
#include "../../include/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

#define DATASET_NAME "/home/andre/dataset/albums.csv"
#define ERRORS_DATASET_NAME "/home/andre/dataset/albums_errors.csv"

struct Album {
    char *id;
    char *title;
    GArray *artist_ids;
    int year;
    GArray *producers;
};

Album* create_album_from_line(const char *line, FILE *error_registry, AlbumTable *albumTable, ArtistTable *artistTable) {
    int has_error = 0;

    Album *album = g_malloc0(sizeof(Album));
    if (!album) {
        fprintf(stderr, "Error: Memory allocation failed for album\n");
        return NULL;
    }

    gchar **tokens = g_strsplit(line, ";", -1);
    int num_tokens = g_strv_length(tokens);

    if (num_tokens < 5) {
        register_error_line(error_registry, line);
        free_album(album);
        g_strfreev(tokens);
        return NULL;
    }

    set_album_id(album, tokens[0]);
    set_album_title(album, tokens[1]);
    set_album_artist_id(album, tokens[2], artistTable, error_registry, &has_error);
    set_album_year(album, tokens[3]);
    set_album_producers(album, tokens[4]);

    if (has_error) {
        register_error_line(error_registry, line);
        free_album(album);
        album = NULL;
        }

    g_strfreev(tokens);
    return album;
}

void set_album_id(Album *album, const char *id) {
    if (album->id) g_free(album->id);
    album->id = g_strdup(id);

    int len = strlen(album->id);
    if (len > 1 && album->id[0] == '"' && album->id[len - 1] == '"') {
        album->id[len - 1] = '\0';
        memmove(album->id, album->id + 1, len - 1);
    }
}

void set_album_title(Album *album, const char *title) {
    if (album->title) g_free(album->title);
    album->title = g_strdup(title);

    int len = strlen(album->title);
    if (len > 1 && album->title[0] == '"' && album->title[len - 1] == '"') {
        album->title[len - 1] = '\0';
        memmove(album->title, album->title + 1, len - 1);
    }
}

void set_album_artist_id(Album *album, const char *artist_id_str, ArtistTable *artistTable, FILE *error_registry, int *has_error) {
    if (!artist_id_str || strlen(artist_id_str) == 0) {
        register_error_line(error_registry, "artist_id_str is empty or NULL");
        *has_error = 1;
        return;
    }

    if (album->artist_ids) {
        for (guint i = 0; i < album->artist_ids->len; i++) {
            g_free(g_array_index(album->artist_ids, char*, i));
        }
        g_array_free(album->artist_ids, TRUE);
    }

    album->artist_ids = g_array_new(FALSE, FALSE, sizeof(char*));

    int len = strlen(artist_id_str);
    gchar *trimmed_id_str = NULL;
    if (len > 1 && artist_id_str[0] == '"' && artist_id_str[len - 1] == '"') {
        trimmed_id_str = g_strndup(artist_id_str + 1, len - 2);
        artist_id_str = trimmed_id_str;
    }

    len = strlen(artist_id_str);
    if (len >= 2 && artist_id_str[0] == '[' && artist_id_str[len - 1] == ']') {
        gchar *clean_ids = g_strndup(artist_id_str + 1, len - 2);
        gchar **ids = g_strsplit(clean_ids, ",", -1);

        for (int i = 0; ids[i] != NULL; i++) {
            gchar *id = g_strstrip(ids[i]);

            if (id[0] == '\'' && id[strlen(id) - 1] == '\'') {
                id[strlen(id) - 1] = '\0';
                memmove(id, id + 1, strlen(id));
            }

            if (strlen(id) > 0 && artist_table_contains(artistTable, id)) {
                gchar *id_copy = g_strdup(id);
                g_array_append_val(album->artist_ids, id_copy);
            } else {
                printf("Erro: artist_id '%s' não encontrado na tabela de artistas.\n", id);
                *has_error = 1;
                register_error_line(error_registry, id);
            }
        }
        g_free(clean_ids);
        g_strfreev(ids);
    } else {
        *has_error = 1;
    }

    g_free(trimmed_id_str);
}

void set_album_year(Album *album, const char *year_str) {
    if (year_str && strlen(year_str) > 0) {
        album->year = atoi(year_str);
    } else {
        album->year = 0;
    }
}

void set_album_producers(Album *album, const char *producers_str) {
    if (album->producers) {
        for (guint i = 0; i < album->producers->len; i++) {
            g_free(g_array_index(album->producers, char*, i));
        }
        g_array_free(album->producers, TRUE);
    }

    album->producers = g_array_new(FALSE, FALSE, sizeof(char*));

    gchar *clean_producers = g_strndup(producers_str + 1, strlen(producers_str) - 2);
    gchar **producers = g_strsplit(clean_producers, ",", -1);

    for (int i = 0; producers[i] != NULL; i++) {
        gchar *producer = g_strstrip(producers[i]);
        gchar *producer_copy = g_strdup(producer);
        g_array_append_val(album->producers, producer_copy);
    }

    g_free(clean_producers);
    g_strfreev(producers);
}

const char* get_album_id(Album *album) {
    return album ? album->id : NULL;
}

const char* get_album_title(Album *album) {
    return album ? album->title : NULL;
}

int get_album_year(Album *album) {
    return album ? album->year : 0;
}

GArray* get_album_artist_id(Album *album) {
    return album ? album->artist_ids : NULL;
}

GArray* get_album_producers(Album *album) {
    return album ? album->producers : NULL;
}

void free_album(void *album_ptr) {
    Album *album = (Album *)album_ptr;
    if (!album) return;

    if (album->id) g_free(album->id);
    if (album->title) g_free(album->title);

    if (album->artist_ids) {
        for (guint i = 0; i < album->artist_ids->len; i++) {
            g_free(g_array_index(album->artist_ids, char*, i));
        }
        g_array_free(album->artist_ids, TRUE);
    }

    if (album->producers) {
        for (guint i = 0; i < album->producers->len; i++) {
            g_free(g_array_index(album->producers, char*, i));
        }
        g_array_free(album->producers, TRUE);
    }

    g_free(album);
}


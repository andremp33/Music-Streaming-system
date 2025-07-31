#include "../../include/Queries/querie2.h"
#include "../../include/structs.h"
#include "../../include/management.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct AlbumTable {
    GHashTable *altable;
};

struct HistoryTable {
    GHashTable *htable;
};

struct MusicTable {
    GHashTable *mtable;
};

struct UserTable {
    GHashTable *utable;
};

struct ArtistTable {
    GHashTable *atable;
};


struct ArtistDiscography {
    Artist *artist;
    int total_duration;
};

int compare_artists(const void *a, const void *b) {
    ArtistDiscography *artist_a = (ArtistDiscography *)a;
    ArtistDiscography *artist_b = (ArtistDiscography *)b;

    if (artist_b->total_duration != artist_a->total_duration) {
        return artist_b->total_duration - artist_a->total_duration;
    }
    return strcmp(get_artist_id(artist_a->artist), get_artist_id(artist_b->artist));
}

void query_q2(STORAGE *storage, int N, const char *country_filter, bool use_separator_equals, const char *output_file) {
    if (N == 0) {
        FILE *output = fopen(output_file, "w");
        if (!output) {
            perror("Erro ao abrir arquivo de saída para Query 2");
            return;
        }
        fprintf(output, "\n");
        fclose(output);
        return;
    }

    GHashTable *artists_table =getArtists(storage)->atable;
    GHashTable *musics_table = getMusics(storage)->mtable;

    GHashTable *artists_duration = g_hash_table_new_full(
        g_str_hash,
        g_str_equal,
        g_free,
        g_free
    );

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, musics_table);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Music *music = (Music *)value;
        const char *duration_str = get_music_duration(music);

        int hh, mm, ss, music_duration;
        if (sscanf(duration_str, "%2d:%2d:%2d", &hh, &mm, &ss) == 3) {
            music_duration = hh * 3600 + mm * 60 + ss;
        } else {
            continue;
        }

        GArray *artist_ids = get_music_artist_id(music);
        for (guint i = 0; i < artist_ids->len; i++) {
            char *artist_id = g_array_index(artist_ids, char*, i);
            Artist *artist = g_hash_table_lookup(artists_table, artist_id);

            if (artist && (!country_filter || g_strcmp0(get_artist_country(artist), country_filter) == 0)) {
                int *total_duration = g_hash_table_lookup(artists_duration, artist_id);
                if (!total_duration) {
                    total_duration = g_new(int, 1);
                    *total_duration = 0;
                    g_hash_table_insert(artists_duration, g_strdup(artist_id), total_duration);
                }
                *total_duration += music_duration;
            }
        }
    }

    GArray *artists_discography = g_array_new(FALSE, FALSE, sizeof(ArtistDiscography));
    g_hash_table_iter_init(&iter, artists_duration);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        char *artist_id = (char *)key;
        int *duration = (int *)value;
        Artist *artist = g_hash_table_lookup(artists_table, artist_id);

        if (artist) {
            ArtistDiscography artist_entry = {artist, *duration};
            g_array_append_val(artists_discography, artist_entry);
        }
    }

    g_array_sort(artists_discography, compare_artists);

    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Erro ao abrir arquivo de saída para Query 2");
        g_array_free(artists_discography, TRUE);
        g_hash_table_destroy(artists_duration);
        return;
    }

    char separator;
if (use_separator_equals) {
    separator = '=';
} else {
    separator = ';';
}
if (separator != '=' && separator != ';') {
    fprintf(stderr, "Erro: Separador inválido detectado. Usando ';' como padrão.\n");
    separator = ';';
}

    for (guint i = 0; i < N && i < artists_discography->len; i++) {
        ArtistDiscography *entry = &g_array_index(artists_discography, ArtistDiscography, i);
        int duration = entry->total_duration;
        int hours = duration / 3600;
        int minutes = (duration % 3600) / 60;
        int seconds = duration % 60;

        fprintf(output, "%s%c%s%c%02d:%02d:%02d%c%s\n", 
                get_artist_name(entry->artist), 
                separator, 
                get_artist_type(entry->artist), 
                separator, 
                hours, minutes, seconds, 
                separator, 
                get_artist_country(entry->artist));
    }

    fclose(output);
    g_array_free(artists_discography, TRUE);
    g_hash_table_destroy(artists_duration);
}

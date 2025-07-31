#include "../../include/Queries/querie6.h"
#include "../../include/utils.h"
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


struct storage {
    UserTable *users;
    ArtistTable *artists;
    MusicTable *musics;
    AlbumTable *albums;
    HistoryTable *history;
};

struct UserSummary {
    char *total_listening_time;   
    guint distinct_musics;       
    char *top_artist;            
    char *most_listened_day;     
    char *top_genre;             
    char *favorite_album;        
    char *peak_hour;             
};

struct ArtistStat{
    char *artist_id;
    char *artist_name;
    int total_time;             // Em segundos
    GHashTable *tracks;         // Para guardar IDs de músicas únicas
};

static UserSummary* create_user_summary(GHashTable *user_history, MusicTable *musics, 
                                        ArtistTable *artists, AlbumTable *albums) {
    UserSummary *summary = g_malloc0(sizeof(UserSummary));
    summary->total_listening_time = calculate_total_listening_time(user_history);

    summary->distinct_musics = count_distinct_musics(user_history);

    summary->top_artist = get_top_artist(user_history, musics, artists);
    summary->most_listened_day = get_day_with_most_listens(user_history);
    summary->top_genre = get_top_genre(user_history, musics);
    summary->favorite_album = get_favorite_album(user_history, musics, albums);
    summary->peak_hour = get_peak_listening_hour(user_history);

    return summary;
}


void free_user_summary(UserSummary *summary) {
    if (!summary) return;
    g_free(summary->total_listening_time);
    g_free(summary->top_artist);
    g_free(summary->most_listened_day);
    g_free(summary->top_genre);
    g_free(summary->favorite_album);
    g_free(summary->peak_hour);
    g_free(summary);
}

void execute_querie6(STORAGE *storage, const char *user_id, int year, int top_n, bool use_separator_equals, const char *output_file) {
    GHashTable *user_history = filter_history_by_user_and_year(getHistory(storage)->htable, user_id, year);
    if (!g_hash_table_size(user_history)) {
        FILE *output = fopen(output_file, "w");
        if (!output) {
            perror("Erro ao abrir arquivo de saída para Query 6");
            g_hash_table_destroy(user_history);
            return;
        }
        fprintf(output, "\n");
        fclose(output);
        g_hash_table_destroy(user_history);
        return;
    }

    UserSummary *summary = create_user_summary(user_history, getMusics(storage), getArtists(storage), getAlbums(storage));
    if (!summary) {
        fprintf(stderr, "Erro ao criar resumo anual para o usuário %s.\n", user_id);
        g_hash_table_destroy(user_history);
        return;
    }

    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Erro ao abrir arquivo de saída para Query 6");
        free_user_summary(summary);
        g_hash_table_destroy(user_history);
        return;
    }

    char separator = use_separator_equals ? '=' : ';';

    fprintf(output, "%s%c%u%c%s%c%s%c%s%c%s%c%s\n",
            summary->total_listening_time, separator,
            summary->distinct_musics, separator,
            summary->top_artist, separator,
            summary->most_listened_day, separator,
            summary->top_genre, separator,
            summary->favorite_album, separator,
            summary->peak_hour);

    if (top_n > 0) {
        GHashTable *artist_stats = calculate_artist_stats(user_history, getMusics(storage), getArtists(storage));
        GList *sorted_artists = sort_artist_stats(artist_stats);

        int count = 0;
        for (GList *node = sorted_artists; node && count < top_n; node = node->next) {
            ArtistStat *stat = (ArtistStat *)node->data;
            fprintf(output, "%s%c%u%c%02d:%02d:%02d\n",
                    stat->artist_id, separator,
                    g_hash_table_size(stat->tracks), separator,
                    stat->total_time / 3600,
                    (stat->total_time % 3600) / 60,
                    stat->total_time % 60);
            count++;
        }

        g_list_free_full(sorted_artists, free_artist_stat);
        g_hash_table_destroy(artist_stats);
    }

    fclose(output);
    free_user_summary(summary);
    g_hash_table_destroy(user_history);
}



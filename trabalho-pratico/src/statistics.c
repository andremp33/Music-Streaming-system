#include "../include/structs.h"
#include "../include/utils.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct History {
    char *id;         
    char *user_id;     
    char *music_id;    
    char *timestamp;   
    char *duration;      
    char *platform;    
};

struct Artist {
    char *id;
    char *name;
    char *description;
    char *recipe_str;
    GArray *id_constituent_str;
    char *country;
    char *type;
    double total_revenue;
    int num_albums;
    double participation_revenue;
    GArray *group_ids;
};

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

struct Date {
    int year;
    int month;
    int day;
};

struct ArtistTime {
    char *artist_id;
    int total_time;
};


int get_music_reproductions_from_history(GHashTable *history_table, const char *music_id) {
    if (!history_table || !music_id) return 0;

    int total_reproductions = 0;
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, history_table);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        if (g_strcmp0(entry->music_id, music_id) == 0) {
            total_reproductions++;
        }
    }

    return total_reproductions;
}

double calculate_total_revenue_for_artist(const char *artist_id, ArtistTable *artists_table, MusicTable *musics_table, HistoryTable *history_table) {
    if (!artist_id || !artists_table || !artists_table->atable || !musics_table || !musics_table->mtable || !history_table || !history_table->htable) {
        //fprintf(stderr, "Erro: Parâmetros inválidos fornecidos para calculate_total_revenue_for_artist.\n");
        //fprintf(stderr, "Debug: artist_id=%s, artists_table=%p, musics_table=%p, history_table=%p\n",
               // artist_id, (void *)artists_table, (void *)musics_table, (void *)history_table);
        return 0.0;
    }


    GHashTable *musics = musics_table->mtable;
    GHashTable *history = history_table->htable;

    if (!musics || !history) {
        //fprintf(stderr, "Erro: Tabelas de músicas ou histórico não inicializadas.\n");
        return 0.0;
    }

    double total_revenue = 0.0;
    GHashTableIter music_iter;
    gpointer music_key, music_value;

    g_hash_table_iter_init(&music_iter, musics);
    while (g_hash_table_iter_next(&music_iter, &music_key, &music_value)) {
        Music *music = (Music *)music_value;
        if (!music) continue;

        GArray *artist_ids = get_music_artist_id(music);
        gboolean artist_found = FALSE;

        for (guint i = 0; i < artist_ids->len; i++) {
            if (g_strcmp0(g_array_index(artist_ids, char *, i), artist_id) == 0) {
                artist_found = TRUE;
                break;
            }
        }

        if (!artist_found) continue;

        const char *music_id = get_music_id(music);
        if (!music_id) continue;

        int reproductions = get_music_reproductions_from_history(history, music_id);

        Artist *artist = g_hash_table_lookup(artists_table->atable, artist_id);
        if (!artist) {
            //fprintf(stderr, "Erro: Artista %s não encontrado na tabela de artistas.\n", artist_id);
            continue;
        }

        const char *rate_str = get_artist_recipe_per_stream(artist);
        if (!rate_str) {
            //fprintf(stderr, "Erro: Receita por stream não encontrada para o artista %s.\n", artist_id);
            continue;
        }

        double rate_per_stream = atof(rate_str);
        total_revenue += reproductions * rate_per_stream;
    }

    return total_revenue;
}


double calculate_participation_revenue_for_artist(const char *artist_id,
                                                  ArtistTable *artists_table,
                                                  MusicTable *musics_table,
                                                  HistoryTable *history_table) {
    if (!artist_id || !artists_table || !artists_table->atable) {
        //fprintf(stderr, "Erro: Parâmetros inválidos fornecidos para calculate_participation_revenue_for_artist.\n");
        return 0.0;
    }

    Artist *artist = g_hash_table_lookup(artists_table->atable, artist_id);
    if (!artist || !artist->group_ids) {
        //fprintf(stderr, "Debug: Artista %s não pertence a nenhum grupo.\n", artist_id);
        return 0.0;
    }


    double participation_revenue = 0.0;

    for (guint i = 0; i < artist->group_ids->len; i++) {
        const char *group_id = g_array_index(artist->group_ids, char *, i);
        Artist *group = g_hash_table_lookup(artists_table->atable, group_id);

        if (group && strcmp(get_artist_type(group), "group") == 0) {
            //fprintf(stderr, "Debug: Grupo associado ao artista: %s\n", group_id);

            double group_revenue = calculate_total_revenue_for_artist(
                group_id, artists_table, musics_table, history_table);

            guint members_count = group->id_constituent_str ? group->id_constituent_str->len : 1;
            if (members_count > 0) {
                double revenue_share = group_revenue / members_count;
                participation_revenue += revenue_share;

                //fprintf(stderr, "Debug: Receita de participação do grupo %s para o artista %s: %.2f\n",group_id, artist_id, revenue_share);
            } else {
                //fprintf(stderr, "Debug: Grupo %s não possui membros válidos.\n", group_id);
            }
        } else {
            //fprintf(stderr, "Debug: Grupo %s não encontrado ou não é do tipo 'group'.\n", group_id);
        }
    }

    return participation_revenue;
}



int calculate_num_albums_for_artist(const char *artist_id, AlbumTable *albums_table) {
    if (!artist_id || !albums_table) return 0;

    int num_albums = 0;
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, albums_table->altable);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Album *album = (Album *)value;
        GArray *artist_ids = get_album_artist_id(album);

        for (guint i = 0; i < artist_ids->len; i++) {
            if (g_strcmp0(g_array_index(artist_ids, char *, i), artist_id) == 0) {
                num_albums++;
                break;
            }
        }
    }

    return num_albums;
}


void process_weekly_top10(GHashTable *weekly_top10, GHashTable *artist_appearances, const char *artist_type) {
    if (!weekly_top10 || !artist_appearances || !artist_type) {
        fprintf(stderr, "Erro: Argumentos inválidos para process_weekly_top10.\n");
        return;
    }

    GHashTableIter week_iter;
    gpointer week_key, week_value;

    g_hash_table_iter_init(&week_iter, weekly_top10);
    while (g_hash_table_iter_next(&week_iter, &week_key, &week_value)) {
        const char *week = (const char *)week_key;
        GList *top10 = (GList *)week_value;

        int count = 0;
        for (GList *node = top10; node && count < 10; node = node->next, count++) {
            ArtistTime *artist_time = (ArtistTime *)node->data;
            gpointer appearances = g_hash_table_lookup(artist_appearances, artist_time->artist_id);

            if (appearances) {
                int updated_count = GPOINTER_TO_INT(appearances) + 1;
                g_hash_table_replace(artist_appearances, g_strdup(artist_time->artist_id), GINT_TO_POINTER(updated_count));
            } else {
                g_hash_table_insert(artist_appearances, g_strdup(artist_time->artist_id), GINT_TO_POINTER(1));
            }
        }

        g_list_free_full(top10, (GDestroyNotify)free_artist_time);
    }
}


unsigned int count_distinct_musics(GHashTable *user_history) {
    GHashTable *unique_musics = g_hash_table_new(g_str_hash, g_str_equal);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, user_history);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        const char *music_id = get_history_music_id(entry);
        g_hash_table_add(unique_musics, (gpointer)music_id);
    }

    unsigned int count = g_hash_table_size(unique_musics);
    g_hash_table_destroy(unique_musics);

    return count;
}


char* calculate_total_listening_time(GHashTable *user_history) {
    int total_seconds = 0;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, user_history);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        const char *duration = get_history_duration(entry);
        int hh, mm, ss;
        sscanf(duration, "%d:%d:%d", &hh, &mm, &ss);
        total_seconds += hh * 3600 + mm * 60 + ss;
    }

    char *result = g_malloc0(16); 
    snprintf(result, 16, "%02d:%02d:%02d", 
             total_seconds / 3600, 
             (total_seconds % 3600) / 60, 
             total_seconds % 60);

    return result;
}


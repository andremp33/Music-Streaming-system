#include "../include/utils.h"
#include "../include/structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <glib.h>


#define MAX_PATH_LENGTH 512

struct Date {
    int year;
    int month;
    int day;
};

struct ArtistTime {
    char *artist_id;
    int total_time;
};

struct MusicTable {
    GHashTable *mtable;
};

struct ArtistTable {
    GHashTable *atable;
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

struct ArtistStat{
    char *artist_id;
    char *artist_name;
    int total_time;             // Em segundos
    GHashTable *tracks;         // Para guardar IDs de músicas únicas
};

char* date_to_string(struct Date *date) {
    if (!date) return NULL;
    char *date_str = g_malloc0(11); // Formato "YYYY/MM/DD" + '\0'
    snprintf(date_str, 11, "%04d/%02d/%02d", date->year, date->month, date->day);
    return date_str;
}

FILE *initialize_error_registery(const char *outputDir, const char *error_dataset) {
    char error_dataset_path[MAX_PATH_LENGTH];

    if (outputDir[strlen(outputDir) - 1] == '/') {
        snprintf(error_dataset_path, MAX_PATH_LENGTH, "%s%s", outputDir, error_dataset);
    } else {
        snprintf(error_dataset_path, MAX_PATH_LENGTH, "%s/%s", outputDir, error_dataset);
    }
    FILE *file = fopen(error_dataset_path, "w");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}

void register_error_line(FILE *error_registery, const char *line) {
    if (error_registery && line) {
        fprintf(error_registery, "%s\n", line);
        fflush(error_registery); 
    }
}

void close_error_registery(FILE *file) {
    fclose(file);
}

void save_result(int line, const char *res, const char *outputDirectory) {
    char *output_file = g_malloc(sizeof(char) * MAX_BUFFER_SIZE);
    if (outputDirectory[strlen(outputDirectory) - 1] == '/') {
        snprintf(output_file, MAX_PATH_LENGTH, "%scommand%d_output.txt", outputDirectory, line);
    } else {
        snprintf(output_file, MAX_PATH_LENGTH, "%s/command%d_output.txt", outputDirectory, line);
    }
    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        perror("Error opening output file!\n");
        exit(1);
    }
    fprintf(fp, "%s", res);
    fclose(fp);
    g_free(output_file);
}

void validate_csv_list(const char *list, int *has_error) {
    int len = strlen(list);
    if (!(len >= 2 && list[0] == '[' && list[len - 1] == ']')) {
        *has_error = 1; 
    }
}

FILE *initialize_file_saving(int line, const char *outputDirectory) {
    char *output_file = g_malloc(sizeof(char) * MAX_BUFFER_SIZE);
    if (outputDirectory[strlen(outputDirectory) - 1] == '/') {
        snprintf(output_file, MAX_PATH_LENGTH, "%scommand%d_output.txt", outputDirectory, line);
    } else {
        snprintf(output_file, MAX_PATH_LENGTH, "%s/command%d_output.txt", outputDirectory, line);
    }
    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        perror("Error opening output file!\n");
        exit(1);
    }
    g_free(output_file);
    return fp;
}

void save_line(FILE *file, const char *line) {
    fprintf(file, "%s", line);
}

void close_file_saving(FILE *file) {
    fclose(file);
}


bool is_valid_date(const char *date) {
    if (!date || strlen(date) != 10 || date[4] != '/' || date[7] != '/') {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        if ((i != 4 && i != 7) && !isdigit(date[i])) {
            return false;
        }
    }
    return true;
}



char* get_week_from_timestamp(const char *timestamp) {
    if (!timestamp || strlen(timestamp) != 19) { 
        return NULL;
    }

    struct tm time_struct = {0};
    char date_part[11]; 


    strncpy(date_part, timestamp, 10);
    date_part[10] = '\0';

    if (strptime(date_part, "%Y/%m/%d", &time_struct) == NULL) {
        return NULL;
    }

    time_t raw_time = mktime(&time_struct);
    if (raw_time == -1) {
        return NULL;
    }

    localtime_r(&raw_time, &time_struct);


    time_struct.tm_mday -= time_struct.tm_wday;

    raw_time = mktime(&time_struct);
    if (raw_time == -1) {
        return NULL;
    }

    char *week_start = malloc(11);
    if (!week_start) {
        return NULL;
    }
    strftime(week_start, 11, "%Y/%m/%d", &time_struct);

    return week_start;
}

const char* get_artist_from_music(const char *music_id, MusicTable *musicTable, ArtistTable *artistTable) {
    if (!music_id || !musicTable || !artistTable) {
        fprintf(stderr, "Erro: Parâmetros inválidos fornecidos a get_artist_from_music.\n");
        return NULL;
    }

    Music *music = g_hash_table_lookup(musicTable->mtable, music_id);
    if (!music) {
        fprintf(stderr, "Erro: Música com ID '%s' não encontrada.\n", music_id);
        return NULL;
    }

    GArray *artist_ids = get_music_artist_id(music);
    if (!artist_ids || artist_ids->len == 0) {
        fprintf(stderr, "Erro: Nenhum artista associado à música com ID '%s'.\n", music_id);
        return NULL;
    }

    for (guint i = 0; i < artist_ids->len; i++) {
        const char *artist_id = g_array_index(artist_ids, char *, i);
        Artist *artist = g_hash_table_lookup(artistTable->atable, artist_id);
        if (artist) {
            return get_artist_id(artist); 
        }
    }

    fprintf(stderr, "Erro: Nenhum artista válido encontrado para a música com ID '%s'.\n", music_id);
    return NULL;
}


char* get_top_artist(GHashTable *user_history, MusicTable *musics_table, ArtistTable *artists_table) {
    GHashTable *artist_time = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, user_history);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        const char *music_id = get_history_music_id(entry);
        Music *music = g_hash_table_lookup(musics_table->mtable, music_id);
        if (!music) continue;

        GArray *artist_ids = get_music_artist_id(music);
        const char *duration = get_history_duration(entry);
        int hh, mm, ss;
        sscanf(duration, "%d:%d:%d", &hh, &mm, &ss);
        int time_in_seconds = hh * 3600 + mm * 60 + ss;

        for (guint i = 0; i < artist_ids->len; i++) {
            const char *artist_id = g_array_index(artist_ids, char*, i);
            int *current_time = g_hash_table_lookup(artist_time, artist_id);
            if (!current_time) {
                int *new_time = g_malloc(sizeof(int));
                *new_time = time_in_seconds;
                g_hash_table_insert(artist_time, g_strdup(artist_id), new_time);
            } else {
                *current_time += time_in_seconds;
            }
        }
    }

    GHashTableIter artist_iter;
    gpointer artist_key, artist_value;
    char *top_artist = NULL;
    int max_time = 0;

    g_hash_table_iter_init(&artist_iter, artist_time);
    while (g_hash_table_iter_next(&artist_iter, &artist_key, &artist_value)) {
        int *time = (int *)artist_value;
        if (*time > max_time) {
            max_time = *time;
            top_artist = (char *)artist_key;
        }
        else if (*time == max_time && top_artist != NULL) {
            if (atoi((char *)artist_key) < atoi(top_artist)) {
                top_artist = (char *)artist_key;
            }
        }
    }

    g_hash_table_destroy(artist_time);
    return g_strdup(top_artist);
}
 

char* get_day_with_most_listens(GHashTable *user_history) {
    GHashTable *date_count = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, user_history);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        char date[11];
        sscanf(get_history_timestamp(entry), "%10s", date);

        int *count = g_hash_table_lookup(date_count, date);
        if (!count) {
            int *new_count = g_malloc(sizeof(int));
            *new_count = 1;
            g_hash_table_insert(date_count, g_strdup(date), new_count);
        } else {
            (*count)++;
        }
    }

    GHashTableIter date_iter;
    gpointer date_key, date_value;
    char *top_date = NULL;
    int max_count = 0;

    g_hash_table_iter_init(&date_iter, date_count);
    while (g_hash_table_iter_next(&date_iter, &date_key, &date_value)) {
        int *count = (int *)date_value;

        if (*count > max_count) {
            max_count = *count;
            top_date = (char *)date_key;
        }
        else if (*count == max_count && top_date != NULL) {
            if (strcmp((char *)date_key, top_date) > 0) {
                top_date = (char *)date_key;
            }
        }
    }

    g_hash_table_destroy(date_count);
    return g_strdup(top_date);
}

char* get_top_genre(GHashTable *user_history, MusicTable *musics_table) {
    GHashTable *genre_time = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, user_history);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        const char *music_id = get_history_music_id(entry);
        Music *music = g_hash_table_lookup(musics_table->mtable, music_id);
        if (!music) continue;

        const char *genre = get_music_genre(music);
        const char *duration = get_history_duration(entry);
        int hh, mm, ss;
        sscanf(duration, "%d:%d:%d", &hh, &mm, &ss);
        int time_in_seconds = hh * 3600 + mm * 60 + ss;

        int *current_time = g_hash_table_lookup(genre_time, genre);
        if (!current_time) {
            int *new_time = g_malloc(sizeof(int));
            *new_time = time_in_seconds;
            g_hash_table_insert(genre_time, g_strdup(genre), new_time);
        } else {
            *current_time += time_in_seconds;
        }
    }

    GHashTableIter genre_iter;
    gpointer genre_key, genre_value;
    char *top_genre = NULL;
    int max_time = 0;

    g_hash_table_iter_init(&genre_iter, genre_time);
    while (g_hash_table_iter_next(&genre_iter, &genre_key, &genre_value)) {
        int *time = (int *)genre_value;
        if (*time > max_time) {
            max_time = *time;
            top_genre = (char *)genre_key;
        }
        else if (*time == max_time && top_genre != NULL) {
            if (strcmp((char*)genre_key, top_genre) < 0) {
                top_genre = (char *)genre_key;
            }
        }
    }

    g_hash_table_destroy(genre_time);
    return g_strdup(top_genre);
}


char* get_favorite_album(GHashTable *user_history, MusicTable *musics_table, AlbumTable *albums_table) {
    GHashTable *album_time = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, user_history);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        const char *music_id = get_history_music_id(entry);
        Music *music = g_hash_table_lookup(musics_table->mtable, music_id);
        if (!music) continue;

        const char *album_id = get_music_album_id(music);
        const char *duration = get_history_duration(entry);
        int hh, mm, ss;
        sscanf(duration, "%d:%d:%d", &hh, &mm, &ss);
        int time_in_seconds = hh * 3600 + mm * 60 + ss;

        int *current_time = g_hash_table_lookup(album_time, album_id);
        if (!current_time) {
            int *new_time = g_malloc(sizeof(int));
            *new_time = time_in_seconds;
            g_hash_table_insert(album_time, g_strdup(album_id), new_time);
        } else {
            *current_time += time_in_seconds;
        }
    }

    GHashTableIter album_iter;
    gpointer album_key, album_value;
    char *top_album = NULL;
    int max_time = 0;

    g_hash_table_iter_init(&album_iter, album_time);
    while (g_hash_table_iter_next(&album_iter, &album_key, &album_value)) {
        int *time = (int *)album_value;
        if (*time > max_time) {
            max_time = *time;
            top_album = (char *)album_key;
        }
        else if (*time == max_time && top_album != NULL) {
            if (strcmp((char*)album_key, top_album) < 0) {
                top_album = (char *)album_key;
            }
        }
    }

    g_hash_table_destroy(album_time);
    return g_strdup(top_album);
}

char* get_peak_listening_hour(GHashTable *user_history) {
    int hour_count[24] = {0};
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, user_history);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        const char *timestamp = get_history_timestamp(entry);

        int hour;
        sscanf(timestamp, "%*d/%*d/%*d %2d", &hour);
        if (hour >= 0 && hour < 24) {
            hour_count[hour]++;
        }
    }

    int peak_hour = 0;
    int max_count = 0;
    for (int i = 0; i < 24; i++) {
        if (hour_count[i] > max_count) {
            max_count = hour_count[i];
            peak_hour = i;
        }
    }

    char *result = g_malloc0(3); 
    snprintf(result, 3, "%02d", peak_hour);
    return result;
}


gint compare_artist_stats(gconstpointer a, gconstpointer b) {
    const ArtistStat* s1 = (const ArtistStat*)a;
    const ArtistStat* s2 = (const ArtistStat*)b;

    if (s1->total_time > s2->total_time) return -1;
    if (s1->total_time < s2->total_time) return 1;

    int id1 = atoi(s1->artist_id);
    int id2 = atoi(s2->artist_id);
    return (id1 - id2);
}

void free_artist_stat(gpointer data) {
    ArtistStat *stat = (ArtistStat*)data;
    if (stat) {
        g_hash_table_destroy(stat->tracks); 
        g_free(stat->artist_id);
        g_free(stat->artist_name);
        g_free(stat);
    }
}

GList* sort_artist_stats(GHashTable *artist_stats) {
    GList *list = NULL;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, artist_stats);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        list = g_list_prepend(list, value);
    }
    list = g_list_sort(list, (GCompareFunc)compare_artist_stats);
    return list;
}

bool artist_belongs_to_any_group(const Artist *artist) {
    if (!artist || !artist->group_ids || artist->group_ids->len == 0) {
        fprintf(stderr, "Debug: Artista %s não pertence a nenhum grupo.\n", artist->id);
        return false;
    }

    fprintf(stderr, "Debug: Artista %s pertence a %u grupos.\n",
            artist->id, artist->group_ids->len);

    return true;
}

GHashTable* calculate_artist_stats(GHashTable *user_history, MusicTable *musics_table, ArtistTable *artists_table) {
    GHashTable *artist_stats = g_hash_table_new(g_str_hash, g_str_equal);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, user_history);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        const char *music_id = get_history_music_id(entry);
        Music *music = g_hash_table_lookup(musics_table->mtable, music_id);
        if (!music) continue;
        const char *duration = get_history_duration(entry);
        int hh, mm, ss;
        sscanf(duration, "%d:%d:%d", &hh, &mm, &ss);
        int time_in_seconds = hh * 3600 + mm * 60 + ss;
        GArray *artist_ids = get_music_artist_id(music);
        if (!artist_ids) continue;

        for (guint i = 0; i < artist_ids->len; i++) {
            const char *artist_id = g_array_index(artist_ids, char*, i);

            ArtistStat *stat = (ArtistStat *)g_hash_table_lookup(artist_stats, artist_id);
            if (!stat) {
                stat = g_malloc0(sizeof(ArtistStat));
                stat->artist_id = g_strdup(artist_id);

                Artist *artist_obj = g_hash_table_lookup(artists_table->atable, artist_id);
                if (artist_obj) {
                    stat->artist_name = g_strdup(get_artist_name(artist_obj));
                } else {
                    stat->artist_name = g_strdup("Unknown Artist");
                }

                stat->total_time = time_in_seconds;

                stat->tracks = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
                g_hash_table_insert(stat->tracks, g_strdup(music_id), GINT_TO_POINTER(1));

                g_hash_table_insert(artist_stats, stat->artist_id, stat);
            } else {
                stat->total_time += time_in_seconds;
                if (!g_hash_table_lookup(stat->tracks, music_id)) {
                    g_hash_table_insert(stat->tracks, g_strdup(music_id), GINT_TO_POINTER(1));
                }
            }
        }
    }

    return artist_stats;
}


const char* get_artist_name_from_id(const char *artist_id, ArtistTable *artistTable) {
    if (!artist_id || !artistTable) return NULL;

    Artist *artist = g_hash_table_lookup(artistTable->atable, artist_id);
    return artist ? get_artist_name(artist) : NULL;
}

const char* get_artist_type_from_id(const char *artist_id, ArtistTable *artistTable) {
    if (!artist_id || !artistTable) return NULL;

    Artist *artist = g_hash_table_lookup(artistTable->atable, artist_id);
    return artist ? get_artist_type(artist) : NULL;
}

void free_artist_time(ArtistTime *artist_time) {
    if (!artist_time) return;

    if (artist_time->artist_id) {
        g_free(artist_time->artist_id);
    }

    g_free(artist_time);
}


const char* get_artist_country_from_id(const char *artist_id, ArtistTable *artistTable) {
    if (!artist_id || !artistTable) {
        return NULL;
    }

    Artist *artist = g_hash_table_lookup(artistTable->atable, artist_id);
    return artist ? get_artist_country(artist) : NULL;
}
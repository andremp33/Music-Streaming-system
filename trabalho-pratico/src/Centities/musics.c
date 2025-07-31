#include "../../include/structs.h"
#include "../../include/management.h"
#include "../../include/utils.h"
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DATASET_NAME "/home/andre/dataset/musics1.csv"
#define ERRORS_DATASET_NAME "/home/andre/dataset/musics_errors.csv"

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

struct MusicTable {
    GHashTable *mtable;
};



void debug_print_music_table(MusicTable *musicTable) {
    printf("Debug: Lista de músicas na tabela:\n");
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, musicTable->mtable);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Music *music = (Music *)value;
        printf("Music ID: %s\n", get_music_id(music));
        printf("Genre: %s\n", get_music_genre(music));
    }
}


Music* create_music_from_line(const char *line, FILE *error_registry, MusicTable *musicTable, ArtistTable *artistTable, AlbumTable *albumTable) {
    int has_error = 0;

    if (strchr(line, ';') == NULL) {
        fprintf(stderr, "Erro: Linha inválida sem separadores detectada '%s'\n", line);
        register_error_line(error_registry, line);
        return NULL;
    }

    Music *music = g_malloc0(sizeof(Music));
    if (!music) {
        fprintf(stderr, "Error: Memory allocation failed for music\n");
        return NULL;
    }

    gchar **tokens = g_strsplit(line, ";", -1);
    int num_tokens = g_strv_length(tokens);

    if (num_tokens < 8) {
        fprintf(stderr, "Error: Incomplete line detected '%s'\n", line);
        register_error_line(error_registry, line);
        free_music(music);
        g_strfreev(tokens);
        return NULL;
    }

    set_music_id(music, tokens[0]);
    set_music_title(music, tokens[1]);
    set_music_artist_id(music, tokens[2], artistTable, error_registry, &has_error);
    set_music_album_id(music, tokens[3], albumTable, error_registry, &has_error);
    set_music_duration(music, tokens[4], &has_error);
    set_music_genre(music, tokens[5]);
    set_music_year(music, tokens[6]);
    music->lyrics = NULL; // No memory allocated for lyrics

    if (has_error) {
        register_error_line(error_registry, line);
        free_music(music);
        music = NULL;
    }
    g_strfreev(tokens);
    return music;
}


void set_music_id(Music *music, const char *id) {
    if (music->id) g_free(music->id);
    music->id = g_strdup(id);

    int len = strlen(music->id);
    if (len > 1 && music->id[0] == '"' && music->id[len - 1] == '"') {
        music->id[len - 1] = '\0';
        memmove(music->id, music->id + 1, len - 1);
    }
}

void set_music_title(Music *music, const char *title) {
    if (music->title) g_free(music->title);
    music->title = g_strdup(title);

    int len = strlen(music->title);
    if (len > 1 && music->title[0] == '"' && music->title[len - 1] == '"') {
        music->title[len - 1] = '\0';
        memmove(music->title, music->title + 1, len - 1);
    }
}

void set_music_duration(Music *music, const char *duration, int *has_error) {
    char *cleaned_duration = g_strdup(duration);
    int len = strlen(cleaned_duration);

    if (len > 1 && cleaned_duration[0] == '"' && cleaned_duration[len - 1] == '"') {
        cleaned_duration[len - 1] = '\0';
        memmove(cleaned_duration, cleaned_duration + 1, len - 1);
    }

    if (strlen(cleaned_duration) != 8 || cleaned_duration[2] != ':' || cleaned_duration[5] != ':') {
        *has_error = 1;
        g_free(cleaned_duration);
        return;
    }

    int hh, mm, ss;
    char hh_str[3] = {cleaned_duration[0], cleaned_duration[1], '\0'};
    char mm_str[3] = {cleaned_duration[3], cleaned_duration[4], '\0'};
    char ss_str[3] = {cleaned_duration[6], cleaned_duration[7], '\0'};

    hh = atoi(hh_str);
    mm = atoi(mm_str);
    ss = atoi(ss_str);

    if (hh < 0 || hh > 99 || mm < 0 || mm > 59 || ss < 0 || ss > 59) {
        *has_error = 1;
        g_free(cleaned_duration);
        return;
    }

    if (music->duration) g_free(music->duration);
    music->duration = cleaned_duration;  
}

void set_music_genre(Music *music, const char *genre) {
    if (music->genre) g_free(music->genre);
    music->genre = g_strdup(genre);

    int len = strlen(music->genre);
    if (len > 1 && music->genre[0] == '"' && music->genre[len - 1] == '"') {
        music->genre[len - 1] = '\0';
        memmove(music->genre, music->genre + 1, len - 1);
    }
}



void set_music_year(Music *music, const char *year_str) {
    int len = strlen(year_str);
    char *cleaned_year = g_strdup(year_str);

    if (len > 1 && year_str[0] == '"' && year_str[len - 1] == '"') {
        cleaned_year[len - 1] = '\0';
        memmove(cleaned_year, cleaned_year + 1, len - 1);
    }
    music->year_str = atoi(cleaned_year);
    g_free(cleaned_year);
}

void set_music_artist_id(Music *music, const char *artist_id_str, ArtistTable *artistTable, FILE *error_registry, int *has_error) {
    if (artist_id_str == NULL || strlen(artist_id_str) == 0) {
        register_error_line(error_registry, "artist_id_str is empty or NULL");
        *has_error = 1;
        return;
    }

    if (music->artist_id_str) {
        for (guint i = 0; i < music->artist_id_str->len; i++) {
            g_free(g_array_index(music->artist_id_str, char*, i));
        }
        g_array_free(music->artist_id_str, TRUE);
    }
    music->artist_id_str = g_array_new(FALSE, FALSE, sizeof(char*));

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
                g_array_append_val(music->artist_id_str, id_copy);
            } else {
                *has_error = 1;
            }
        }
        g_free(clean_ids);
        g_strfreev(ids);
    } else {
        *has_error = 1;
    }

    g_free(trimmed_id_str);
}

void set_music_album_id(Music *music, const char *album_id_str, AlbumTable *albumTable, FILE *error_registry, int *has_error) {
    if (album_id_str == NULL || strlen(album_id_str) == 0) {
        register_error_line(error_registry, "album_id is empty or NULL");
        *has_error = 1;
        return;
    }

    char *cleaned_album_id = g_strdup(album_id_str);
    int len = strlen(cleaned_album_id);

    if (len > 1 && cleaned_album_id[0] == '"' && cleaned_album_id[len - 1] == '"') {
        cleaned_album_id[len - 1] = '\0';
        memmove(cleaned_album_id, cleaned_album_id + 1, len - 1);
        len = strlen(cleaned_album_id);
    }

    if (len != 8 || strncmp(cleaned_album_id, "AL", 2) != 0) {
        *has_error = 1;
        g_free(cleaned_album_id);
        return;
    }

    if (!album_table_contains(albumTable, cleaned_album_id)) {
        *has_error = 1;
        g_free(cleaned_album_id);
        return;
    }

    if (music->album_id_str) g_free(music->album_id_str);
    music->album_id_str = cleaned_album_id;
}

const char* get_music_id(Music *music) { 
    return music ? music->id : NULL;
}
const char* get_music_title(Music *music) {
     return music ? music->title : NULL;
}
GArray* get_music_artist_id(Music *music) { 
    return music ? music->artist_id_str : NULL; 
}
const char* get_music_duration(Music *music) { 
    return music ? music->duration : NULL;
}
const char* get_music_album_id(Music *music) { 
return music ? music->album_id_str : NULL;
}
const char* get_music_genre(Music *music) {
    return music ? music->genre : NULL; 
}
int get_music_year(Music *music) {
     return music ? music->year_str : 0;
}


void free_music(void *music_ptr) {
    Music *music = (Music *)music_ptr;
    if (music == NULL) return;

    if (music->id) g_free(music->id);
    if (music->title) g_free(music->title);
    if (music->duration) g_free(music->duration);
    if (music->genre) g_free(music->genre);
    if (music->lyrics) g_free(music->lyrics);
    if (music->album_id_str) g_free(music->album_id_str);


    if (music->artist_id_str) {
        for (guint i = 0; i < music->artist_id_str->len; i++) {
            char *artist_id = g_array_index(music->artist_id_str, char*, i);
            if (artist_id) g_free(artist_id);
        }
        g_array_free(music->artist_id_str, TRUE);
    }

    g_free(music);
}

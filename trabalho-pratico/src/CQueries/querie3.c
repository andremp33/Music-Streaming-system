__asm__(".section .note.GNU-stack,\"\",@progbits");
#include "../../include/Queries/querie3.h"
#include "../../include/structs.h"
#include "../../include/management.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define NUM_THREADS 1

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

struct GenreLikes {
    char *genre;
    int total_likes;
};

struct ThreadData {
    GHashTable *users_table;
    GHashTable *musics_table;
    int birth_year_min;
    int birth_year_max;
    GHashTable *local_genre_likes;
    int start_index;
    int end_index;
};

void print_final_genre_counts(GHashTable *global_genre_likes) {
    printf("\nResultado Final por Gênero:\n");
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, global_genre_likes);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        printf("Gênero: %s, Total Likes: %d\n", (char *)key, *(int *)value);
    }
}

void validate_song_ids(GArray *liked_songs, GHashTable *musics_table) {
    fprintf(stderr, "Validando IDs de músicas curtidas...\n");
    for (guint i = 0; i < liked_songs->len; i++) {
        char *song_id = g_array_index(liked_songs, char *, i);

        g_strstrip(song_id);
        for (char *p = song_id; *p; p++) *p = g_ascii_tolower(*p);

        if (!g_hash_table_contains(musics_table, song_id)) {
            fprintf(stderr, "Erro: Música '%s' não encontrada na tabela.\n", song_id);
        }
    }
}

int compare_genres(const void *a, const void *b) {
    const GenreLikes *genre_a = (const GenreLikes *)a;
    const GenreLikes *genre_b = (const GenreLikes *)b;

    if (genre_b->total_likes != genre_a->total_likes) {
        return genre_b->total_likes - genre_a->total_likes;
    }
    return strcmp(genre_a->genre, genre_b->genre);
}

void query_q3(STORAGE *storage, int min_age, int max_age, bool use_separator_equals, const char *output_file) {
    if (!storage || !output_file || min_age < 0 || max_age < 0 || min_age > max_age) {
        fprintf(stderr, ANSI_COLOR_RED "Erro: Parâmetros inválidos fornecidos para Query 3.\n");
        return;
    }

    GHashTable *users_table = getUsers(storage)->utable;
    GHashTable *musics_table = getMusics(storage)->mtable;

    if (!users_table || !musics_table) {
        fprintf(stderr, ANSI_COLOR_RED "Erro: Tabelas de usuários ou músicas não estão disponíveis.\n");
        return;
    }

    int current_year = 2024;
    int birth_year_max = current_year - min_age;
    int birth_year_min = current_year - max_age;

    // fprintf(stderr, "Debug: Min birth year = %d, Max birth year = %d\n", birth_year_min, birth_year_max);

    GHashTable *global_genre_likes = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

      GHashTableIter user_iter;
    gpointer user_key, user_value;
    g_hash_table_iter_init(&user_iter, users_table);

    while (g_hash_table_iter_next(&user_iter, &user_key, &user_value)) {
        User *user = (User *)user_value;

        Date *birth_date = get_user_birth_date(user);
        if (!birth_date || birth_date->year < birth_year_min || birth_date->year > birth_year_max) {
            // fprintf(stderr, ANSI_COLOR_RED "Debug: Usuário '%s' fora da faixa etária ou sem data de nascimento.\n" ANSI_COLOR_RESET, get_user_username(user));
            continue;
        }

        GArray *liked_songs = get_user_liked_songs_id(user);
        if (!liked_songs || liked_songs->len == 0) {
            //fprintf(stderr, "Debug: UTilizador '%s' sem músicas curtidas.\n", get_user_username(user));
            continue;
        }

        for (guint i = 0; i < liked_songs->len; i++) {
            char *song_id = g_array_index(liked_songs, char *, i);
            // printf("Debug: Processando música ID '%s' para usuário '%s'.\n", song_id, get_user_username(user));

            Music *music = g_hash_table_lookup(musics_table, song_id);
            if (!music) {
                fprintf(stderr, ANSI_COLOR_RED "Erro: Música '%s' não encontrada para o utilizador '%s'.\n" ANSI_COLOR_RESET, song_id, get_user_username(user));
                continue;
            }

            char *genre = g_strdup(get_music_genre(music));
            if (!genre || strlen(genre) == 0) {
                fprintf(stderr, ANSI_COLOR_RED "Erro: Música '%s' não possui género válido.\n" ANSI_COLOR_RESET, song_id);
                g_free(genre);
                continue;
            }

            g_strstrip(genre);

            int *global_like_count = g_hash_table_lookup(global_genre_likes, genre);
            if (!global_like_count) {
                global_like_count = g_new(int, 1);
                *global_like_count = 0;
                g_hash_table_insert(global_genre_likes, g_strdup(genre), global_like_count);
            }
            (*global_like_count)++;

            // printf("Debug: Incrementando contagem global para gênero '%s': %d\n", genre, *global_like_count);
            g_free(genre);
        }
    }

    if (g_hash_table_size(global_genre_likes) == 0) {
        fprintf(stderr, ANSI_COLOR_RED "Erro: Nenhum género contabilizado. 'global_genre_likes' está vazio.\n");
        g_hash_table_destroy(global_genre_likes);
        return;
    }

    GArray *sorted_genres = g_array_new(FALSE, FALSE, sizeof(GenreLikes));
    GHashTableIter genre_iter;
    gpointer genre_key, genre_value;

    g_hash_table_iter_init(&genre_iter, global_genre_likes);
    while (g_hash_table_iter_next(&genre_iter, &genre_key, &genre_value)) {
        GenreLikes genre_entry = {(char *)genre_key, *(int *)genre_value};
        g_array_append_val(sorted_genres, genre_entry);
    }

    g_array_sort(sorted_genres, compare_genres);

    char separator = use_separator_equals ? '=' : ';';

    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Erro ao abrir arquivo de saída para Query 3");
        g_array_free(sorted_genres, TRUE);
        g_hash_table_destroy(global_genre_likes);
        return;
    }

    for (guint i = 0; i < sorted_genres->len; i++) {
        GenreLikes *entry = &g_array_index(sorted_genres, GenreLikes, i);
        fprintf(output, "%s%c%d\n", entry->genre, separator, entry->total_likes);
    }

    fclose(output);
/*
    printf("\nResultado Final por Gênero:\n");
    for (guint i = 0; i < sorted_genres->len; i++) {
        GenreLikes *entry = &g_array_index(sorted_genres, GenreLikes, i);
        printf("Gênero: %s, Total Likes: %d\n", entry->genre, entry->total_likes);
    }
*/
    g_array_free(sorted_genres, TRUE);
    g_hash_table_destroy(global_genre_likes);
}

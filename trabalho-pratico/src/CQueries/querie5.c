/*#include "../../include/Queries/querie5.h"
#include "../../include/recomendador.h"
#include "../../include/management.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024


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



void execute_querie5(const char *outputDir, STORAGE *storage, const char *user_id, int num_recommendations, bool use_separator_equals) {
    if (!storage) {
        fprintf(stderr, "Erro: Estrutura STORAGE inválida.\n");
        return;
    }

    GHashTable *users = getUsers(storage)->utable;
    GHashTable *history = getHistory(storage)->htable;
    GHashTable *musics = getMusics(storage)->mtable;

    if (!users || !history || !musics) {
        fprintf(stderr, "Erro: Tabelas inválidas na execução da Query 5.\n");
        return;
    }

    if (!g_hash_table_contains(users, user_id)) {
        fprintf(stderr, "Erro: Utilizador com ID '%s' não encontrado.\n", user_id);
        return;
    }

    GHashTable *genres = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    GHashTableIter music_iter;
    gpointer key, value;
    g_hash_table_iter_init(&music_iter, musics);

    int genre_index = 0;
    while (g_hash_table_iter_next(&music_iter, &key, &value)) {
        Music *music = (Music *)value;
        const char *genre = get_music_genre(music);
        if (genre && !g_hash_table_contains(genres, genre)) {
            g_hash_table_insert(genres, g_strdup(genre), GINT_TO_POINTER(genre_index++));
        }
    }

    int num_genres = g_hash_table_size(genres);
    int num_users = g_hash_table_size(users);

    int **classification_matrix = calloc(num_users, sizeof(int *));
    for (int i = 0; i < num_users; i++) {
        classification_matrix[i] = calloc(num_genres, sizeof(int));
    }

    const char **user_ids = calloc(num_users, sizeof(char *));
    const char **genre_names = calloc(num_genres, sizeof(char *));

    GHashTableIter genre_iter;
    gpointer genre_key;
    int idx = 0;
    g_hash_table_iter_init(&genre_iter, genres);
    while (g_hash_table_iter_next(&genre_iter, &genre_key, NULL)) {
        genre_names[idx++] = (const char *)genre_key;
    }

    populate_classification_matrix(users, history, musics, classification_matrix, user_ids, genre_names);

    char **recommendations = recomendaUtilizadores(user_id, classification_matrix, user_ids, genre_names, num_users, num_genres, num_recommendations);

    char output_file[MAX_BUFFER_SIZE];
    snprintf(output_file, MAX_BUFFER_SIZE, "%scommand5_output.txt", outputDir);
    FILE *output = fopen(output_file, "w");
    if (!output) {
        perror("Erro ao abrir o ficheiro de saída");
        g_hash_table_destroy(genres);
        for (int i = 0; i < num_users; i++) free(classification_matrix[i]);
        free(classification_matrix);
        free(user_ids);
        free(genre_names);
        return;
    }

    char separator = use_separator_equals ? '=' : ';';

    for (int i = 0; recommendations[i] != NULL; i++) {
        fprintf(output, "%s%c\n", recommendations[i], separator);
        free(recommendations[i]);
    }
    free(recommendations);

    fclose(output);

    g_hash_table_destroy(genres);
    for (int i = 0; i < num_users; i++) free(classification_matrix[i]);
    free(classification_matrix);
    free(user_ids);
    free(genre_names);
}
*/
/*#include "../../include/Queries/querie4.h"
#include "../../include/utils.h" // Incluído para usar as funções auxiliares
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LENGTH 4096

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

struct ArtistTime {
    char *artist_id;
    int total_time;
};



gint compare_artist_time(gconstpointer a, gconstpointer b) {
    if (!a || !b) return 0;

    const ArtistTime *artistA = (const ArtistTime *)a;
    const ArtistTime *artistB = (const ArtistTime *)b;

    // Ordem decrescente por total_time; desempate pelo menor artist_id
    if (artistB->total_time != artistA->total_time) {
        return artistB->total_time - artistA->total_time;
    }
    return strcmp(artistA->artist_id, artistB->artist_id);
}


void execute_querie4(const char *outputDir, STORAGE *storage, const char *begin_date, const char *end_date, bool use_separator_equals) {
    if (!storage) {
        fprintf(stderr, "Erro: Estrutura STORAGE inválida.\n");
        return;
    }

    HistoryTable *historyTable = getHistory(storage);
    ArtistTable *artistTable = getArtists(storage);

    if (!historyTable || !historyTable->htable || !artistTable || !artistTable->atable) {
        fprintf(stderr, "Erro: Tabelas inválidas no STORAGE.\n");
        return;
    }

    if ((begin_date && !is_valid_date(begin_date)) || (end_date && !is_valid_date(end_date))) {
        fprintf(stderr, "Erro: Datas inválidas fornecidas para Query 4: %s a %s.\n",
                begin_date ? begin_date : "NULL",
                end_date ? end_date : "NULL");
        return;
    }

    // Inicialização de tabelas auxiliares
    GHashTable *weekly_top10_individual = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)g_list_free_full);
    GHashTable *weekly_top10_group = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)g_list_free_full);
    GHashTable *artist_appearances = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, historyTable->htable);

    int entry_count = 0;

    // Processa entradas no histórico
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;
        const char *timestamp = get_history_timestamp(entry);

        // Filtra pelo intervalo de datas fornecido
        if (begin_date && end_date) {
            char date_only[11];
            strncpy(date_only, timestamp, 10);
            date_only[10] = '\0';
            if (strcmp(date_only, begin_date) < 0 || strcmp(date_only, end_date) > 0) {
                continue;
            }
        }

        char *week = get_week_from_timestamp(timestamp);
        if (!week) continue;

        // Recupera o artista associado à música
        const char *artist_id = get_artist_from_music(get_history_music_id(entry), getMusics(storage), artistTable);
        const char *duration_str = get_history_duration(entry);
        int played_time = duration_str ? atoi(duration_str) : 0;

        if (artist_id) {
            const char *artist_type = get_artist_type_from_id(artist_id, artistTable);
            GHashTable *weekly_top10 = (strcmp(artist_type, "individual") == 0) ? weekly_top10_individual : weekly_top10_group;

            GList *list = g_hash_table_lookup(weekly_top10, week);

            // Validações adicionais para garantir listas válidas
            if (list == NULL) {
                list = NULL;
            }

            ArtistTime *artist_time = NULL;
            for (GList *node = list; node; node = node->next) {
                ArtistTime *at = (ArtistTime *)node->data;
                if (strcmp(at->artist_id, artist_id) == 0) {
                    artist_time = at;
                    break;
                }
            }

            if (artist_time) {
                artist_time->total_time += played_time;
            } else {
                artist_time = g_new(ArtistTime, 1);
                artist_time->artist_id = g_strdup(artist_id);
                artist_time->total_time = played_time;
                list = g_list_insert_sorted(list, artist_time, compare_artist_time);
            }

            g_hash_table_replace(weekly_top10, g_strdup(week), list);
        }

        g_free(week);
        entry_count++;

        if (entry_count % 1000 == 0) {
            fprintf(stderr, "Debug: Processadas %d entradas do histórico...\n", entry_count);
        }
    }

    fprintf(stderr, "Debug: Histórico filtrado. Total de entradas relevantes: %d\n", entry_count);

    // Processa o top 10 semanal para indivíduos e grupos
    process_weekly_top10(weekly_top10_individual, artist_appearances, "individual");
    process_weekly_top10(weekly_top10_group, artist_appearances, "group");

    // Determina o artista mais frequente
    gpointer top_artist = NULL;
    int max_count = 0;
    g_hash_table_iter_init(&iter, artist_appearances);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        int count = GPOINTER_TO_INT(value);
        if (count > max_count || (count == max_count && strcmp((char *)key, (char *)top_artist) < 0)) {
            top_artist = key;
            max_count = count;
        }
    }

    // Gera o output
    char output_path[MAX_PATH_LENGTH];
    snprintf(output_path, MAX_PATH_LENGTH, "%s/command4_output.txt", outputDir);

    FILE *output_file = fopen(output_path, "w");
    if (!output_file) {
        perror("Erro ao criar o arquivo de saída");
        fprintf(stderr, "Caminho do arquivo: %s\n", output_path);
        return;
    }

    char separator = use_separator_equals ? '=' : ';';

    if (top_artist) {
        const char *artist_name = get_artist_name_from_id((char *)top_artist, artistTable);
        const char *artist_type = get_artist_type_from_id((char *)top_artist, artistTable);
        if (artist_name && artist_type) {
            fprintf(output_file, "%s%c%s%c%d\n",
                    artist_name,
                    separator,
                    artist_type,
                    separator,
                    max_count);
        } else {
            fprintf(output_file, "\n");
        }
    } else {
        fprintf(output_file, "\n");
    }

    fclose(output_file);

    // Liberação de memória
    g_hash_table_destroy(weekly_top10_individual);
    g_hash_table_destroy(weekly_top10_group);
    g_hash_table_destroy(artist_appearances);
}
*/
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/management.h"
#include "../../include/parser.h"
#include "../../include/utils.h"
#include "../../include/structs.h"

#define MAX_LINE_LENGTH 16384

struct HistoryTable {
    GHashTable *htable;
};

struct History {
    char *id;         
    char *user_id;     
    char *music_id;    
    char *timestamp;   
    char *duration;      
    char *platform;    
};

HistoryTable* create_history_table() {
    HistoryTable* historyTable = g_new(HistoryTable, 1);
    historyTable->htable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free_history_wrapper);
    return historyTable;
}

void free_history_table(HistoryTable *historyTable) {
    if (historyTable) {
        g_hash_table_destroy(historyTable->htable);
        g_free(historyTable);
    }
}

void add_history_to_table(HistoryTable* historyTable, History* history) {
    if (history_table_contains(historyTable, get_history_id(history))) {
        fprintf(stderr, "Error: Duplicate history ID detected: %s\n", get_history_id(history));
        return;
    }
    char *history_id_copy = g_strdup(get_history_id(history));
    g_hash_table_insert(historyTable->htable, history_id_copy, history);
}

void free_history_wrapper(void *history_ptr) {
    free_history(history_ptr);
}

bool history_table_contains(HistoryTable *historyTable, const char *history_id) {
    return g_hash_table_contains(historyTable->htable, history_id);
}



void populate_history_table(HistoryTable* historyTable,  GHashTable *history_table) {
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, history_table);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        add_history_to_table(historyTable, (History*) value);
    }
}

/*void print_history_table(GHashTable *history_table) {
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, history_table);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *history = (History *)value;
        printf("History ID: %s\n", get_history_id(history));
        printf("User ID: %s\n", get_history_user_id(history));
        printf("Music ID: %s\n", get_history_music_id(history));
        printf("Timestamp: %s\n", get_history_timestamp(history));
        printf("Duration: %s\n", get_history_duration(history));
        printf("Platform: %s\n", get_history_platform(history));
        printf("\n");
    }
}

*/

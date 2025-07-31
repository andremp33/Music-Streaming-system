#include "../../include/management.h"
#include "../../include/structs.h"
#include "../../include/utils.h"
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct History {
    char *id;         
    char *user_id;     
    char *music_id;    
    char *timestamp;   
    char *duration;      
    char *platform;    
};

History* create_history_from_line(const char *line, FILE *error_registry, HistoryTable *historyTable, UserTable *userTable, MusicTable *musicTable) {
    int has_error = 0;
    History *history = g_malloc0(sizeof(History));
    if (!history) {
        fprintf(stderr, "Erro: Falha na alocação de memória para history\n");
        return NULL;
    }

    gchar **tokens = g_strsplit(line, ";", -1);
    int num_tokens = g_strv_length(tokens);

    if (num_tokens < 6) {
        fprintf(stderr, "Erro: Linha incompleta detectada\n");
        has_error = 1;
    } else {
        set_history_id(history, tokens[0]);
        set_history_user_id(history, tokens[1], userTable, &has_error);
        set_history_music_id(history, tokens[2], musicTable, &has_error);
        set_history_timestamp(history, tokens[3]);
        set_history_duration(history, tokens[4], &has_error);
        set_history_platform(history, tokens[5], &has_error);
    }

    if (has_error) {
        register_error_line(error_registry, line);
        free_history(history);
        history = NULL;
    }
    g_strfreev(tokens);
    return history;
}

void set_history_id(History *history, const char *id) {
    if (history->id) g_free(history->id);
    history->id = g_strdup(id);

    int len = strlen(history->id);
    if (len > 1 && history->id[0] == '"' && history->id[len - 1] == '"') {
        history->id[len - 1] = '\0';
        memmove(history->id, history->id + 1, len - 1);
    }
}

void set_history_user_id(History *history, const char *user_id, UserTable *userTable, int *has_error) {
    if (history->user_id) g_free(history->user_id);
    history->user_id = g_strdup(user_id);

    int len = strlen(history->user_id);
    if (len > 1 && history->user_id[0] == '"' && history->user_id[len - 1] == '"') {
        history->user_id[len - 1] = '\0';
        memmove(history->user_id, history->user_id + 1, len - 1);
    }

    if (!user_table_contains(userTable, history->user_id)) {
        *has_error = 1;
    }
}

void set_history_music_id(History *history, const char *music_id, MusicTable *musicTable, int *has_error) {
    if (history->music_id) g_free(history->music_id);
    history->music_id = g_strdup(music_id);

    int len = strlen(history->music_id);
    if (len > 1 && history->music_id[0] == '"' && history->music_id[len - 1] == '"') {
        history->music_id[len - 1] = '\0';
        memmove(history->music_id, history->music_id + 1, len - 1);
    }

    if (!music_table_contains(musicTable, history->music_id)) {
        *has_error = 1;
    }
}

void set_history_timestamp(History *history, const char *timestamp) {
    if (history->timestamp) g_free(history->timestamp);

    char *cleaned_timestamp = g_strdup(timestamp);
    int len = strlen(cleaned_timestamp);
    if (len > 1 && cleaned_timestamp[0] == '"' && cleaned_timestamp[len - 1] == '"') {
        cleaned_timestamp[len - 1] = '\0';
        memmove(cleaned_timestamp, cleaned_timestamp + 1, len - 1);
    }

    history->timestamp = cleaned_timestamp;
}

void set_history_duration(History *history, const char *duration_str, int *has_error) {
    char *cleaned_duration = g_strdup(duration_str);

    int len = strlen(cleaned_duration);
    if (len > 1 && cleaned_duration[0] == '"' && cleaned_duration[len - 1] == '"') {
        cleaned_duration[len - 1] = '\0';
        memmove(cleaned_duration, cleaned_duration + 1, len - 1);
    }

    if (strlen(cleaned_duration) != 8 || cleaned_duration[2] != ':' || cleaned_duration[5] != ':') {
        *has_error = 1;
        g_free(cleaned_duration);
        history->duration = NULL;
        return;
    }

    int hours, minutes, seconds;
    if (sscanf(cleaned_duration, "%02d:%02d:%02d", &hours, &minutes, &seconds) != 3 ||
        hours < 0 || hours > 99 || minutes < 0 || minutes >= 60 || seconds < 0 || seconds >= 60) {
        *has_error = 1;
        g_free(cleaned_duration);
        history->duration = NULL;
        return;
    }

    if (history->duration) g_free(history->duration);
    history->duration = cleaned_duration;
}

void set_history_platform(History *history, const char *platform, int *has_error) {
    if (history->platform) g_free(history->platform);

    char *cleaned_platform = g_strdup(platform);

    int len = strlen(cleaned_platform);
    if (len > 1 && cleaned_platform[0] == '"' && cleaned_platform[len - 1] == '"') {
        cleaned_platform[len - 1] = '\0';
        memmove(cleaned_platform, cleaned_platform + 1, len - 1);
    }

    gchar *lower_platform = g_ascii_strdown(cleaned_platform, -1);
    if (g_strcmp0(lower_platform, "mobile") == 0 || g_strcmp0(lower_platform, "desktop") == 0) {
        history->platform = g_strdup(lower_platform);
    } else {
        *has_error = 1;
        history->platform = NULL;
    }

    g_free(lower_platform);
    g_free(cleaned_platform);
}

const char* get_history_id(History *history) {
    return history ? history->id : NULL;
}

const char* get_history_user_id(History *history) {
    return history ? history->user_id : NULL;
}

const char* get_history_music_id(History *history) {
    return history ? history->music_id : NULL;
}

const char* get_history_timestamp(History *history) {
    return history ? history->timestamp : NULL;
}

const char* get_history_duration(History *history) {
    return history ? history->duration : NULL;
}

const char* get_history_platform(History *history) {
    return history ? history->platform : NULL;
}

void free_history(void *history_ptr) {
    History *history = (History *)history_ptr;
    if (history == NULL) return;

    if (history->id) g_free(history->id);
    if (history->user_id) g_free(history->user_id);
    if (history->music_id) g_free(history->music_id);
    if (history->timestamp) g_free(history->timestamp);
    if (history->platform) g_free(history->platform);
    if (history->duration) g_free(history->duration);   

    g_free(history);
}


GHashTable* filter_history_by_user_and_year(GHashTable *history_table, const char *user_id, int year) {
    GHashTable *filtered_history = g_hash_table_new(g_str_hash, g_str_equal);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, history_table);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        History *entry = (History *)value;

        if (strcmp(get_history_user_id(entry), user_id) == 0) {
            const char *timestamp = get_history_timestamp(entry);
            int entry_year;
            sscanf(timestamp, "%4d", &entry_year);

            if (entry_year == year) {
                g_hash_table_insert(filtered_history, key, value);
            }
        }
    }

    return filtered_history;
}

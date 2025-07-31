#include "../../include/structs.h"
#include "../../include/management.h"
#include "../../include/utils.h"
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define USERS_DATASET_NAME "/home/andre/dataset/users.csv"
#define USERS_ERRORS_DATASET_NAME "/home/andre/dataset/users_errors.csv"

struct User {
    char *username;
    char *email;
    char *first_name;
    char *last_name;
    struct Date *birth_date;
    char *country;
    char *subscription_type;
    GArray *liked_songs_id;
};

struct UserTable {
    GHashTable *utable;
};

struct Date {
    int year;
    int month;
    int day;
};

void debug_print_user_liked_songs(UserTable *userTable) {
    printf("Debug: Lista de músicas curtidas por usuários:\n");
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, userTable->utable);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        User *user = (User *)value;
        printf("User ID: %s\n", get_user_username(user));
        GArray *liked_songs = get_user_liked_songs_id(user);

        if (liked_songs && liked_songs->len > 0) {
            printf("Liked Songs: ");
            for (guint i = 0; i < liked_songs->len; i++) {
                printf("%s ", g_array_index(liked_songs, char *, i));
            }
            printf("\n");
        } else {
            printf("No liked songs\n");
        }
    }
}


User* create_user_from_line(const char *line, FILE *error_registry, UserTable *userTable, MusicTable *musicTable) {
    int has_error = 0;

    User *user = g_malloc0(sizeof(User));
    if (!user) {
        fprintf(stderr, "Error: Memory allocation failed for user\n");
        return NULL;
    }

    gchar **tokens = g_strsplit(line, ";", -1);
    int num_tokens = g_strv_length(tokens);

    if (num_tokens < 8) {
        fprintf(stderr, "Error: Incomplete line detected\n");
        register_error_line(error_registry, line);
        free_user(user);
        g_strfreev(tokens);
        return NULL;
    }

    set_user_username(user, tokens[0]);
    set_user_email(user, tokens[1], &has_error);
    set_user_first_name(user, tokens[2]);
    set_user_last_name(user, tokens[3]);
    set_user_birth_date(user, tokens[4], &has_error);
    set_user_country(user, tokens[5]);
    set_user_subscription_type(user, tokens[6], &has_error);
    set_user_liked_songs_id(user, tokens[7], musicTable, error_registry, &has_error);

    if (has_error) {
        register_error_line(error_registry, line);
        free_user(user);
        user = NULL;
    } 
    g_strfreev(tokens);
    return user;
}

void set_user_username(User *user, const char *username) {
    if (user->username) g_free(user->username);
    user->username = g_strdup(username);

    int len = strlen(user->username);
    if (len > 1 && user->username[0] == '"' && user->username[len - 1] == '"') {
        user->username[len - 1] = '\0';
        memmove(user->username, user->username + 1, len - 1);
    }
}

void set_user_email(User *user, const char *email, int *has_error) {
    if (user->email) g_free(user->email);
    user->email = g_strdup(email);

    int len = strlen(user->email);
    if (len > 1 && user->email[0] == '"' && user->email[len - 1] == '"') {
        user->email[len - 1] = '\0';
        memmove(user->email, user->email + 1, len - 1);
    }

    const char *at = strchr(user->email, '@');
    if (!at || at == user->email || at[1] == '\0' || at[1] == '.') {
        *has_error = 1;
        return;
    }

    const char *domain = at + 1;
    const char *dot = strrchr(domain, '.');
    if (!dot || dot == domain || strlen(dot + 1) < 2 ||  strlen(dot + 1) > 3) {
        *has_error = 1;
        return;
    }

    for (const char *p = user->email; p < at; p++) {
        if (!((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9'))) {
            *has_error = 1;
            return;
        }
    }

    for (const char *p = domain; *p; p++) {
        if (!(*p >= 'a' && *p <= 'z') && *p != '.') {
            *has_error = 1;
            return;
        }
    }
}

void set_user_first_name(User *user, const char *first_name) {
    if (user->first_name) g_free(user->first_name);
    user->first_name = g_strdup(first_name);

    int len = strlen(user->first_name);
    if (len > 1 && user->first_name[0] == '"' && user->first_name[len - 1] == '"') {
        user->first_name[len - 1] = '\0';
        memmove(user->first_name, user->first_name + 1, len - 1);
    }
}

void set_user_last_name(User *user, const char *last_name) {
    if (user->last_name) g_free(user->last_name);
    user->last_name = g_strdup(last_name);

    int len = strlen(user->last_name);
    if (len > 1 && user->last_name[0] == '"' && user->last_name[len - 1] == '"') {
        user->last_name[len - 1] = '\0';
        memmove(user->last_name, user->last_name + 1, len - 1);
    }
}
void set_user_birth_date(User *user, const char *birth_date, int *has_error) {
    struct Date *date = g_malloc0(sizeof(struct Date));
    char *cleaned_birth_date = g_strdup(birth_date);
    int len = strlen(cleaned_birth_date);

    if (len > 1 && cleaned_birth_date[0] == '"' && cleaned_birth_date[len - 1] == '"') {
        cleaned_birth_date[len - 1] = '\0';
        memmove(cleaned_birth_date, cleaned_birth_date + 1, len - 1);
    }

    if (strlen(cleaned_birth_date) != 10 || cleaned_birth_date[4] != '/' || cleaned_birth_date[7] != '/') {
        *has_error = 1;
        g_free(date);
        g_free(cleaned_birth_date);
        return;
    }

    for (int i = 0; i < 10; i++) {
        if ((i != 4 && i != 7 && !isdigit(cleaned_birth_date[i]))) {
            *has_error = 1;
            g_free(date);
            g_free(cleaned_birth_date);
            return;
        }
    }

    if (sscanf(cleaned_birth_date, "%d/%d/%d", &date->year, &date->month, &date->day) != 3 ||
        date->month < 1 || date->month > 12 || date->day < 1 || date->day > 31) {
        *has_error = 1;
        //printf("Erro: data inválida %s\n", cleaned_birth_date);
        g_free(date);
        g_free(cleaned_birth_date);
        return;
    }

    int current_year = 2024;
    int current_month = 9;
    int current_day = 9;

    if (date->year > current_year ||
        (date->year == current_year && date->month > current_month) ||
        (date->year == current_year && date->month == current_month && date->day > current_day)) {
        *has_error = 1;
        //printf("Erro: data futura %s\n", cleaned_birth_date);
        g_free(date);
        g_free(cleaned_birth_date);
        return;
    }

    user->birth_date = date;
    g_free(cleaned_birth_date);
    //printf("Data atribuída com sucesso: %d/%02d/%02d\n", date->year, date->month, date->day);
    //printf("Valor final de has_error em set_user_birth_date: %d\n", *has_error);
}





void set_user_country(User *user, const char *country) {
    if (user->country) g_free(user->country);
    user->country = g_strdup(country);

    int len = strlen(user->country);
    if (len > 1 && user->country[0] == '"' && user->country[len - 1] == '"') {
        user->country[len - 1] = '\0';
        memmove(user->country, user->country + 1, len - 1);
    }
}

void set_user_subscription_type(User *user, const char *subscription_type, int *has_error) {
    if (user->subscription_type) g_free(user->subscription_type);
    user->subscription_type = g_strdup(subscription_type);

    int len = strlen(user->subscription_type);
    if (len > 1 && user->subscription_type[0] == '"' && user->subscription_type[len - 1] == '"') {
        user->subscription_type[len - 1] = '\0';
        memmove(user->subscription_type, user->subscription_type + 1, len - 1);
    }

    //printf("antesdaComp: %s\n", user->subscription_type);
    if (strcmp(user->subscription_type, "normal") != 0 && strcmp(user->subscription_type, "premium") != 0) {
        *has_error = 1;
    }
}

void set_user_liked_songs_id(User *user, const char *liked_songs_id, MusicTable *musicTable, FILE *error_registry, int *has_error) {
    if (liked_songs_id == NULL || strlen(liked_songs_id) == 0) {
        register_error_line(error_registry, "artist_id_str is empty or NULL");
        *has_error = 1;
        return;
    }

    if (user->liked_songs_id) {
        for (guint i = 0; i < user->liked_songs_id->len; i++) {
            g_free(g_array_index(user->liked_songs_id, char*, i));
        }
        g_array_free(user->liked_songs_id, TRUE);
    }
    user->liked_songs_id = g_array_new(FALSE, FALSE, sizeof(char*));

    int len = strlen(liked_songs_id);
    gchar *trimmed_songs_id_str = NULL;
    if (len > 1 && liked_songs_id[0] == '"' && liked_songs_id[len - 1] == '"') {
        trimmed_songs_id_str = g_strndup(liked_songs_id + 1, len - 2);
        liked_songs_id = trimmed_songs_id_str;
    }

    len = strlen(liked_songs_id);
    if (len >= 2 && liked_songs_id[0] == '[' && liked_songs_id[len - 1] == ']') {
        gchar *clean_ids2 = g_strndup(liked_songs_id + 1, len - 2);
        gchar **ids2 = g_strsplit(clean_ids2, ",", -1);

        for (int i = 0; ids2[i] != NULL; i++) {
            gchar *id = g_strstrip(ids2[i]);

            if (id[0] == '\'' && id[strlen(id) - 1] == '\'') {
                id[strlen(id) - 1] = '\0';
                memmove(id, id + 1, strlen(id));
            }

            if (strlen(id) > 0 && music_table_contains(musicTable, id)) {
                gchar *id_copy = g_strdup(id);
                g_array_append_val(user->liked_songs_id, id_copy);
            } else {
                //printf("Erro: music_id '%s' não encontrado na tabela de users.\n", id);
                *has_error = 1;
                //register_error_line(error_registry, id);
            }
        }
        g_free(clean_ids2);
        g_strfreev(ids2);
    } else {
        *has_error = 1;
        //register_error_line(error_registry, "Invalid artist_id_str format; expected list in brackets");
    }

    g_free(trimmed_songs_id_str);
}





const char* get_user_username(User *user) { 
    return user ? user->username : NULL;
}

const char* get_user_email(User *user) { 
    return user ? user->email : NULL;
}

const char* get_user_first_name(User *user) { 
    return user ? user->first_name : NULL;
}

const char* get_user_last_name(User *user) { 
    return user ? user->last_name : NULL;
}

Date* get_user_birth_date(User *user) { 
    return user ? user->birth_date : NULL;
}

const char* get_user_country(User *user) { 
    return user ? user->country : NULL;
}

const char* get_user_subscription_type(User *user) { 
    return user ? user->subscription_type : NULL;
}

GArray* get_user_liked_songs_id(User *user) { 
    return user ? user->liked_songs_id : NULL;
}


void free_user(void *user_ptr) {
    User *user = (User *)user_ptr;
    if (user == NULL) return;

    if (user->username) g_free(user->username);
    if (user->email) g_free(user->email);
    if (user->first_name) g_free(user->first_name);
    if (user->last_name) g_free(user->last_name);
    if (user->birth_date) g_free(user->birth_date);
    if (user->country) g_free(user->country);
    if (user->subscription_type) g_free(user->subscription_type);

    if (user->liked_songs_id) {
        for (guint i = 0; i < user->liked_songs_id->len; i++) {
            char *song_id = g_array_index(user->liked_songs_id, char*, i);
            if (song_id) g_free(song_id);
        }
        g_array_free(user->liked_songs_id, TRUE);
    }

    g_free(user);
}
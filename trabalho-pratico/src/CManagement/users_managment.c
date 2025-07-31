#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/management.h"
#include "../../include/parser.h"
#include "../../include/utils.h"
#include "../../include/structs.h"

#define MAX_LINE_LENGTH 16384

struct UserTable {
    GHashTable *utable;
};

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

UserTable* create_user_table() {
    UserTable* userTable = g_new(UserTable, 1);
    userTable->utable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)free_user);
    return userTable;
}



void add_user_to_table(UserTable* userTable, User* user) {
    if (user_table_contains(userTable, get_user_username(user))) {
        fprintf(stderr, "Error: Duplicate user ID: %s\n", get_user_username(user));
        return;
    }
    char *user_id_copy = g_strdup(get_user_username(user));
    g_hash_table_insert(userTable->utable, user_id_copy, user);
}

void free_user_table(UserTable* userTable) {
    if (userTable) {
        g_hash_table_destroy(userTable->utable);
        g_free(userTable);
    }
}

bool user_table_contains(UserTable *userTable, const char *user_id) {
    return g_hash_table_contains(userTable->utable, user_id);
}

void print_user_table(UserTable *userTable) {
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, userTable->utable);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        User *user = (User *)value;
        printf("User ID: %s\n", get_user_username(user));
        printf("Email: %s\n", get_user_email(user));
        printf("First Name: %s\n", get_user_first_name(user));
        printf("Last Name: %s\n", get_user_last_name(user));

        char *birth_date_str = date_to_string(get_user_birth_date(user));
        printf("Birth Date: %s\n", birth_date_str ? birth_date_str : "N/A");
        g_free(birth_date_str);

        printf("Country: %s\n", get_user_country(user));
        printf("Subscription Type: %s\n", get_user_subscription_type(user));

        printf("Liked Songs: ");
        for (guint i = 0; i < user->liked_songs_id->len; i++) {
            printf("%s ", g_array_index(user->liked_songs_id, char*, i));
        }
        printf("\n\n");
    }
}

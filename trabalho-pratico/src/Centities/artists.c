#include "../../include/structs.h"
#include "../../include/management.h"
#include "../../include/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

#define DATASET_NAME "/home/andre/dataset/artists.csv"
#define ERRORS_DATASET_NAME "/home/andre/dataset/artists_errors.csv"

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

Artist* create_artist_from_line(const char *line, FILE *error_registry, ArtistTable *artistTable) {
    int has_error = 0;

    Artist *artist = g_malloc0(sizeof(Artist));
    if (!artist) {
        fprintf(stderr, "Error: Memory allocation failed for artist\n");
        return NULL;
    }

    gchar **tokens = g_strsplit(line, ";", -1);
    int num_tokens = g_strv_length(tokens);

    if (num_tokens < 7) {
        register_error_line(error_registry, line);
        free_artist(artist);
        g_strfreev(tokens);
        return NULL;
    }

    set_artist_id(artist, tokens[0]);
    set_artist_name(artist, tokens[1]);
    artist->description = NULL;

    if (tokens[3] != NULL) {
        set_artist_recipe_per_stream(artist, tokens[3]);
    }

    set_artist_country(artist, tokens[5]);
    set_artist_type(artist, tokens[6]);
    set_artist_id_constituent(artist, tokens[4], tokens[6], error_registry, &has_error);
    artist->total_revenue = 0.0; 
    artist->num_albums = 0;
    artist->participation_revenue = 0.0;
    if (has_error) {
        register_error_line(error_registry, line);
        free_artist(artist);
        artist = NULL;
    }
    g_strfreev(tokens);
    return artist;
}

void set_artist_id(Artist *artist, const char *id) {
    if (artist->id) g_free(artist->id);
    artist->id = g_strdup(id);
    
    int len = strlen(artist->id);
    if (len > 1 && artist->id[0] == '"' && artist->id[len - 1] == '"') {
        artist->id[len - 1] = '\0';
        memmove(artist->id, artist->id + 1, len - 1);
    }
}

void set_artist_name(Artist *artist, const char *name) {
    if (artist->name) g_free(artist->name);
    artist->name = g_strdup(name);

    int len = strlen(artist->name);
    if (len > 1 && artist->name[0] == '"' && artist->name[len - 1] == '"') {
        artist->name[len - 1] = '\0';
        memmove(artist->name, artist->name + 1, len - 1);
    }
}

void set_artist_country(Artist *artist, const char *country) {
    if (artist->country) g_free(artist->country);
    artist->country = g_strdup(country);

    int len = strlen(artist->country);
    if (len > 1 && artist->country[0] == '"' && artist->country[len - 1] == '"') {
        artist->country[len - 1] = '\0';
        memmove(artist->country, artist->country + 1, len - 1);
    }
}

void set_artist_recipe_per_stream(Artist *artist, const char *recipe_str) {
    if (artist->recipe_str) g_free(artist->recipe_str);
    artist->recipe_str = g_strdup(recipe_str);

    int len = strlen(artist->recipe_str);
    if (len > 1 && artist->recipe_str[0] == '"' && artist->recipe_str[len - 1] == '"') {
        artist->recipe_str[len - 1] = '\0';
        memmove(artist->recipe_str, artist->recipe_str + 1, len - 1);
    }
}

void set_artist_type(Artist *artist, const char *type) {
    if (artist->type) g_free(artist->type);
    artist->type = g_strdup(type);

    int len = strlen(artist->type);
    if (len > 1 && artist->type[0] == '"' && artist->type[len - 1] == '"') {
        artist->type[len - 1] = '\0';
        memmove(artist->type, artist->type + 1, len - 1);
    }
}

void set_artist_id_constituent(Artist *artist, const char *id_constituent_str, const char *type, FILE *error_registry, int *has_error) {
    if (id_constituent_str == NULL || strlen(id_constituent_str) == 0) {
        register_error_line(error_registry, "id_constituent_str is empty or NULL");
        *has_error = 1;
        return;
    }

    int type_len = strlen(type);
    gchar *cleaned_type = NULL;
    if (type_len > 1 && type[0] == '"' && type[type_len - 1] == '"') {
        cleaned_type = g_strndup(type + 1, type_len - 2);
        type = cleaned_type;
    }

    if (artist->id_constituent_str) {
        for (guint i = 0; i < artist->id_constituent_str->len; i++) {
            g_free(g_array_index(artist->id_constituent_str, char*, i));
        }
        g_array_free(artist->id_constituent_str, TRUE);
    }
    artist->id_constituent_str = g_array_new(FALSE, FALSE, sizeof(char*));

    int len = strlen(id_constituent_str);
    gchar *trimmed_id = NULL;
    if (len > 1 && id_constituent_str[0] == '"' && id_constituent_str[len - 1] == '"') {
        trimmed_id = g_strndup(id_constituent_str + 1, len - 2);
        id_constituent_str = trimmed_id;
    }

    if (g_strcmp0(type, "individual") == 0) {
        if (g_strcmp0(id_constituent_str, "[]") != 0) {
            *has_error = 1;
        }
        g_free(trimmed_id);
        g_free(cleaned_type);
        return;
    }

    if (g_strcmp0(type, "group") == 0) {
        gchar *clean_ids = g_strndup(id_constituent_str + 1, strlen(id_constituent_str) - 2);
        gchar **ids = g_strsplit(clean_ids, ",", -1);

        for (int i = 0; ids[i] != NULL; i++) {
            gchar *id = g_strstrip(ids[i]);
            if (strlen(id) > 0) {
                gchar *id_copy = g_strdup(id);
                g_array_append_val(artist->id_constituent_str, id_copy);
            } else {
                *has_error = 1;
            }
        }

        g_free(clean_ids);
        g_strfreev(ids);
    } else {
        *has_error = 1;
        register_error_line(error_registry, id_constituent_str);
    }

    g_free(trimmed_id);
    g_free(cleaned_type);
}




const char* get_artist_id(Artist *artist) {
    return artist ? artist->id : NULL;
}

const char* get_artist_name(Artist *artist) {
    return artist ? artist->name : NULL;
}

const char* get_artist_recipe_per_stream(Artist *artist) {
    return artist ? artist->recipe_str : NULL;
}

GArray* get_artist_id_constituent(Artist *artist) {
    return artist ? artist->id_constituent_str : NULL;
}

const char* get_artist_country(Artist *artist) {
    return artist ? artist->country : NULL;
}

const char* get_artist_type(Artist *artist) {
    return artist ? artist->type : NULL;
}

void free_artist(void *artist_ptr) {
    Artist *artist = (Artist *)artist_ptr;
    if (!artist) return;

    if (artist->id) g_free(artist->id);
    if (artist->name) g_free(artist->name);
    if (artist->recipe_str) g_free(artist->recipe_str);
    if (artist->country) g_free(artist->country);
    if (artist->type) g_free(artist->type);

    if (artist->id_constituent_str) {
        for (guint i = 0; i < artist->id_constituent_str->len; i++) {
            char *id = g_array_index(artist->id_constituent_str, char*, i);
            if (id) g_free(id);
        }
        g_array_free(artist->id_constituent_str, TRUE);
    }
     if (artist->group_ids) {
            g_array_free(artist->group_ids, TRUE);
        }
    g_free(artist);
}

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../include/management.h"
#include "../../include/parser.h"
#include "../../include/utils.h"
#include "../../include/structs.h"

#define MAX_LINE_LENGTH 16384

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

ArtistTable* create_artists_table() {
    ArtistTable* artistTable = g_new(ArtistTable, 1);
    artistTable->atable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)free_artist);
    return artistTable;
}


bool parse_groups_and_artists(ArtistTable *artists_table) {
    if (!artists_table || !artists_table->atable) {
        fprintf(stderr, "Erro: Tabela de artistas inválida.\n");
        return false;
    }

    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, artists_table->atable);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Artist *group = (Artist *)value;

        if (strcmp(get_artist_type(group), "group") == 0) {
            GArray *constituents = get_artist_id_constituent(group);

            if (!constituents || constituents->len == 0) {
                fprintf(stderr, "Debug: Grupo %s não possui membros.\n", group->id);
                continue;
            }

            //fprintf(stderr, "Debug: Processando grupo %s com %u membros:\n", group->id, constituents->len);

            for (guint i = 0; i < constituents->len; i++) {
                char *member_id = g_array_index(constituents, char *, i);

                // Remove as aspas ao redor do ID, se existirem
                if (member_id[0] == '\'' && member_id[strlen(member_id) - 1] == '\'') {
                    member_id[strlen(member_id) - 1] = '\0'; // Remove a última aspa
                    member_id++; // Ignora a primeira aspa
                }

                //fprintf(stderr, "  Debug: ID do constituinte: '%s'\n", member_id);

                Artist *member = g_hash_table_lookup(artists_table->atable, member_id);

                if (member) {
                    if (!member->group_ids) {
                        member->group_ids = g_array_new(FALSE, FALSE, sizeof(char *));
                    }

                    g_array_append_val(member->group_ids, group->id);
                    //fprintf(stderr, "  Debug: Adicionado grupo %s ao artista %s\n", group->id, member->id);
                } else {
                    fprintf(stderr, "  Erro: Membro '%s' não encontrado ao processar grupo %s.\n",
                            member_id, group->id);
                }
            }
        }
    }

    return true;
}


void add_artist_to_table(ArtistTable* artistTable, Artist* artist) {
    if (artist_table_contains(artistTable, get_artist_id(artist))) {
        fprintf(stderr, "Error: Duplicate artist ID: %s\n", get_artist_id(artist));
        return;
    }
    char *artist_id_copy = g_strdup(get_artist_id(artist));
    g_hash_table_insert(artistTable->atable, artist_id_copy, artist);
}

void free_artist_table(ArtistTable* artistTable) {
    if (artistTable) {
        g_hash_table_destroy(artistTable->atable);
        g_free(artistTable);
    }
}

bool artist_table_contains(ArtistTable *artistTable, const char *artist_id) {
    return g_hash_table_contains(artistTable->atable, artist_id);
}

void print_artist_table(ArtistTable *artistTable) {
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, artistTable->atable);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Artist *artist = (Artist *)value;
        printf("Artist ID: %s\n", get_artist_id(artist));
        printf("Name: %s\n", get_artist_name(artist));
        printf("Description: %s\n", get_artist_description(artist));
        printf("Country: %s\n", get_artist_country(artist));
        printf("Type: %s\n", get_artist_type(artist));

        printf("Constituent IDs: ");
        for (guint i = 0; i < artist->id_constituent_str->len; i++) {
            printf("%s ", g_array_index(artist->id_constituent_str, char*, i));
        }
        printf("\n\n");
    }
}

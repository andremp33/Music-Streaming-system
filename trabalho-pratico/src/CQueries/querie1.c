#include "../../include/Queries/querie1.h"
#include "../../include/structs.h"
#include "../../include/management.h"
#include "../../include/statistics.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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
    GHashTable *group_constituents;
};

struct storage {
    UserTable *users;
    ArtistTable *artists;
    MusicTable *musics;
    AlbumTable *albums;
    HistoryTable *history;
};

struct Date {
    int year;
    int month;
    int day;
};


void query_q1(STORAGE *storage, const char *id, bool use_separator_equals, FILE *output) {
    if (!storage || !id || !output || strlen(id) < 2 || (id[0] != 'U' && id[0] != 'A')) {
        fprintf(stderr, "Erro: ID inválido ou parâmetros ausentes: %s\n", id ? id : "NULL");
        fprintf(output, "\n");
        return;
    }

    char separator = use_separator_equals ? '=' : ';';

    if (id[0] == 'U') {
        GHashTable *users_table = getUsers(storage)->utable;
        if (!users_table) {
            fprintf(stderr, "Erro: Tabela de utilizadores não está disponível.\n");
            fprintf(output, "\n");
            return;
        }

        User *user = g_hash_table_lookup(users_table, id);
        if (!user) {
            fprintf(output, "\n");
            return;
        }

        const char *email = get_user_email(user);
        const char *first_name = get_user_first_name(user);
        const char *last_name = get_user_last_name(user);
        struct Date *birth_date = get_user_birth_date(user);
        const char *country = get_user_country(user);

        if (!email || !first_name || !last_name || !birth_date || !country) {
            fprintf(stderr, "Erro: Dados incompletos para o utilizador %s.\n", id);
            fprintf(output, "\n");
            return;
        }

        int age = 2024 - birth_date->year;
        if (birth_date->month > 9 || (birth_date->month == 9 && birth_date->day > 9)) {
            age--;
        }

        fprintf(output, "%s%c%s%c%s%c%d%c%s\n", 
                email, separator, 
                first_name, separator, 
                last_name, separator, 
                age, separator, 
                country);
    } else if (id[0] == 'A') {
        GHashTable *artists_table = getArtists(storage)->atable;
        if (!artists_table) {
            fprintf(stderr, "Erro: Tabela de artistas não está disponível.\n");
            fprintf(output, "\n");
            return;
        }

        Artist *artist = g_hash_table_lookup(artists_table, id);
        if (!artist) {
            fprintf(output, "\n");
            return;
        }

        const char *name = get_artist_name(artist);
        const char *type = get_artist_type(artist);
        const char *country = get_artist_country(artist);

        if (!name || !type || !country) {
            fprintf(stderr, "Erro: Dados incompletos para o artista %s.\n", id);
            fprintf(output, "\n");
            return;
        }

        int num_albums = calculate_num_albums_for_artist(id, getAlbums(storage));
        double total_revenue = calculate_total_revenue_for_artist(id, getArtists(storage), getMusics(storage), getHistory(storage));
        double participation_revenue = calculate_participation_revenue_for_artist(id, getArtists(storage), getMusics(storage), getHistory(storage));
        total_revenue += participation_revenue;

        fprintf(output, "%s%c%s%c%s%c%d%c%.2f\n", 
                name, separator, 
                type, separator, 
                country, separator, 
                num_albums, separator, 
                total_revenue);
    } else {
        fprintf(stderr, "Erro: ID %s não reconhecido.\n", id);
        fprintf(output, "\n");
    }
}

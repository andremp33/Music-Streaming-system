#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

#include "../../include/utils.h"
#include "../../include/parser.h"
#include "../../include/structs.h"
#include "../../include/management.h" 

#define MAX_PATH_LENGTH 512
#define MAX_LINE_SIZE 16384 

#define ARTISTS_DATASET_NAME "artists.csv"
#define ARTISTS_ERRORS_DATASET_NAME "artists_errors.csv"

#define MUSICS_DATASET_NAME "musics.csv"
#define MUSICS_ERRORS_DATASET_NAME "musics_errors.csv"

#define USERS_DATASET_NAME "users.csv"
#define USERS_ERRORS_DATASET_NAME "users_errors.csv"

#define ALBUMS_DATASET_NAME "albums.csv"
#define ALBUMS_ERRORS_DATASET_NAME "albums_errors.csv"

#define HISTORY_DATASET_NAME "history.csv"
#define HISTORY_ERRORS_DATASET_NAME "history_errors.csv"

struct storage {
    UserTable *users;
    ArtistTable *artists;
    MusicTable *musics;
    AlbumTable *albums;
    HistoryTable *history;
};

struct Music {
    char *id;           
    char *title;         
    GArray *artist_id_str;
    char *album_id_str;   
    char *duration;      
    char *genre;         
    int year_str;           
    char *lyrics;        
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

bool parse_artists(const char *datasetDir, const char *outputDir, 
                   ArtistTable *artists_table, AlbumTable *albums_table, 
                   MusicTable *musics_table, HistoryTable *history_table) {
    char datasetPath[MAX_PATH_LENGTH];
    snprintf(datasetPath, MAX_PATH_LENGTH, "%s/%s", datasetDir, ARTISTS_DATASET_NAME);

    FILE *file = fopen(datasetPath, "r");
    if (!file) {
        perror("Failed to open artists dataset file");
        return false;
    }

    FILE *error_registry = initialize_error_registery(outputDir, ARTISTS_ERRORS_DATASET_NAME);
    if (!error_registry) {
        perror("Failed to open error registry file for artists");
        fclose(file);
        return false;
    }

    char line[MAX_LINE_SIZE];
    if (!fgets(line, sizeof(line), file)) {
        perror("Error reading header");
        fclose(file);
        close_error_registery(error_registry);
        return false;
    }
    fprintf(error_registry, "%s", line);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        Artist *artist = create_artist_from_line(line, error_registry, artists_table);

        if (artist) {
            // Calculate and set additional artist properties
            artist->num_albums = calculate_num_albums_for_artist(artist->id, albums_table);
            artist->total_revenue = calculate_total_revenue_for_artist(artist->id, musics_table, history_table);

            add_artist_to_table(artists_table, artist);
        }
    }

    // Processar grupos e seus membros após carregar todos os artistas
    if (!parse_groups_and_artists(artists_table)) {
        fprintf(stderr, "Error processing groups and artists.\n");
        close_error_registery(error_registry);
        fclose(file);
        return false;
    }

    close_error_registery(error_registry);
    fclose(file);

    return true;
}



bool parse_users(const char *datasetDir, const char *outputDir, UserTable *users_table, MusicTable *musics_table) {
    char datasetPath[MAX_PATH_LENGTH];
    snprintf(datasetPath, MAX_PATH_LENGTH, "%s/%s", datasetDir, USERS_DATASET_NAME);

    FILE *file = fopen(datasetPath, "r");
    if (!file) {
        perror("Failed to open users dataset file");
        return false;
    }

    FILE *error_registry = initialize_error_registery(outputDir, USERS_ERRORS_DATASET_NAME);
    if (!error_registry) {
        perror("Failed to open error registry file for users");
        fclose(file);
        return false;
    }

    char line[MAX_LINE_SIZE];
    if (!fgets(line, sizeof(line), file)) {
        perror("Error reading header");
        fclose(file);
        close_error_registery(error_registry);
        return false;
    }
    fprintf(error_registry, "%s", line);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        User *user = create_user_from_line(line, error_registry, users_table, musics_table);

        if (user) {
            add_user_to_table(users_table, user); // Chamando o manager corretamente
        }
    }

    close_error_registery(error_registry);
    fclose(file);
    return true;
}

bool parse_albums(const char *datasetDir, const char *outputDir, AlbumTable *albums_table, ArtistTable *artists_table) {
    char datasetPath[MAX_PATH_LENGTH];
    snprintf(datasetPath, MAX_PATH_LENGTH, "%s/%s", datasetDir, ALBUMS_DATASET_NAME);

    FILE *file = fopen(datasetPath, "r");
    if (!file) {
        perror("Failed to open albums dataset file");
        return false;
    }

    FILE *error_registry = initialize_error_registery(outputDir, ALBUMS_ERRORS_DATASET_NAME);
    if (!error_registry) {
        perror("Failed to open error registry file for albums");
        fclose(file);
        return false;
    }

    char line[MAX_LINE_SIZE];
    if (!fgets(line, sizeof(line), file)) {
        perror("Error reading header");
        fclose(file);
        close_error_registery(error_registry);
        return false;
    }
    fprintf(error_registry, "%s", line);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        Album *album = create_album_from_line(line, error_registry, albums_table, artists_table);

        if (album) {
            add_album_to_table(albums_table, album); // Chamando o manager corretamente
        }
    }

    close_error_registery(error_registry);
    fclose(file);
    return true;
}

bool parse_musics(const char *datasetDir, const char *outputDir, MusicTable *musics_table, ArtistTable *artists_table, AlbumTable *albums_table) {
    char datasetPath[MAX_PATH_LENGTH];
    snprintf(datasetPath, MAX_PATH_LENGTH, "%s/%s", datasetDir, MUSICS_DATASET_NAME);

    FILE *file = fopen(datasetPath, "r");
    if (!file) {
        perror("Failed to open musics dataset file");
        return false;
    }

    FILE *error_registry = initialize_error_registery(outputDir, MUSICS_ERRORS_DATASET_NAME);
    if (!error_registry) {
        perror("Failed to open error registry file for musics");
        fclose(file);
        return false;
    }

    char line[MAX_LINE_SIZE];
    if (!fgets(line, sizeof(line), file)) {
        perror("Error reading header");
        fclose(file);
        close_error_registery(error_registry);
        return false;
    }
    fprintf(error_registry, "%s", line);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        Music *music = create_music_from_line(line, error_registry, musics_table, artists_table, albums_table);

        if (music) {
            add_music_to_table(musics_table, music); // Chamando o manager corretamente
        }
    }

    close_error_registery(error_registry);
    fclose(file);
    return true;
}

// Parser para histórico
bool parse_history(const char *datasetDir, const char *outputDir, HistoryTable *history_table, UserTable *users_table, MusicTable *musics_table) {
    char datasetPath[MAX_PATH_LENGTH];
    snprintf(datasetPath, sizeof(datasetPath), "%s/%s", datasetDir, HISTORY_DATASET_NAME);

    FILE *file = fopen(datasetPath, "r");
    if (!file) {
        perror("Failed to open history dataset file");
        return false;
    }

    FILE *error_registry = initialize_error_registery(outputDir, HISTORY_ERRORS_DATASET_NAME);
    if (!error_registry) {
        perror("Failed to open error registry file for history");
        fclose(file);
        return false;
    }

    char line[MAX_LINE_SIZE];
    if (!fgets(line, sizeof(line), file)) {
        perror("Error reading header");
        fclose(file);
        close_error_registery(error_registry);
        return false;
    }
    fprintf(error_registry, "%s", line);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        History *history = create_history_from_line(line, error_registry, history_table, users_table, musics_table);

        if (history) {
            add_history_to_table(history_table, history); // Chamando o manager corretamente
        }
    }

    close_error_registery(error_registry);
    fclose(file);
    return true;
}

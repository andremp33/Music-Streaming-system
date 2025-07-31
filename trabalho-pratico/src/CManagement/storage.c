#include "../../include/parser.h"
#include "../../include/management.h"
#include "../../include/structs.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct storage {
    UserTable *users;
    ArtistTable *artists;
    MusicTable *musics;
    AlbumTable *albums;
    HistoryTable *history;
};



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

STORAGE *newStorage() {
    STORAGE *storage = g_new(STORAGE, 1);
    storage->users = create_user_table();
    storage->artists = create_artists_table();
    storage->musics = create_music_table();
    storage->albums = create_album_table();
    storage->history = create_history_table();
    return storage;
}


void free_storage(STORAGE *storage) {
    if (storage) {
        free_user_table(storage->users);
        free_artist_table(storage->artists);
        free_music_table(storage->musics);
        free_album_table(storage->albums);
        free_history_table(storage->history);
        g_free(storage);
    }
}

bool populate_storage(STORAGE *storage, const char *datasetDir, const char *outputDir) {
    if (!storage || !datasetDir || !outputDir) {
        fprintf(stderr, "Erro: Argumentos inválidos fornecidos para populate_storage.\n");
        return false;
    }

    // Parse artists and populate the relevant data
    if (!parse_artists(datasetDir, outputDir, getArtists(storage), getAlbums(storage), getMusics(storage), getHistory(storage))) {
        fprintf(stderr, "Erro ao carregar dados de artistas.\n");
        return false;
    }

    if (!parse_albums(datasetDir, outputDir, getAlbums(storage), getArtists(storage))) {
        fprintf(stderr, "Erro ao carregar os álbuns.\n");
        return false;
    }

    if (!parse_musics(datasetDir, outputDir, getMusics(storage), getArtists(storage), getAlbums(storage))) {
        fprintf(stderr, "Erro ao carregar as músicas.\n");
        return false;
    }

    if (!parse_users(datasetDir, outputDir, getUsers(storage), getMusics(storage))) {
        fprintf(stderr, "Erro ao carregar os usuários.\n");
        return false;
    }

    if (!parse_history(datasetDir, outputDir, getHistory(storage), getUsers(storage), getMusics(storage))) {
        fprintf(stderr, "Erro ao carregar o histórico.\n");
        return false;
    }
}


UserTable *getUsers(STORAGE *s) {
    return s->users;
}

ArtistTable *getArtists(STORAGE *s) {
    return s->artists;
}

MusicTable *getMusics(STORAGE *s) {
    return s->musics;
}

AlbumTable *getAlbums(STORAGE *s) {
    return s->albums;
}

HistoryTable *getHistory(STORAGE *s) {
    return s->history;
}
#ifndef QUERIE2_H
#define QUERIE2_H

/**
 * @file querie2.h
 * @brief Header file for Query 2 implementation.
 *
 * This file contains the function declarations and data structures used
 * for processing Query 2, which retrieves the top N artists with the largest
 * discography, optionally filtered by country.
 */

#include <glib.h>
#include <stdio.h>
#include "../structs.h"

/**
 * @typedef ArtistDiscography
 * @brief Represents an artist and the total duration of their discography.
 */
typedef struct ArtistDiscography ArtistDiscography;

/**
 * @typedef STORAGE
 * @brief Represents the storage structure containing all application data.
 */
typedef struct storage STORAGE;

/**
 * @brief Compares two artists by the total duration of their discography.
 *
 * This function is used for sorting artists in descending order of total
 * discography duration. In case of a tie, artists are sorted by their IDs
 * in ascending order.
 *
 * @param a Pointer to the first ArtistDiscography structure.
 * @param b Pointer to the second ArtistDiscography structure.
 * @return Negative if `a` is greater than `b`, positive if `b` is greater than `a`, and 0 if they are equal.
 */
int compare_artists(const void *a, const void *b);

/**
 * @brief Processes Query 2: retrieves the top N artists with the largest discography.
 *
 * This function calculates the total duration of the discographies for all artists
 * and selects the top N artists. Results can optionally be filtered by country and
 * are written to the specified output file.
 *
 * @param storage Pointer to the storage structure containing application data.
 * @param N The number of top artists to retrieve.
 * @param country_filter Optional country filter. If NULL, no filtering is applied.
 * @param use_separator_equals Flag to determine if output uses `=` as a separator instead of `;`.
 * @param output_file Path to the file where results will be written.
 */
void query_q2(STORAGE *storage, int N, const char *country_filter, bool use_separator_equals, const char *output_file);

#endif // QUERIE2_H


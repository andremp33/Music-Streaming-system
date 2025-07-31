#ifndef QUERIE3_H
#define QUERIE3_H

/**
 * @file querie3.h
 * @brief Header file for Query 3 implementation.
 *
 * This file contains the function declarations and data structures used
 * for processing Query 3, which determines the most popular music genres
 * for a given age range of users.
 */

#include <glib.h>
#include <stdio.h>
#include "../structs.h"

/**
 * @typedef GenreLikes
 * @brief Represents a music genre and its associated total likes.
 */
typedef struct GenreLikes GenreLikes;

/**
 * @typedef Date
 * @brief Represents a date structure (year, month, day).
 */
typedef struct Date Date;

/**
 * @typedef STORAGE
 * @brief Represents the storage structure containing all application data.
 */
typedef struct storage STORAGE;

/**
 * @typedef ThreadData
 * @brief Represents data passed to each thread for batch processing in Query 3.
 */
typedef struct ThreadData ThreadData;

/**
 * @brief Processes Query 3: determines the most popular music genres for a given age range.
 *
 * This function computes the popularity of music genres for users within a specified
 * age range. The results are written to the specified output file, ordered by popularity.
 *
 * @param storage Pointer to the storage structure containing application data.
 * @param min_age Minimum age in the specified range.
 * @param max_age Maximum age in the specified range.
 * @param use_separator_equals Flag to determine if output uses `=` as a separator instead of `;`.
 * @param output_file Path to the file where results will be written.
 */
void query_q3(STORAGE *storage, int min_age, int max_age, bool use_separator_equals, const char *output_file);

/**
 * @brief Processes a batch of users to calculate genre likes.
 *
 * This function is intended to be run as a thread, where each thread handles
 * a subset of the users. It calculates the popularity of music genres for
 * the assigned users and accumulates the results.
 *
 * @param arg Pointer to a ThreadData structure containing batch-specific data.
 * @return A pointer to the result of the batch processing.
 */
void *process_batch(void *arg);

/**
 * @brief Compares two genres by their total likes.
 *
 * This function is used for sorting genres in descending order of popularity.
 * In case of a tie, genres are sorted alphabetically.
 *
 * @param a Pointer to the first GenreLikes structure.
 * @param b Pointer to the second GenreLikes structure.
 * @return Negative if `a` is greater than `b`, positive if `b` is greater than `a`, and 0 if they are equal.
 */
int compare_genres(const void *a, const void *b);

#endif 

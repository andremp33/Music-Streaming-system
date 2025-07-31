#ifndef QUERIE1_H
#define QUERIE1_H

/**
 * @file querie1.h
 * @brief Header file for Query 1 implementation.
 *
 * This file contains the function declaration and related data structures
 * used for processing Query 1, which provides a summary of a user or artist
 * based on a unique identifier.
 */

#include "../structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @brief Processes Query 1: retrieves a summary of a user or artist by ID.
 *
 * This function retrieves and writes a summary of the specified user or artist
 * to the provided output stream. The summary includes specific details based
 * on the entity type (user or artist) and uses a specified separator format
 * for the output.
 *
 * @param storage Pointer to the storage structure containing application data.
 * @param id The unique identifier of the user or artist.
 * @param use_separator_equals If true, uses '=' as the separator; otherwise, uses ';'.
 * @param output The file stream where the results will be written.
 */
void query_q1(STORAGE *storage, const char *id, bool use_separator_equals, FILE *output);

#endif 

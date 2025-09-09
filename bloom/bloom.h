/*
 *  Copyright (c) 2012-2019, Jyri J. Virkki
 *  All rights reserved.
 *
 *  This file is under BSD license. See LICENSE file.
 */

#ifndef _BLOOM_H
#define _BLOOM_H

#ifdef _WIN64
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** ***************************************************************************
 * Structure to keep track of one bloom filter.  Caller needs to
 * allocate this and pass it to the functions below. First call for
 * every struct must be to bloom_init().
 *
 */
struct bloom
{
  // These fields are part of the public interface of this structure.
  // Client code may read these values if desired. Client code MUST NOT
  // modify any of these.
  uint64_t entries;
  uint64_t bits;
  uint64_t bytes;
  uint8_t hashes;
  long double error;

  // Fields below are private to the implementation. These may go away or
  // change incompatibly at any moment. Client code MUST NOT access or rely
  // on these.
  uint8_t ready;
  uint8_t major;
  uint8_t minor;
  double bpe;
  uint8_t *bf;
};
/*
Customs
*/

/*
int bloom_loadcustom(struct bloom * bloom, char * filename);
int bloom_savecustom(struct bloom * bloom, char * filename);
*/


/** ***************************************************************************
 * Initialize the bloom filter for use.
 *
 * The filter is initialized with a bit field and number of hash functions
 * according to the computations from the wikipedia entry:
 *     http://en.wikipedia.org/wiki/Bloom_filter
 *
 * Optimal number of bits is:
 *     bits = (entries * ln(error)) / ln(2)^2
 *
 * Optimal number of hash functions is:
 *     hashes = bpe * ln(2)
 *
 * Parameters:
 * -----------
 *     bloom   - Pointer to an allocated struct bloom (see above).
 *     entries - The expected number of entries which will be inserted.
 *               Must be at least 1000 (in practice, likely much larger).
 *     error   - Probability of collision (as long as entries are not
 *               exceeded).
 *
 * Return:
 * -------
 *     0 - on success
 *     1 - on failure
 *
 */
int bloom_init2(struct bloom * bloom, uint64_t entries, long double error);


/**
 * DEPRECATED.
 * Kept for compatibility with libbloom v.1. To be removed in v3.0.
 *
 * Use bloom_init2 instead.
 *
 */
int bloom_init(struct bloom * bloom, size_t entries, double error);

/** ***************************************************************************
 * Add an item to the bloom filter.
 *
 * Parameters:
 * -----------
 *     bloom  - Pointer to an allocated struct bloom (see above).
 *     buffer - Pointer to buffer containing data to add.
 *     len    - Size of the buffer.
 *
 * Return:
 * -------
 *     0 - on success
 *     1 - on failure (bloom not initialized)
 *
 */
int bloom_add(struct bloom * bloom, const void * buffer, int len);


/** ***************************************************************************
 * Check if an item is possibly in the bloom filter.
 *
 * Note that a returned 1 does not definitively mean the item is in the
 * bloom filter, only that it is a probable hit. A returned 0 means the
 * item is definitely not in the filter.
 *
 * Parameters:
 * -----------
 *     bloom  - Pointer to an allocated struct bloom (see above).
 *     buffer - Pointer to buffer containing data to check.
 *     len    - Size of the buffer.
 *
 * Return:
 * -------
 *     0 - item definitely not present
 *     1 - item possibly present
 *     2 - bloom not initialized
 *
 */
int bloom_check(struct bloom * bloom, const void * buffer, int len);


/** ***************************************************************************
 * Print (to stdout) info about this bloom filter. Debugging aid.
 *
 */
void bloom_print(struct bloom * bloom);


/** ***************************************************************************
 * Deallocate internal storage.
 *
 * Upon return, the bloom struct is no longer usable. You may call bloom_init
 * again on the same struct to reinitialize it again.
 *
 * Parameters:
 * -----------
 *     bloom  - Pointer to an allocated struct bloom (see above).
 *
 * Return: none
 *
 */
void bloom_free(struct bloom * bloom);


/** ***************************************************************************
 * Erase internal storage.
 *
 * Erases all elements. Upon return, the bloom struct returns to its initial
 * (initialized) state.
 *
 * Parameters:
 * -----------
 *     bloom  - Pointer to an allocated struct bloom (see above).
 *
 * Return:
 *     0 - on success
 *     1 - on failure
 *
 */
int bloom_reset(struct bloom * bloom);


/** ***************************************************************************
 * Save a bloom filter to a file.
 *
 * Parameters:
 * -----------
 *     bloom    - Pointer to an allocated struct bloom (see above).
 *     filename - Create (or overwrite) bloom data to this file.
 *
 * Return:
 *     0 - on success
 *     1 - on failure
 *
 */
//int bloom_save(struct bloom * bloom, char * filename);


/** ***************************************************************************
 * Load a bloom filter from a file.
 *
 * This functions loads a file previously saved with bloom_save().
 *
 * Parameters:
 * -----------
 *     bloom    - Pointer to an allocated struct bloom (see above).
 *     filename - Load bloom filter data from this file.
 *
 * Return:
 *     0   - on success
 *     > 0 - on failure
 *
 */
//int bloom_load(struct bloom * bloom, char * filename);


/** ***************************************************************************
 * Returns version string compiled into library.
 *
 * Return: version string
 *
 */
const char * bloom_version();

#ifdef __cplusplus
}
#endif

#endif
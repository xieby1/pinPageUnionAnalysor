#ifndef H_PAGEUNIONCHECKER
#define H_PAGEUNIONCHECKER

#include <stdint.h>
#include <stdlib.h> // bsearch

#include "proc_maps_parser/pmparser.h"

typedef uintptr_t puc_addr;
// Size in power form. E.g. 16k = 2^14
typedef uint8_t puc_pow_size;
typedef struct
{
    procmaps_struct *map;
    // stats
    /// bool
    uint8_t page_union_over_16k_boundary;
    /// bool
    uint8_t page_union_prot_unmatch[2]; // lower, higher
} procmaps_stat_struct;

// Global variables
procmaps_iterator *maps_iter;
procmaps_stat_struct *maps_stat_array;
uint32_t maps_len;

/**
 * @brief Initialize PUC maps related data structures
 * @param maps_file The path of maps file
 */
int PUC_init(char *maps_file);

/**
 * @brief Clean PUC data structures
 */
int PUC_exit(void);

/**
 * @brief Check whether a given `addr` is safe, with a given guest page size
 * `gps` and host page size `hps`
 * @param addr The addr to be checked
 * @param gps2 Guest Page Size in power form
 * @param hps2 Host Page Size in power form
 * @return Safe 1, unsafe 0
 */
int PUC_is_safe(puc_addr addr, puc_pow_size gps2, puc_pow_size hps2);

#endif // H_PAGEUNIONCHECKER

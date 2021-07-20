#ifndef H_PAGEUNIONCHECKER
#define H_PAGEUNIONCHECKER

#include <stdint.h>
#include <stdlib.h> // bsearch

#include "proc_maps_parser/pmparser.h"

typedef uintptr_t puc_addr;
typedef struct
{
    procmaps_struct *map;
    // stats
    uintptr_t lower_host_page_tag;
    uintptr_t higher_host_page_tag;
    uint32_t count_lower_unsafe;
    uint32_t count_higher_unsafe;
    uint32_t count_safe;
} procmaps_stat_struct;

// Global variables
procmaps_iterator *maps_iter;
procmaps_stat_struct *maps_stat_array;
uint32_t maps_len;
uint32_t count_safe, count_unsafe; // global
// Host/Guest Page Size
#define HPS (1 << 14)
#define GPS (1 << 12)
#define MASK_HPTag (~(HPS - 1))

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
 * @brief For a given addr, generate its stat in its procmaps_stat_struct,
 * return 1 if this addr is safe, 0 if not safe.
 */
int PUC_stat(puc_addr addr);

/**
 * @brief print stat to stdout
 */
void PUC_print_stat(void);

#endif // H_PAGEUNIONCHECKER

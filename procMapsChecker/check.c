#include "check.h"

int PUC_init(char *maps_file)
{
    maps_len = 0;
    maps_iter = pmparser_parse_file(maps_file);
    for (procmaps_struct *map = pmparser_head(maps_iter); map != NULL;
         map = pmparser_next(maps_iter))
    {
        maps_len++;
    }

    maps_stat_array = malloc(maps_len * sizeof(procmaps_stat_struct));

    int i = 0;
    for (procmaps_struct *map = pmparser_head(maps_iter); map != NULL;
         map = pmparser_next(maps_iter), i++)
    {
        maps_stat_array[i].map = map;
    }

    return 0;
}

int PUC_exit(void)
{
    free(maps_stat_array);

    return 0;
}

/**
 * @brief Compare key (first arg) and arrary member (second arg), according to
 * manual
 * @param a1 Addr to be checked
 * @param a2 A map stat structure in maps_stat_array
 */
static int cmp_map(const void *a1, const void *a2)
{
    puc_addr *paddr = (puc_addr *)a1;
    procmaps_struct *pmap = ((procmaps_stat_struct *)a2)->map;

    if ((*paddr) < (puc_addr)(pmap->addr_start))
        return -1;
    else if ((*paddr) < (puc_addr)(pmap->addr_end))
        return 0;
    else
        return 1;
}

/**
 * @brief Get permission of the page, which covers addr
 * @return * [0, 0b1111], if page is found; others, if page is not found
 */
static char get_perm(puc_addr addr)
{
    procmaps_stat_struct *smap = bsearch(&addr, maps_stat_array, maps_len,
                                         sizeof(procmaps_stat_struct), cmp_map);

    if (smap == NULL)
        return -1;
    else
        return smap->map->rwxp;
}

int PUC_is_safe(puc_addr addr)
{
    if (GPS >= HPS)
        return 1;
    // Host Page Mask
    puc_addr hpm = ~(HPS - 1);
    // Number of Guest pages in a Host pages
    int ngh = HPS / GPS;

    addr &= hpm;

    char perm = get_perm(addr);
    for (int i = 1; i < ngh; i++)
        if (perm != get_perm(addr + i * GPS))
            return 0;
    return 1;
}

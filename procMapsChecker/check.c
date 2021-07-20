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

    // make sure the maps_stat_array are zeroed out
    maps_stat_array = calloc(maps_len, sizeof(procmaps_stat_struct));

    int i = 0;
    for (procmaps_struct *map = pmparser_head(maps_iter); map != NULL;
         map = pmparser_next(maps_iter), i++)
    {
        maps_stat_array[i].map = map;

        maps_stat_array[i].lower_host_page_tag =
            (uintptr_t)map->addr_start & MASK_HPTag;
        maps_stat_array[i].higher_host_page_tag =
            (uintptr_t)map->addr_end & MASK_HPTag;
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
 *  @breif Get procmaps_stat struct of a given host addr
 */
static procmaps_stat_struct *get_smap(puc_addr addr)
{
    procmaps_stat_struct *smap = bsearch(&addr, maps_stat_array, maps_len,
                                         sizeof(procmaps_stat_struct), cmp_map);

    if (smap == NULL)
        return NULL;
    else
        return smap;
}

/**
 * @brief Get permission of the page, which covers addr
 * @return * [0, 0b1111], if page is found; others, if page is not found
 */
static char get_perm_by_smap(procmaps_stat_struct *smap)
{
    if (smap == NULL)
        return -1;
    else
        return smap->map->rwxp;
}

int PUC_stat(puc_addr addr)
{
    procmaps_stat_struct *smap = get_smap(addr);
    char perm = get_perm_by_smap(smap);

    int ngh = HPS / GPS;
    uintptr_t hpt = addr & MASK_HPTag;
    for (int i = 0; i < ngh; i++)
    {
        procmaps_stat_struct *smap1 = get_smap(hpt + i * GPS);
        if (smap1 == smap)
            continue;
        char perm1 = get_perm_by_smap(smap1);
        if (perm1 != perm)
        {
            if (smap)
            {
                if (hpt == smap->lower_host_page_tag)
                    smap->count_lower_unsafe++;
                else if (hpt == smap->higher_host_page_tag)
                    smap->count_higher_unsafe++;
                else
                    fprintf(stderr,
                            "warn: midlle unsafe not supposed to exist!\n");
            }
            return 0;
        }
    }
    return 1;
}

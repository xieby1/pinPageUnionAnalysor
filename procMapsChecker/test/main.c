#include <stdio.h>

#include "../check.h"

#define GPS2 12
#define HPS2 14

#define assert(cond)                                                        \
    do                                                                      \
    {                                                                       \
        if (!(cond))                                                        \
        {                                                                   \
            fprintf(stderr, "\033[31massert failed\033[m @ %s: %s: %d: \n", \
                    __FILE__, __FUNCTION__, __LINE__);                      \
        }                                                                   \
    } while (0)

int main(void)
{
    PUC_init("maps");

    assert(!PUC_is_safe(0x12345, GPS2, HPS2));
    assert(!PUC_is_safe(0x66196, GPS2, HPS2));
    assert(PUC_is_safe(0x76543, GPS2, HPS2));

    printf("done\n");
    return 0;
}

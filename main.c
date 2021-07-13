#include "main.h"
#include "procMapsChecker/check.h"

#include <inttypes.h> // fmt of 'printf scanf'
#include <stdio.h>    // fscanf, fprintf, fopen
#include <string.h>   // strerror

static void usage(void)
{
    fprintf(stdout, "Usage: mcpuc <path-to-maps> <path-to-memcount>\n");
}

int main(int argc, char **argv)
{
    char *file_maps, *file_memcount;
    if (argc != 3)
    {
        usage();
        return -1;
    }
    else
    {
        file_maps = argv[1];
        file_memcount = argv[2];
    }

    // init
    PUC_init(file_maps);
    FILE *FILE_memcount = fopen(file_memcount, "r");
    if (!FILE_memcount)
    {
        fprintf(stderr, "mcpuc: cannot open the memory maps, %s\n",
                strerror(errno));
        return -1;
    }

    int count_safe=0, count_unsafe=0;
    memcount_record mcr;
    const char *scanfmt =
        "%" SCNxPTR "%" SCNx32 "%" SCNu8 "%" SCNu8 "%" SCNxPTR;
    while (fscanf(FILE_memcount, scanfmt, &mcr.pc, &mcr.inst, &mcr.rw,
                          &mcr.size, &mcr.addr) == NUM_MEMBER_MC_REC)
    {
//        fprintf(stdout,
//                "%" PRIxPTR " %" PRIu32 "%" PRIu8 "%" PRIu8 "%" PRIuPTR "\n",
//                mcr.pc, mcr.inst, mcr.rw, mcr.size, mcr.addr);
        if ( PUC_is_safe(mcr.addr, 12, 14) )
            count_safe ++;
        else
            count_unsafe ++;
    }

    fprintf(stdout, "safe %d unsafe %d\n", count_safe, count_unsafe);

    PUC_exit();
    return 0;
}

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

    memcount_record mcr;
    const char *scanfmt =
        "%" SCNxPTR "%" SCNx32 "%" SCNu8 "%" SCNu8 "%" SCNxPTR;
    while (fscanf(FILE_memcount, scanfmt, &mcr.pc, &mcr.inst, &mcr.rw,
                  &mcr.size, &mcr.addr) == NUM_MEMBER_MC_REC)
    {
        PUC_stat(mcr.addr);
    }

    PUC_print_stat();

    PUC_exit();
    return 0;
}

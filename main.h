#ifndef H_MEMCOUNT_PAGEUNIONCHECKER
#define H_MEMCOUNT_PAGEUNIONCHECKER
#include <stdint.h>

#define NUM_MEMBER_MC_REC 5
typedef struct memcount_record
{
    uintptr_t pc;
    uint32_t inst;
    uint8_t rw; // read 0, write 1
    uint8_t size;
    uintptr_t addr;
} memcount_record;

#endif // H_MEMCOUNT_PAGEUNIONCHECKER

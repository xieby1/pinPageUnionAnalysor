/*
 * Copyright 2002-2020 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 */

/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

// based on ../ManualExamples/pinatrace.cpp

#include <stdio.h>
#include "pin.H"

// global variables
#define unlikely(x) __builtin_expect((x), 0)
static FILE *trace;
static unsigned int samplingCount = 0;

// command line options
KNOB <unsigned int> KnobSampleRate(KNOB_MODE_WRITEONCE, "pintool", "r", "65536", "sample rate: sample per <s> memory accesses");

// Print a memory read record
VOID RecordMemRead(VOID *ip, VOID *addr, UINT32 size)
{
    samplingCount++;
    if (unlikely(samplingCount==KnobSampleRate.Value()))
    {
        samplingCount = 0;
        fprintf(trace, "%p %8x 0 %u %p\n", ip, *(UINT32 *)ip, size, addr);
    }
}

// Print a memory write record
VOID RecordMemWrite(VOID *ip, VOID *addr, UINT32 size)
{
    samplingCount++;
    if (unlikely(samplingCount==KnobSampleRate.Value()))
    {
        samplingCount = 0;
        fprintf(trace, "%p %8x 1 %u %p\n", ip, *(UINT32 *)ip, size, addr);
    }
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID* v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR,
                    IARG_MEMORYOP_EA, memOp, IARG_MEMORYREAD_SIZE, IARG_END);
        }
        // Note that in some architectures a single memory operand can be
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite, IARG_INST_PTR,
                    IARG_MEMORYOP_EA, memOp, IARG_MEMORYWRITE_SIZE, IARG_END);
        }
    }
}

#include <unistd.h> // getpid
#define PATH_STR_LEN (1 << 7)
#define BUF_SIZE (1 << 16)
void copyProcMaps(void)
{
    FILE *src, *dst;
    char pathProcMaps[PATH_STR_LEN];
    sprintf(pathProcMaps, "/proc/%d/maps", getpid());
    src = fopen(pathProcMaps, "r");
    if (!src)
    {
        fprintf(stderr, "fopen %s failed\n", pathProcMaps);
        return;
    }
    dst = fopen("maps", "w");
    if (!dst)
    {
        fprintf(stderr, "fopen maps failed\n");
        return;
    }

    char *buf = (char *)malloc(BUF_SIZE * sizeof(char));
    size_t in, out;
    while (1)
    {
        in = fread(buf, sizeof(char), BUF_SIZE, src);
        if (!in)
            break;
        out = fwrite(buf, sizeof(char), in, dst);
        if (!out)
        {
            fprintf(stderr, "fwrite failed\n");
            return;
        }
    }
    fclose(src);
    fclose(dst);
    return;
}

VOID Fini(INT32 code, VOID* v)
{
    copyProcMaps();
    fclose(trace);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints a trace of memory addresses\n" + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char* argv[])
{
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("pinatrace.out", "w");

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}

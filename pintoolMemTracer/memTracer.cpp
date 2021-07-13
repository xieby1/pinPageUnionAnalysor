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

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static unsigned long long icount = 0;

// This function is called before every block
// Use the fast linkage for calls
VOID PIN_FAST_ANALYSIS_CALL docount(ADDRINT c) { icount += c; }

// Pin calls this function every time a new basic block is encountered
// It inserts a call to docount
VOID Trace(TRACE arg_trace, VOID* v)
{
    // Visit every basic block  in the arg_trace
    for (BBL bbl = TRACE_BblHead(arg_trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        // Insert a call to docount for every bbl, passing the number of instructions.
        // IPOINT_ANYWHERE allows Pin to schedule the call anywhere in the bbl to obtain best performance.
        // Use a fast linkage for the call.
        BBL_InsertCall(bbl, IPOINT_ANYWHERE, AFUNPTR(docount), IARG_FAST_ANALYSIS_CALL, IARG_UINT32, BBL_NumIns(bbl), IARG_END);
    }
}

FILE* trace;

// Print a memory read record
VOID RecordMemRead(VOID *ip, VOID *addr, UINT32 size)
{
    fprintf(trace, "%p: %8x R %u %p\n", ip, *(UINT32 *)ip, size, addr);
}

// Print a memory write record
VOID RecordMemWrite(VOID *ip, VOID *addr, UINT32 size)
{
    fprintf(trace, "%p: %8x W %u %p\n", ip, *(UINT32 *)ip, size, addr);
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
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp, IARG_MEMORYREAD_SIZE,
                                     IARG_END);
        }
        // Note that in some architectures a single memory operand can be
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp, IARG_MEMORYWRITE_SIZE,
                                     IARG_END);
        }
    }
}

#include <stdlib.h> // system
#include <unistd.h> // getpid
VOID Fini(INT32 code, VOID* v)
{
    fprintf(stderr, "pid %d\n", getpid());
    char command[256];
    scanf("%s", command);
    sprintf(command, "cp /proc/%d/maps pinatrace.maps", getpid());
    fprintf(stderr, "%s", command);
    int ret = system(command);
    int errsv = errno;
    if (ret < 0)
        fprintf(stderr, "ret: %d, errno: %d\n", ret, errsv);

    fprintf(trace, "Count %llu\n", icount);
    fprintf(trace, "#eof\n");
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
    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}

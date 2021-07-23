#!/bin/bash
# exit when command failed
set -e

# variables
PPUA_DEFAULT_SAMPLE_RATE=65536

${PIN_ROOT:?"not set"}/pin \
    -t ${PPUA_ROOT:?"not set"}/pintoolMemTracer/obj-intel64/memTracer.so \
    -r ${PPUA_SAMPLE_RATE:-${PPUA_DEFAULT_SAMPLE_RATE}} \
    -- \
    $@

${PPUA_ROOT:?"not set"}/analysor maps pinatrace.out

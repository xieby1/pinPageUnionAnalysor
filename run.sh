#!/bin/bash
${PIN_ROOT:?"not set"}/pin \
    -t ${PPUA_ROOT:?"not set"}/pintoolMemTracer/obj-intel64/memTracer.so \
    -- \
    $@

${PPUA_ROOT:?"not set"}/analysor maps pinatrace.out

#!/bin/bash

# variables
SPEC_ROOT=/home/xieby1/Codes/spec2000
PIN_ROOT=/home/xieby1/Softwares/pin-3.19-98425-gd666b2bee-gcc-linux
PPUA_ROOT=/home/xieby1/Codes/MyRepos/pinPageUnionAnalysor
PPUA_RUN=/home/xieby1/Codes/MyRepos/pinPageUnionAnalysor/run.sh
OPT_PIN=0 # 0 auto, 1 yes
OPT_ANA=1 # 0 no, 1 yes

cd ${SPEC_ROOT} || exit
source ./shrc

for list in "${SPEC_ROOT}"/benchspec/*/*/run/list
do
    info=$(grep "size=test" "${list}")
    number=$(echo "${info}" | awk '{print $1;}')

    cd "${list%/*}/${number}" || exit
    pwd

    # decide whether to execute pin
    if [[ -e maps && -e pinatrace.out ]]
    then
        exec_pin=${OPT_PIN}
    else
        exec_pin=1
    fi
    if [[ ${exec_pin} -eq 1 ]]
    then
        speccmd=$(specinvoke -n speccmds.cmd | grep -v "^#")
        speccmd_no_redirect=${speccmd%%>*}

        pin_cmd="${PIN_ROOT}/pin"
        pin_cmd+=" -t ${PPUA_ROOT}/pintoolMemTracer/obj-intel64/memTracer.so"
#        pin_cmd+=" -r "
        pin_cmd+=" --"
        pin_cmd+=" ${speccmd}"
        echo "$pin_cmd"
        eval "${pin_cmd}"
    fi

    # decide whether to execute analysor
    if [[ ${OPT_ANA} -eq 1 ]]
    then
        ${PPUA_ROOT}/analysor maps pinatrace.out
    fi
done

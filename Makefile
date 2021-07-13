ifndef PIN_ROOT
$(error env $PIN_ROOT is not set.)
# Tested with pin version 3.19.)
# Download pin "https://software.intel.com/content/www/us/en/develop/articles/pin-a-binary-instrumentation-tool-downloads.html".)
endif

CC = gcc
CFLAGS = -g

EXE = analysor

# sub-projects
DIR_CHECK = procMapsChecker
DIR_TRACE = pintoolMemTracer # pin 3.19

all: ${EXE} ${DIR_TRACE}

${DIR_TRACE}: FORCE
	PIN_ROOT="${PIN_ROOT}" make -C $@

${EXE}: main.o libcheck.a
	${CC} ${CFLAGS} main.o -L${DIR_CHECK} -lcheck -o $@

libcheck.a: FORCE
	make -C ${DIR_CHECK}

%.o: %.c %.h
	${CC} -c ${CFLAGS} -o $@ $<

clean: FORCE
	rm -f *.o ${EXE}
	make -C ${DIR_CHECK} clean
	PIN_ROOT="${PIN_ROOT}" make -C ${DIR_TRACE} clean

ORCE:

IMLIB_CFLAGS = `imlib-config --cflags`
IMLIB_LIBS = `imlib-config --libs`
GCCOPTS = -MMD -Wall
GCC = gcc

all : E-cpuStat.epplet

E-cpuStat.epplet : E-cpuStat.c
#	${GCC} ${GCCOPTS}  E-Mount.c -o E-Mount.epplet -lepplet ${IMLIB_LIBS} ${IMLIB_CFLAGS}
	gcc -g -O2 E-cpuStat.c -o E-cpuStat.epplet api/.libs/libepplet.so -L/usr/lib64 -lgtop-2.0 -ldl -lm -Wl,-rpath -Wl,/usr/local/lib -I /usr/include/libgtop-2.0

clean :
	-rm E-cpuStat.epplet

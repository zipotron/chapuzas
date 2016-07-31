# Daniel Erat, erat@cats.ucsc.edu

IMLIB_CFLAGS = `imlib-config --cflags`
IMLIB_LIBS = `imlib-config --libs`
GCCOPTS = -MMD -Wall
GCC = gcc

all : E-cpuMemTemp.epplet

E-cpuMemTemp.epplet : E-cpuMemTemp.c
#	${GCC} ${GCCOPTS}  E-Mount.c -o E-Mount.epplet -lepplet ${IMLIB_LIBS} ${IMLIB_CFLAGS}
	gcc -g -O2 E-cpuMemTemp.c -o E-cpuMemTemp.epplet /usr/local/lib/libepplet.so -L/usr/lib -lgtop-2.0 -ldl -lm -Wl,-rpath -Wl,/usr/local/lib -I /usr/include/libgtop-2.0 -I /usr/include/glib-2.0 -I /usr/lib/arm-linux-gnueabihf/glib-2.0/include/

clean :
	-rm E-cpuMem.epplet

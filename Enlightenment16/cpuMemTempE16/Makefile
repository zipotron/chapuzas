all : E-cpuMemTemp.epplet

E-cpuMemTemp.epplet : E-cpuMemTemp.c
	gcc -g -O2 E-cpuMemTemp.c -o E-cpuMemTemp.epplet /usr/lib64/libepplet.so -L/usr/lib -lgtop-2.0 -ldl -lm -Wl,-rpath -Wl,/usr/local/lib -I /usr/include/libgtop-2.0 -I /usr/include/glib-2.0/ -I /usr/lib64/glib-2.0/include/ -I /usr/lib/arm-linux-gnueabihf/glib-2.0/include/ -I /usr/lib/glib-2.0/include/

clean :
	-rm E-cpuMem.epplet

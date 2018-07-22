CFLAGS=-g -Wall -fPIC -I/usr/local/include
LIBS=-L/usr/local/lib -lexpat

all: libParseMap

test: testParseMap
	./testParseMap map.xml

testParseMap: Makefile parseMap.h testParseMap.c
	gcc ${CFLAGS} -o testParseMap testParseMap.c ${LIBS} -lParseMap

libParseMap: Makefile parseMap.o parseMap.h
	gcc -shared -o libParseMap.so.1.0 parseMap.o ${LIBS}

parseMap.o: Makefile parseMap.h parseMap.c
	gcc ${CFLAGS} -c parseMap.c -o parseMap.o

install:
	cp libParseMap.so.1.0 /usr/local/lib
	ln -sf /usr/local/lib/libParseMap.so.1.0 /usr/local/lib/libParseMap.so.1
	ln -sf /usr/local/lib/libParseMap.so.1.0 /usr/local/lib/libParseMap.so
	ldconfig /usr/local/lib
	cp parseMap.h /usr/local/include/parseMap.h

clean:
	rm *.o; rm *.so*; rm core*; rm testParseMap

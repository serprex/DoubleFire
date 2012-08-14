CC=gcc
CFLAGS+=-g -std=gnu11 -O2 -pipe
O=tgen.o spr.o lv.o net.o foe.o df.o
all: ${O}
	${CC} ${O} -o DoubleFire!! -lglfw -lGL -lm
mingw:
	i486-mingw32-gcc ${CFLAGS} ${O:.o=.c} -msse2 -o DoubleFire!!.exe -lglfw -lopengl32 -lm -lws2_32
mac: ${O}
	clang ${O} -o DoubleFire!! -lglfw -lm -framework OpenGL
clean:
	rm -rf *.o
spr: spr/*
	pinch S
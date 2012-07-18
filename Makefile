CC=gcc -std=gnu11 -O2 -ffast-math -g -pipe
O=tgen.o spr.o lv.o net.o foe.o df.o
all: ${O}
	${CC} ${O} -o DoubleFire!! -lglfw -lGL -lm
mingw:
	i486-mingw32-${CC} ${O:.o=.c} -o DoubleFire!!.exe -lglfw -lopengl32 -lm -lws2_32
clean:
	rm -rf *.o
spr: spr/*
	pinch S
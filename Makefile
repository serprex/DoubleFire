CC=gcc -std=gnu11 -O2 -ffast-math -g -pipe
O=tgen.o spr.o lv.o net.o foe.o df.o
all: ${O}
	${CC} ${O} -o DoubleFire!! -lglfw -lGL -lm
clean:
	rm -rf *.o
spr: spr/*
	pinch S
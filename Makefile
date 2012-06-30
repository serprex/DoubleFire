CC=gcc -std=gnu11 -Ofast -g -pipe
O=tgen.o spr.o lv.o df.o
all: ${O}
	${CC} ${O} -o DoubleFire -lglfw -lGL -lm
clean:
	rm -rf *.o
spr: spr/*
	pinch S
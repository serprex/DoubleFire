#include "df.h"
#define PBIT (p?128:0)
static uint8_t Lv[65536];
uint8_t*Lp=Lv;
#define l(x) static void l##x(uint##x##_t a){*(uint##x##_t*)Lp=a;Lp+=x/8;}static int##x##_t rl##x(){Lp+=x/8;return*(int##x##_t*)(Lp-x/8);}
#define lt(t) static void l##t(t a){*(t*)Lp=a;Lp+=sizeof(t);}static t rl##t(){Lp+=sizeof(t);return*(t*)(Lp-sizeof(t));}
l(8)
l(16)
l(32)
lt(float)
void mke(){
	while(T==*(uint16_t*)Lp){
		float d,v;
		uint8_t*LL=Lp;
		Lp+=2;
		uint8_t t=rl8();
		w8(7);
		obje*e=Etop++;
		e->t=t;
		e->c=T;
		switch(t&127){
		default:printf("Unknown E%x\n",t);
		case(ECAN)
			e->h=5;
			e->x=rl16();
			e->y=rl16();
			v=rlfloat();
			e->d=rlfloat();
			e->xd=cos(e->d)*v;
			e->yd=-sin(e->d)*v;
		case(ETAR)
			e->h=32;
			e->x=rl16();
			e->y=rl16();
			e->xd=0;
			e->yd=M_PI;
		case(EB1)
			e->h=64;
			e->x=64;
			e->y=320;
			e->xd=0;
		case(EB2)
			memset(e->a+1,0,19);
		case(EROT)
			e->h=12;
			e->x=rl16();
			e->y=rl16();
			e->d=M_PI/2;
			v=rlfloat();
			d=rlfloat();
			e->xd=cos(d)*v;
			e->yd=-sin(d)*v;
		case(EDOG)
			e->h=16;
			e->x=rl16();
			e->y=rl16();
			e->xd=rlfloat();
			e->d=rlfloat();
		}
		w8(Lp-LL);
		w8(8);
	}
}
static void can(uint16_t t,int p,int16_t x,int16_t y,float v,float d){
	l16(t);
	l8(PBIT|ECAN);
	l16(x);
	l16(y);
	lfloat(v);
	lfloat(d);
}
static void tar(uint16_t t,int16_t x,int16_t y){
	l16(t);
	l8(ETAR);
	l16(x);
	l16(y);
}
static void b1(uint16_t t){
	l16(t);
	l8(EB1);
}
static void b2(uint16_t t){
	l16(t);
	l8(EB2);
}
static void rot(uint16_t t,int p,int16_t x,int16_t y,float v,float d){
	l16(t);
	l8(PBIT|EROT);
	l16(x);
	l16(y);
	lfloat(v);
	lfloat(d);
}
static void dog(uint16_t t,int16_t x,int16_t y,float v,float d){
	l16(t);
	l8(EDOG);
	l16(x);
	l16(y);
	lfloat(v);
	lfloat(d);
}
void genL1(){
	for(int i=0;i<5;i++){
		can(i*60+5,i&1,10,10,1,M_PI*3/2);
		can(i*60+15,!(i&1),118,246,1,M_PI/2);
	}
	tar(333,80,96);
	for(int i=0;i<5;i++){
		can(i*60+395,!(i&1),10,246,1.5,M_PI/2);
		can(i*60+400,i&1,118,10,1.5,M_PI*3/2);
	}
	tar(666,48,160);
	b1(721);
	for(int i=0;i<16;i++){
		rot(1000+i*60,i&1,i*8,-4,2,M_PI*3/2);
		if(i==8){
			dog(1000+i*66,32,64,1,0);
			dog(1000+i*66,96,64,1,M_PI);
		}
	}
	b2(2000);
	can(2060,0,64,128,0,M_PI*3/2);
	for(int i=0;i<5;i++){
		can(i*30+2100,0,64,i*20,1./i,M_PI*3/2);
	}
	printf("L1: %ld\n",Lp-Lv);
	Lp=Lv;
}
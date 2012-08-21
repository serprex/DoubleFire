#include "df.h"
static uint8_t Lv[65536];
uint8_t*Lp=Lv;
#define w(x) static void w##x(uint##x##_t a){*(uint##x##_t*)Lp=a;Lp+=x/8;}static int##x##_t r##x(){Lp+=x/8;return*(int##x##_t*)(Lp-x/8);}
#define wt(t) static void w##t(t a){*(t*)Lp=a;Lp+=sizeof(t);}static t r##t(){Lp+=sizeof(t);return*(t*)(Lp-sizeof(t));}
w(8)
w(16)
wt(float)
#define PBIT (p?128:0)
#define E(x) static void x(uint16_t t
#define P ,int p
#define XY ,int16_t x,int16_t y
#define VD ,float v,float d
#define T(x) ){w16(t);w8(x);
#define END }
#define WVD wfloat(v);wfloat(d);
#define WXY w16(x);w16(y);
#define RXY e->x=r16();e->y=r16();
#define RVD v=rfloat();d=rfloat();
#define RF2(v,d) v=rfloat();d=rfloat();
#define EXYV e->xd=cos(d)*v;e->yd=sin(d)*v;
E(can) XY VD T(CAN) WXY WVD END
E(tar) XY T(TAR) WXY END
E(b1) T(B1) END
E(b2) T(B2) END
E(b3) P T(PBIT|B3) END
E(rot) P XY VD T(PBIT|ROT) WXY WVD END
E(dog) XY VD T(DOG) WXY WVD END
E(poo) XY VD T(POO) WXY WVD END
void lvstep(){
	if(T==*(uint16_t*)Lp){
		float v,d;
		uint8_t*LL=Lp;
		Lp+=2;
		obje*e=Etop++;
		e->t=r8();
		e->c=T;
		switch(e->t&127){
		default:printf("Unknown E%x\n",e->t);
		case(CAN)
			e->h=5;
			RXY
			RVD
			EXYV
			e->d=d;
		case(TAR)
			e->h=32;
			RXY
			e->xd=0;
			e->yd=M_PI;
		case(ROT)
			e->h=12;
			e->d=M_PI/2;
			RXY
			RVD
			EXYV
		case(DOG)
			e->h=16;
			RXY
			RF2(e->xd,e->d)
		case(POO)
			e->h=16;
			RXY
			RF2(e->xd,e->d)
		case(B1)
			e->h=64;
			e->x=64;
			e->y=-64;
			e->xd=0;
		case(B2)
			memset(e->a+1,0,19);
		case(B3)
			e->h=127;
			e->x=e->t&128?32:96;
			e->y=e->t&128?-32:288;
			e->d=0;
		}
		marklp(Lp-LL);
	}
}
void genL1(){
	/*for(int i=0;i<5;i++){
		can(i*60+5,10,246,1,M_PI*3/2);
		can(i*60+15,118,10,1,M_PI/2);
	}
	tar(333,80,160);
	for(int i=0;i<5;i++){
		can(i*60+395,10,10,1.5,M_PI/2);
		can(i*60+400,118,246,1.5,M_PI*3/2);
	}
	tar(666,48,160);
	b1(721);
	for(int i=0;i<16;i++){
		rot(1000+i*60,i&1,i*8,260,2,M_PI*3/2);
		if(i==8){
			dog(1000+i*72,32,192,1,0);
			dog(1001+i*72,96,192,1,M_PI);
		}
	}
	b2(2000);
	can(2060,64,128,0,M_PI*3/2);
	for(int i=0;i<8;i++)
		poo(2100+i*30,64,128,0,M_PI*i/8);*/
	b3(30,0);
	b3(31,1);
	printf("L1: %ld\n",Lp-Lv);
	Lp=Lv;
}
#include "df.h"
#include <stdarg.h>
static uint8_t Lv[65536];
uint8_t*restrict Lp=Lv;
#define w(x) static void w##x(uint##x##_t a){*(uint##x##_t*)Lp=a;Lp+=x/8;}static int##x##_t r##x(){Lp+=x/8;return*(int##x##_t*)(Lp-x/8);}
#define wt(t) static void w##t(t a){*(t*)Lp=a;Lp+=sizeof(t);}static t r##t(){Lp+=sizeof(t);return*(t*)(Lp-sizeof(t));}
w(8)
w(16)
wt(float)
enum{P=1,XY=2,VD=4,RXYV=8,RXVD=16};
static const struct{uint8_t f,h;}eopt[]={
	[CAN]={XY|VD|RXYV,5},
	[TAR]={XY,32},
	[ROT]={P|XY|VD|RXYV,12},
	[DOG]={XY|VD|RXVD,16},
	[POO]={XY|VD|RXVD,16},
	[B1]={0,64},
	[B3]={P,127},
};
static void e(uint8_t t,uint16_t T,...){
	va_list vl;
	va_start(vl,T);
	w16(T);
	w8(((eopt[t]&P)&&!va_arg(vl,int)?128:0)|t);
	if(eopt[t]&XY){
		w16(va_arg(vl,int));
		w16(va_arg(vl,int));
	}
	if(eopt[t]&VD){
		wfloat(va_arg(vl,double));
		wfloat(va_arg(vl,double));
	}
	va_end(vl);
}
void lvstep(){
	if(T==*(uint16_t*)Lp){
		float v,d;
		uint8_t*LL=Lp;
		Lp+=2;
		obje*e=Etop;
		e->t=r8();
		e->c=T;
		e->h=eopt[e->t&127].h;
		uint8_t eo=eopt[e->t&127].f;
		if(eo&XY){
			e->x=r16();
			e->y=r16();
		}
		if(eo&VD){
			v=rfloat();
			d=rfloat();
			if(eo&RXYV){
				e->xd=cos(d)*v;
				e->yd=sin(d)*v;
			}else(eo&RXVD){
				e->xd=v;
				e->d=d;
			}
		}
		switch(e->t&127){
		case(CAN)
			e->d=d;
		case(TAR)
			e->xd=0;
			e->yd=M_PI;
		case(ROT)
			e->d=M_PI/2;
		case(B1)
			e->x=64;
			e->y=-64;
			e->xd=0;
		case(B2)
			memset(e->a+2,0,18);
		case(B3)
			e->x=e->t&128?32:96;
			e->y=e->t&128?-32:288;
			e->d=0;
		}
		marklp(Lp-LL);
	}else assert(T<*(uint16_t*)Lp||!*(uint16_t*)Lp);
}
void genL1(){
	for(int i=0;i<5;i++){
		e(CAN,i*60+5,10,246,1.,M_PI*3/2);
		e(CAN,i*60+15,118,10,1.,M_PI/2);
	}
	e(TAR,333,80,160);
	for(int i=0;i<5;i++){
		e(CAN,i*60+395,10,10,1.5,M_PI/2);
		e(CAN,i*60+400,118,246,1.5,M_PI*3/2);
	}
	e(TAR,666,48,160);
	e(B1,721);
	for(int i=0;i<16;i++){
		e(ROT,900+i*60,i&1,i*8,260,2.,M_PI*3/2);
		if(i==2||i==4)
			e(DOG,910+i*60,64,0,2.,M_PI/2);
		if(i==8){
			e(DOG,915+i*60,32,192,1.,0.);
			e(DOG,920+i*60,96,192,1.,M_PI);
		}
	}
	e(B2,2000);
	e(CAN,2060,64,128,0,M_PI*3/2);
	e(B3,3000,0);
	e(B3,3001,1);
	printf("L1: %ld\n",Lp-Lv);
	Lp=Lv;
}
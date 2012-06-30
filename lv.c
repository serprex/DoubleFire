#include <math.h>
#include <stdint.h>
unsigned char Lv[65536];
static void*Lp;
#define w(x) static void w##x(uint##x##_t a){*(uint##x##_t*)Lp=a;Lp+=x/8;}
#define wt(t,x) static void w##t(t a){*(t*)Lp=a;Lp+=x;}
w(8)
w(16)
w(32)
wt(float,4)
static void can(uint16_t t,int p,int16_t x,int16_t y,float v,float d)
{
	w16(t);
	w8(p?160:32);
	w16(x);
	w16(y);
	wfloat(d);
	wfloat(cos(d)*v);
	wfloat(-sin(d)*v);
}
static void tar(uint16_t t,int16_t x,int16_t y)
{
	w16(t);
	w8(33);
	w16(x);
	w16(y);
}
void genL1(){
	Lp=Lv;
	/*for(int i=0;i<10;i++){
		can(i*30+5,i&1,10,10,2,M_PI*3/2);
		if(i==6)tar(i*30+10,64,128);
		can(i*30+15,!(i&1),118,246,2,M_PI/2);
	}
	for(int i=0;i<10;i++){
		can(i*30+305,!(i&1),10,246,2,M_PI*3/2);
		if(i==6)tar(i*30+310,64,128);
		can(i*30+315,i&1,118,10,2,M_PI/2);
	}
	tar(700,48,160);*/
	tar(70,80,96);
}
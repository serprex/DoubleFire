#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <assert.h>
typedef const uint8_t*const colt;
extern uint16_t T,MT;
extern uint8_t*pin,Pf[2],Pi,Pt;
extern int8_t Pe;
extern int Ph[2];
extern uint16_t Php[2];
extern float Px[2],Py[2],Lzr[32][2];
extern int Lzo,Box,Boy,Bor;
extern const uint8_t col[13];
extern colt red,blu,wht,shr,shb,blk;
#include "lv.h"
#include "spr.h"
#include "net.h"
#include "foe.h"
#define case(x) break;case x:;
#define else(x) else if(x)
#define sqr(x) ({__typeof__(x) _x=(x);_x*_x;})
#define max(x,y) ({__typeof__(x) _x=(x);__typeof__(y) _y=(y);_x>_y?_x:_y;})
#define min(x,y) ({__typeof__(x) _x=(x);__typeof__(y) _y=(y);_x<_y?_x:_y;})
#define dst2(x1,y1,x2,y2) (sqr((x1)-(x2))+sqr((y1)-(y2)))
#define dst(x1,y1,x2,y2) sqrt(dst2(x1,y1,x2,y2))
#define dir(x1,y1,x2,y2) atan2((y2)-(y1),(x2)-(x1))
#define gZ(x) (!!((x)&1))
#define gX(x) (!!((x)&2))
#define gD(x) (!!((x)&4))
#define gA(x) (!!((x)&8))
#define gS(x) (!!((x)&16))
#define gW(x) (!!((x)&32))
#ifdef FIRE
typedef struct{
	uint8_t*p;
	int n;
}frame;
static frame*rw;
static int crw=-1,rwp,rwT,welt,cpi=-2,piT,pip;
#define w(x) void w##x(uint##x##_t a){rw[crw].p=realloc(rw[crw].p,rw[crw].n+x/8);*(uint##x##_t*)(rw[crw].p+rw[crw].n)=a;rw[crw].n+=x/8;}static uint##x##_t r##x(){rw[crw].n-=x/8;assert(rw[crw].n>=0);return*(uint##x##_t*)(rw[crw].p+rw[crw].n);}
#define wt(t) void w##t(t a){rw[crw].p=realloc(rw[crw].p,rw[crw].n+sizeof(t));*(t*)(rw[crw].p+rw[crw].n)=a;rw[crw].n+=sizeof(t);}static t r##t(){rw[crw].n-=sizeof(t);assert(rw[crw].n>=0);return*(t*)(rw[crw].p+rw[crw].n);}
#else
#define w(x) void w##x(uint##x##_t a);
#define wt(t) void w##t(t a);
#endif
w(8)
w(16)
w(32)
wt(float)
wt(obje)
wt(bxy)
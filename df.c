#include "df.h"
typedef struct{
	uint8_t*p;
	int n;
}frame;
static frame*rw;
static int crw=-1,rwp,rwT,welt,cpi=-2,piT,pip;
#define gZ(x) (!!((x)&1))
#define gX(x) (!!((x)&2))
#define gD(x) (!!((x)&4))
#define gA(x) (!!((x)&8))
#define gS(x) (!!((x)&16))
#define gW(x) (!!((x)&32))
static uint16_t T,MT,mnT,mxT,moT;
static const uint8_t col[]={255,0,0,255,255,255,63,47,95,255,0,0,0};
static colt red=col,blu=col+1,wht=col+3,shr=col+5,shb=col+7,blk=col+10;
float rnorm(float a){
	a=fmodf(a,M_PI*2);
	return a>M_PI?a-M_PI*2:a;
}
typedef struct{
	uint16_t p;
	float x,y,xd,yd;
}bxy;
typedef union{
	uint8_t a[24];
	struct{
		uint8_t t;
		int8_t h;
		uint16_t c;
		float x,y,xd,yd,d;
	};
}obje;
typedef enum{
	BMI,BEX,
	ECAN=32,ETAR,EB1,EB2,EROT
}oid;
#define w(x) static void w##x(uint##x##_t a){rw[crw].p=realloc(rw[crw].p,rw[crw].n+x/8);*(uint##x##_t*)(rw[crw].p+rw[crw].n)=a;rw[crw].n+=x/8;}static uint##x##_t r##x(){rw[crw].n-=x/8;assert(rw[crw].n>=0);return*(uint##x##_t*)(rw[crw].p+rw[crw].n);}
#define wt(t) static void w##t(t a){rw[crw].p=realloc(rw[crw].p,rw[crw].n+sizeof(t));*(t*)(rw[crw].p+rw[crw].n)=a;rw[crw].n+=sizeof(t);}static t r##t(){rw[crw].n-=sizeof(t);assert(rw[crw].n>=0);return*(t*)(rw[crw].p+rw[crw].n);}
w(8)
w(16)
w(32)
wt(float)
wt(obje)
wt(bxy)
bxy B[8192];
bxy*Btop=B-1;
obje E[64];
obje*Etop=E-1;
bxy PB[256];
bxy*PBtop=PB-1;
uint8_t*pin,Pf[2],Pi,*L=Lv;
int8_t Pe=127;
int Pt,Ph[2];
uint16_t Php[2];
float Px[2]={32,96},Py[2]={160,160},Lzr[32][2];
int Lzo,Box,Boy,Bor;
static void pushrw(){
	int shift=min(mnT,T)-rwT;
	if(shift>0){
		printf("<<%d %d %d\n",shift,rwp,rwT);
		crw-=shift;
		rwT+=shift;
		for(int i=0;i<shift;i++)
			free(rw[i].p);
		memmove(rw,rw+shift,sizeof(frame)*(rwp-=shift));
	}
	if(++crw==rwp){
		rw=realloc(rw,sizeof(frame)*++rwp);
		rw[crw].p=0;
		rw[crw].n=0;
	}
	shift=(min(min(mnT,T),moT)-piT)*2;
	if(shift>0){
		cpi-=shift;
		piT+=shift;
		memmove(pin,pin+shift,pip-=shift);
	}
	if((cpi+=2)==pip){
		pin=realloc(pin,pip+=2);
		memset(pin+cpi,0,2);
	}
}
void mkpb(uint8_t p,float x,float y,float xd,float yd){
	w8(19);
	bxy*b=++PBtop;
	b->p=p;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
}
void mkb(int p,float x,float y,float xd,float yd){
	w8(26);
	bxy*b=++Btop;
	b->p=p;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
}
void mkbd(int p,float x,float y,float v,float d){
	mkb(p,x,y,cos(d)*v,-sin(d)*v);
}
void mkbxy(int p,float x,float y,float xx,float yy,float v){
	xx-=x;
	yy-=y;
	float xy=sqrt(xx*xx+yy*yy);
	if(xy)mkb(p,x,y,xx*v/xy,yy*v/xy);
}
static obje*mke(uint8_t t){
	w8(7);
	obje*e=++Etop;
	e->t=t;
	e->c=T;
	return e;
}
void mkcan(uint8_t t,float x,float y,float d,float v){
	obje*e=mke(t);
	e->h=5;
	e->x=x;
	e->y=y;
	e->d=d;
	e->xd=cos(d)*v;
	e->yd=-sin(d)*v;
}
void mktar(uint8_t t,float x,float y){
	obje*e=mke(t);
	e->h=32;
	e->x=x;
	e->y=y;
	e->xd=0;
	e->yd=M_PI;
}
void mkb1(){
	obje*e=mke(EB1);
	e->h=64;
	e->x=64;
	e->y=320;
	e->xd=0;
}
void mkb2(){
	obje*e=mke(EB2);
	memset(e->a+1,0,19);
}
void mkrot(uint8_t t,float x,float y,float d,float v){
	obje*e=mke(t);
	e->h=12;
	e->x=x;
	e->y=y;
	e->d=M_PI/2;
	e->xd=cos(d)*v;
	e->yd=-sin(d)*v;
}
void xLz(int c,float x,float y,float d){
	if(T==MT)
		glTriangle(x,y,x+cos(d+M_PI/64)*c,y-sin(d+M_PI/64)*c,x+cos(d-M_PI/64)*c,y-sin(d-M_PI/64)*c);
	for(int i=0;i<2;i++)
		if(dst(x,y,Px[i],Py[i])<c&&fabsf(rnorm(d+dir(x,y,Px[i],Py[i])))<M_PI/64)Ph[i]--;
}
int rinpb(float x,float y,int r,int p){
	r*=r;
	int n=0;
	for(bxy*b=PB;b<=PBtop;b++)
		if(dst2(x,y,b->x,b->y)<=r&&b->p!=p){
			n++;
			wbxy(*b);
			w8(b-PB);
			w8(20);
			*b--=*PBtop--;
		}
	return n;
}
int rinlz(float x,float y,int r){
	return Lzo&&x>=Px[1]-r&&x<=Px[1]+r&&(y<=Py[1]||dst2(x,y,Px[1],Py[1])<=sqr(r));
}
int rinbo(float x,float y,int r){
	return Bor&&dst2(x,y,Box,Boy)<=sqr(r+Bor);
}
int rdmg(float x,float y,int r,int p){
	return rinpb(x,y,r,p)+(!p?0:rinbo(x,y,r))+(p==1?0:rinlz(x,y,r));
}
float dtop(int p,float x,float y){
	return dst2(x,y,Px[p],Py[p]);
}
float btop(int p,bxy*b){
	return dtop(p,b->x,b->y);
}
float rrot(float a,float b,float m){
	b=rnorm(b+a);
	if(fabsf(b)<=m)return a-b;
	return b>0?a-m:a+m;
}
void rrota(float*a,float b,float m){
	*a=rrot(*a,b,m);
}
void rrotxy(float*a,float x1,float y1,float x2,float y2,float m){
	rrota(a,dir(x1,y1,x2,y2),m);
}
void erotxy(obje*e,float x,float y,float m){
	wfloat(e->d);
	w8(e-E);
	w8(28);
	rrotxy(&e->d,e->x,e->y,x,y,m);
}
void stepBack(int n){
	printf("sb %d %d",n,crw);
	for(;;){
		while(rw[crw].n){
			uint8_t a=r8();
			switch(a){
			case(2 ... 3)Px[a-2]=rfloat();
			case(4 ... 5)Py[a-4]=rfloat();
			case(6)Pe=r8();
			case(7)Etop--;
			case(8)L-=r8();
			case(9){
				obje*e=E+r8();
				*++Etop=*e;
				*e=robje();
			}
			case(10 ... 11)Pf[a-10]=0;
			case(12)PBtop--;
			case(13){
				obje*e=E+r8();
				*++Etop=*e;
				*e=robje();
			}
			case(14)
				for(bxy*b=PB;b<=PBtop;b++){
					b->x-=b->xd;
					b->y-=b->yd;
				}
			case(15)Pi++;
			case(16)Pe--;
			case(17)
				Btop=B+r16();
				for(bxy*b=Btop;b>=B;b--)
					*b=rbxy();
			case 18:
				Pe+=48;
				Pi=0;
			case(19)PBtop--;
			case(20){
				bxy*b=PB+r8();
				*++PBtop=*b;
				*b=rbxy();
			}
			case(21){
				bxy*b=B+r16();
				*++Btop=*b;
				*b=rbxy();
			}
			case(22 ... 23)Pf[a-22]=r8();
			case(24)
				Px[0]=128-Px[0];
				Py[0]=256-Py[0];
				Bor=0;
			case(25)
				for(bxy*b=B;b<=Btop;b++){
					b->x-=b->xd;
					b->y-=b->yd;
				}
			case(26)Btop--;
			case(27)
				for(obje*e=E;e<=Etop;e++){
					switch(e->t&127){
					case(EROT)
						e->d-=M_PI/(e->t&128?128:-128);
					case ECAN:
						e->x-=e->xd;
						e->y-=e->yd;
					}
				}
			case(28){
				obje*e=E+r8();
				e->d=rfloat();
			}
			case(29){
				obje*e=E+r8();
				e->h=r8();
			}
			case(30){
				obje*e=E+r8();
				e->yd=rfloat();
				e->xd=rfloat();
			}
			case(31){
				obje*e=E+r8();
				e->y=r16();
			}
			case(32 ... 33)Lzo=a-32;
			case(34){
				obje*e=E+r8();
				int xy=r8(),x=xy-1&3,y=xy-1>>2;
				e->a[xy]^=1;
				if(x<3)e->a[xy+1]^=1;
				if(x>0)e->a[xy-1]^=1;
				if(y<3)e->a[xy+4]^=1;
				if(y>0)e->a[xy-4]^=1;
			}
			case(35 ... 36)E[r8()].a[a-17]--;
			case(37)Bor=r8();
			case(38)E[r8()].h++;
			case(39)
				memmove(Lzr,Lzr+1,248);
				Lzr[31][1]=rfloat();
				Lzr[31][0]=rfloat();
			case(40)Pe++;
			}
		}
		T--;
		crw--;
		cpi-=2;
		if(!--n||crw==-1){
			printf(":%d\n",crw);
			return;
		}
	}
}
int main(int argc,char**argv){
	sprInit();
	genL1();
	Pt=atoi(argv[1]);
	netinit(argv[2]);
	for(;;){
		nsend(0,0);
		if(any()){
			nrecv(0,0);
			break;
		}
		sprInput();
		sprBeginFrame();
		glRecti(0,0,320,512);
		sprEndFrame(0);
	}
	for(;;){
		pushrw();
		if(T==MT)sprBeginFrame();
		if(Pe<0){
			wfloat(Px[0]);
			w8(2);
			wfloat(Px[1]);
			w8(3);
			w8(Pe);
			w8(6);
			Px[0]=Px[1]=20;
			Pe=0;
		}
		while(T==*(uint16_t*)L){
			uint8_t*LL=L;
			L+=2;
			switch(*L++&127){
			default:printf("Unknown E%x\n",L[-1]);
			case(ECAN)
				mkcan(L[-1],*(int16_t*)L,*(int16_t*)(L+2),*(float*)(L+4),*(float*)(L+8));
				L+=12;
			case(ETAR)
				mktar(L[-1],*(uint16_t*)L,*(uint16_t*)(L+2));
				L+=4;
			case(EB1)
				mkb1();
			case(EB2)
				mkb2();
			case(EROT)
				mkrot(L[-1],*(int16_t*)L,*(int16_t*)(L+2),*(float*)(L+4),*(float*)(L+8));
				L+=12;
			}
			w8(L-LL);
			w8(8);
		}
		if(T==MT){
			printf("--%d\n",T);
			int len=3;
			uint8_t pcm[5];
			*(uint16_t*)pcm=T;
			pcm[2]=pin[cpi+Pt]=sprInput();
			if(mxT-mnT>4){
				len+=2;
				*(uint16_t*)(pcm+3)=mnT;
			}
			nsend(pcm,len);
			notex();
			disableBlend();
		}
		for(int i=0;i<2;i++){
			float Pxx=(gD(pin[cpi+i])-gA(pin[cpi+i]))*1.5,Pyy=(gS(pin[cpi+i])-gW(pin[cpi+i]))*1.5;
			if(Pxx&&Pyy){
				Pxx*=M_SQRT2;
				Pyy*=M_SQRT2;
			}
			if(Pxx){
				wfloat(Px[i]);
				w8(2+i);
				Px[i]=fminf(fmaxf(Px[i]+Pxx,8),120);
			}
			if(Pyy){
				wfloat(Py[i]);
				w8(4+i);
				Py[i]=fminf(fmaxf(Py[i]+Pyy,8),248);
			}
			if(Pf[i]){
				w8(Pf[i]);
				w8(22+i);
				if(Pf[i]<8)Pf[i]++;
				else Pf[i]=8+(Pf[i]+1&7);
				if(Pe&&!(Pf[i]&7)){
					w8(40);
					Pe--;
					if(i){
						mkpb(1,Px[1],Py[1]-3,0,-3);
					}else{
						mkpb(0,Px[0],Py[0],0,4);
						float xd=64-Px[0],yd=128-Py[0],xy=sqrt(xd*xd+yd*yd)?:1;
						mkpb(0,Px[0],Py[0],xd*3/xy,yd*3/xy);
					}
				}
			}
			if(gZ(pin[cpi+i])){
				if(!Pf[i]){
					w8(10+i);
					Pf[i]=1;
				}
			}else{
				w8(Pf[i]);
				w8(22+i);
				Pf[i]=0;
			}
			if(gX(pin[cpi+i])){
				if(i){
					if(!Lzo){
						w8(32);
						Lzo=1;
						for(int i=0;i<32;i++){
							Lzr[i][0]=Px[1]+3;
							Lzr[i][1]=Py[1]+1;
						}
					}
				}else(!Bor){
					w8(24);
					Px[0]=128-Px[0];
					Py[0]=256-Py[0];
					Bor=1;
					Box=128-Px[0];
					Boy=256-Py[0];
				}
			}else(Lzo&&i){
				w8(33);
				Lzo=0;
			}
		}
		if(Bor){
			w8(Bor);
			w8(37);
			if(++Bor>24)Bor=0;
		}
		if(Lzo){
			wfloat(Lzr[31][0]);
			wfloat(Lzr[31][1]);
			w8(39);
			memmove(Lzr+1,Lzr,248);
			Lzr[0][0]=Px[1]+(rand()%3);
			Lzr[0][1]=Py[1]-3+(rand()&3);
		}
		if(PBtop>=PB)w8(14);
		for(bxy*b=PB;b<=PBtop;b++){
			b->x+=b->xd;
			b->y+=b->yd;
			if(b->y<0||b->x<0||b->x>128||b->y>256){
				wbxy(*b);
				w8(b-PB);
				w8(20);
				*b--=*PBtop--;
			}else(T==MT){
				glColor(red+b->p);
				glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
				glColor(b->p?wht:blk);
				glRectf(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
			}
		}
		Ph[0]=Ph[1]=2;
		Php[0]=Php[1]=0;
		if(T==MT)rndcol();
		if(Btop>=B)w8(25);
		for(bxy*b=B;b<=Btop;b++){
			float bx=b->x,by=b->y;
			b->x+=b->xd;
			b->y+=b->yd;
			if(T==MT){
				glLine(bx,by,b->x,b->y);
				glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
			}
			if(b->y<0||b->x<0||b->x>128||b->y>256)goto killb;
			for(int i=0;i<2;i++)
				if(btop(i,b)<256){
					if(Pe<127){
						w8(16);
						Pe++;
					}
					if(Php[i]!=b->p){
						Ph[i]--;
						Php[i]=b->p;
					}
					goto killb;
				}
			if(0){killb:
				wbxy(*b);
				w16(b-B);
				w8(21);
				*b--=*Btop--;
			}
		}
		if(Etop>=E)w8(27);
		for(obje*e=E;e<=Etop;e++){
			float r;
			int et=!!(e->t&128);
			switch(e->t&127){
			case(ECAN)
				if(T==MT){
					float r=min(T-e->c,e->h);
					glColor(wht);
					glLine(e->x,e->y,e->x+cos(e->d)*r*2,e->y-sin(e->d)*r*2);
					glColor(red+et);
					glCirc(e->x,e->y,r);
				}
				e->x+=e->xd;
				e->y+=e->yd;
				erotxy(e,Px[et],Py[et],M_PI/16);
				if(!(T-e->c&7)||!(T-e->c&3))
					mkbd(e->c,e->x+cos(e->d)*r*2,e->y-sin(e->d)*r*2,6,e->d);
				if(e->x<-5||e->x>133||e->y<-5||e->y>261||e->h<1)goto kille;
				else(e->h<4||rdmg(e->x,e->y,e->h*3/2,2)){
					w8(e-E);
					w8(38);
					e->h--;
				}
			case(ETAR)
				r=min(T-e->c,abs(e->h));
				if(T==MT){
					glColor(wht);
					glCirc(e->x,e->y,r);
					rndcol();
					glCirc(e->x,e->y,min(r,24));
					glColor(wht);
					glCirc(e->x,e->y,min(r,16));
					rndcol();
					glCirc(e->x,e->y,min(r,8));
				}
				et=e->h<6?4:rdmg(e->x,e->y,r,2);
				if(et){
					w8(e->h);
					w8(e-E);
					w8(29);
					e->h-=et;
				}
				wfloat(e->xd);
				wfloat(e->yd);
				w8(e-E);
				w8(30);
				rrotxy(&e->xd,e->x,e->y,Px[0],Py[0],M_PI/64);
				rrotxy(&e->yd,e->x,e->y,Px[1],Py[1],M_PI/64);
				for(int i=0;i<2;i++)
					xLz(min(T-e->c,120+e->h),e->x,e->y,i?e->yd:e->xd);
				if(e->h<-120)goto kille;
			case(EB1)
				r=e->h;
				w16(e->y);
				w8(e-E);
				w8(31);
				e->y-=e->y>192?2:0;
				if(T==MT){
					for(double a=0;a<M_PI;a+=M_PI/48){
						double aa=a+T/256.;
						rndrndcol();
						glLineC(e->x+cos(aa)*r,e->y+sin(aa)*r,e->x-cos(aa)*r,e->y-sin(aa)*r,red+!(e->h&8));
					}
				}
				mkbxy(e->c,e->x,e->y,Px[0],Py[0],4);
				mkbxy(e->c+1,e->x,e->y,Px[1],Py[1],4);
				mkbxy(e->c+2,e->x,e->y,Px[!(e->h&8)],Py[!(e->h&8)],1);
				if(rdmg(e->x,e->y,e->h,!(e->h&8))){
					w8(e->h);
					w8(e-E);
					w8(29);
					e->h++;
				}else(rdmg(e->x,e->y,e->h,!!(e->h&8))||e->h<7){
					w8(e-E);
					w8(38);
					e->h--;
					if(!e->h)goto kille;
				}
			case(EB2)
				for(int y=0;y<4;y++)
					for(int x=0;x<4;x++){
						int xx=28+x*(e->a[18]/2-e->a[19]),yy=92+y*(e->a[18]/2-e->a[19]),xy=x+y*4+1;
						if((!(T+x+y*3&15))&&(x==0&&y==0||x==3&&y==0||x==3&&y==3||x==0&&y==3))
							mkbxy(e->c+x+y*4,xx,yy,Px[e->a[xy]],Py[e->a[xy]],1);
						if(rdmg(xx,yy,8,!e->a[xy])){
							w8(xy);
							w8(e-E);
							w8(34);
							e->a[xy]^=1;
							if(x<3)e->a[xy+1]^=1;
							if(x>0)e->a[xy-1]^=1;
							if(y<3)e->a[xy+4]^=1;
							if(y>0)e->a[xy-4]^=1;
						}
						if(T==MT){
							glColor(red+e->a[xy]);
							glCirc(xx,yy,e->a[18]/4-e->a[19]/2);
						}
					}
				et=0;
				for(int i=0;i<16;i++)
					if(!e->a[i+1]){
						et=1;
						break;
					}
				if(et){
					if(e->a[18]<48){
						w8(e-E);
						w8(35);
						e->a[18]++;
					}
				}else{
					w8(e-E);
					w8(36);
					e->a[19]++;
					if(e->a[19]==24)goto kille;
				}
			case(EROT)
				e->x+=e->xd;
				e->y+=e->yd;
				e->d+=M_PI/(et?128:-128);
				for(int i=0;i<3;i++)
					mkbd(e->c,e->x+cos(e->d+i*M_PI*2/3)*32,e->y-sin(e->d+i*M_PI*2/3)*32,8,e->d+i*M_PI*2/3);
				if(T==MT){
					rndcol();
					glCirc(e->x,e->y,e->h);
					glColor(wht);
					for(int i=0;i<3;i++)
						glLine(e->x+cos(e->d+i*M_PI*2/3)*(32-e->h*3),e->y-sin(e->d+i*M_PI*2/3)*(32-e->h*3),e->x+cos(e->d+i*M_PI*2/3)*32,e->y-sin(e->d+i*M_PI*2/3)*32);
				}
				if(e->x<-5||e->x>133||e->y<-5||e->y>261||e->h<1)goto kille;
				else(e->h<8||rdmg(e->x,e->y,e->h*3/2,2)){
					w8(e-E);
					w8(38);
					e->h--;
				}
			}
			if(0)kille:{
				wobje(*e);
				w8(e-E);
				w8(9);
				*e--=*Etop--;
			}
		}
		if(Pi){
			w8(15);
			Pi--;
		}else{
			if(Lzo&&Ph[1]==1)Ph[1]=0;
			for(int i=0;i<2;i++)
				if(Ph[i]==1&&Pe<127){
					w8(16);
					Pe++;
				}else(Ph[i]<1){
					if(Btop>=B){
						for(bxy*b=B;b<=Btop;b++)
							wbxy(*b);
						w16(Btop-B);
						w8(17);
						Btop=B-1;
					}else w8(18);
					Pe-=48;
					Pi=96;
				}
		}
		if(T==MT){
			if(Bor){
				glColor(wht);
				glCirc(Box,Boy,Bor);
			}
			rndcol();
			glRecti(128,0,136,Pe*2);
			glRecti(136,64,144,64+T-mnT);
			glRecti(144,64,152,64+T-mxT);
			enableBlend();
			if(Lzo)glLzr(Lzr);
			retex();
			drawSpr(Kae,Px[0]-3,Py[0]-4,(Pf[0]&7)>3,shr);
			drawSpr(Ika,Px[1]-3,Py[1]-4,Pf[1]>3,shb);
			sprEndFrame(mxT>T);
			MT++;
		}
		T++;
		while(any()){
			uint8_t ubu[6];
			int len=nrecv(ubu,5);
			uint8_t oin=ubu[2];
			if(!len){
				if(++welt&1)nsend(0,0);
			}else(len==1){
				return 0;
			}else(len>=3){
				uint16_t mt=*(uint16_t*)ubu;
				printf("udp%d: %d %d\n",len,mt,oin);
				if((mt-piT)*2>=pip){
					int hip=pip;
					pin=realloc(pin,pip=(mt-piT+1)*2);
					memset(pin+hip,oin,pip-hip);
				}
				if(!(pin[(mt-piT)*2+!Pt]&128)){
					if(len==5){
						uint16_t rt=*(uint16_t*)(ubu+3);
						if(rt>=moT){
							moT=rt;
							ubu[5]=pin[(rt-piT)*2+Pt];
							printf("ubu5pin %d\n",(rt-piT)*2+Pt);
							nsend(ubu+3,3);
						}
					}
					pin[(mt-piT)*2+!Pt]=oin|128;
					if(mt<T){
						for(int i=(1+mt-piT)*2+!Pt;i<pip;i+=2){
							if(pin[i]&128)break;
							pin[i]=oin;
						}
						stepBack(T-mt);
					}
					if(mt==mnT)mnT++;
					if(mt>mxT)mxT=mt;
				}
			}
		}
	}
}
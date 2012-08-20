#include "df.h"
uint16_t T,MT;
static uint16_t mnT,mxT,moT;
typedef struct{
	uint8_t*p;
	int n;
}frame;
static frame*rw;
static int crw=-1,rwp,rwT,welt,cpi=-2,piT,pip,mlen;
static uint8_t*pin,*mbuf;
#define w(x) static void w##x(uint##x##_t a){rw[crw].p=realloc(rw[crw].p,rw[crw].n+x/8);*(uint##x##_t*)(rw[crw].p+rw[crw].n)=a;rw[crw].n+=x/8;}static uint##x##_t r##x(){rw[crw].n-=x/8;assert(rw[crw].n>=0);return*(uint##x##_t*)(rw[crw].p+rw[crw].n);}
#define wt(t) static void w##t(t a){rw[crw].p=realloc(rw[crw].p,rw[crw].n+sizeof(t));*(t*)(rw[crw].p+rw[crw].n)=a;rw[crw].n+=sizeof(t);}static t r##t(){rw[crw].n-=sizeof(t);assert(rw[crw].n>=0);return*(t*)(rw[crw].p+rw[crw].n);}
w(8)
w(16)
wt(float)
wt(obje)
wt(bxy)
static void stepBack(int n){
	for(;;){
		while(rw[crw].n){
			uint8_t a=r8();
			switch(a){
			default:__builtin_unreachable();
			case(2 ... 3)Px[a-2]=rfloat();
			case(4 ... 5)Py[a-4]=rfloat();
			case(6)Pe=r8();
			case(7)Pe++;
			case(8)
				Etop--;
				Lp-=r8();
			case(9){
				obje*e=E+r8();
				*Etop++=*e;
				*e=robje();
			}
			case(10 ... 11)Pf[a-10]=0;
			case(12 ... 13)Pf[a-12]=r8();
			case(14)
				for(bxy*b=PB;b<PBtop;b++){
					b->x-=b->xd;
					b->y-=b->yd;
				}
			case(15)Pi++;
			case(16)Pe--;
			case(17)PBtop--;
			case(18 ... 19)E[r8()].a[a]--;
			case(20){
				bxy*b=PB+r8();
				*PBtop++=*b;
				*b=rbxy();
			}
			case(21){
				bxy*b=B+r16();
				*Btop++=*b;
				*b=rbxy();
			}
			case(22 ... 23)Lzo=a-32;
			case(24)
				Boy=r8();
				Box=r8();
				Bor=0;
				Px[0]=128-Px[0];
				Py[0]=256-Py[0];
			case(25)
				for(obje*e=E;e<Etop;e++){
					switch(e->t&127){
					case(ROT)
						e->d-=M_PI/(e->t&128?128:-128);
					case CAN:
						e->x-=e->xd;
						e->y-=e->yd;
					}
				}
				for(bxy*b=B;b<Btop;b++){
					b->x-=b->xd;
					b->y-=b->yd;
				}
			case(26)Btop--;
			case(27){
				obje*e=E+r8();
				e->y=rfloat();
				e->x=rfloat();
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
			case(31)E[r8()].y-=2;
			case(32)
				Btop=B+r16();
				for(bxy*b=Btop-1;;b--){
					*b=rbxy();
					if(b==B)break;
				}
			case 33:
				Pe+=48;
				Pi=0;
			case(34){
				obje*e=E+r8();
				int xy=r8(),x=xy-1&3,y=xy-1>>2;
				e->a[xy]^=1;
				if(x<3)e->a[xy+1]^=1;
				if(x>0)e->a[xy-1]^=1;
				if(y<3)e->a[xy+4]^=1;
				if(y>0)e->a[xy-4]^=1;
			}
			case(35)Bor=r8();
			case(36)E[r8()].h++;
			}
		}
		assert(T);
		T--;
		crw--;
		cpi-=2;
		if(!--n)return;
		assert(crw!=-1&&cpi!=-2);
	}
}
void rwBegin(int isudp){
	int shift=min(mnT,T)-rwT;
	if(shift>0){
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
	shift=min(T,isudp?min(mnT,moT):mnT)*2-piT;
	if(shift>0){
		cpi-=shift;
		piT+=shift;
		memmove(pin,pin+shift,pip-=shift);
	}
	if((cpi+=2)==pip){
		pin=realloc(pin,pip+=2);
		memset(pin+cpi,cpi?pin[cpi-2+!Pt]&127:0,2);
	}
	printf("%s%d %.2x:%.2x  %.2x:%.2x %d:%d:%d %d,%d %d,%d   %d:%d:%d\n",T==MT?"==":"-",T,pin[cpi],pin[cpi+1],Pe,Pi,Btop-B,Etop-E,PBtop-PB,(int)Px[0],(int)Py[0],(int)Px[1],(int)Py[1],mnT,moT,mxT);
}
void rwInput(int isudp){
	if(isudp){
		*(uint16_t*)(mbuf)=mnT;
		*(uint16_t*)(mbuf+2)=T;
		mbuf[4]=pin[cpi+Pt]=sprInput();
		nsend(mbuf,mlen);
		mlen=5;
	}else{
		pin[cpi+Pt]=sprInput();
		nsend(pin+cpi+Pt,1);
	}
}
void setBor(int b){
	w8(Bor);
	w8(35);
	Bor=b;
}
void setPx(int i,float x){
	wfloat(Px[i]);
	w8(2+i);
	Px[i]=x;
}
void setPy(int i,float y){
	wfloat(Py[i]);
	w8(4+i);
	Py[i]=y;
}
void setPf(int i,uint8_t f){
	w8(Pf[i]);
	w8(12+i);
	Pf[i]=f;
}
void onPf(int i){
	w8(10+i);
	Pf[i]=1;
}
void setPe(int8_t e){
	w8(Pe);
	w8(6);
	Pe=e;
}
void decPe(){
	w8(7);
	Pe--;
}
void incPe(){
	w8(16);
	Pe++;
}
void onLzo(){
	w8(22);
	Lzo=1;
}
void offLzo(){
	w8(23);
	Lzo=0;
}
void onBor(){
	w8(Box);
	w8(Boy);
	w8(24);
	Bor=1;
	Box=Px[0];
	Boy=Py[0];
	Px[0]=128-Px[0];
	Py[0]=256-Py[0];
}
void decPi(){
	w8(15);
	Pi--;
}
void plHit(){
	if(Btop>B){
		for(bxy*b=B;b<Btop;b++)
			wbxy(*b);
		w16(Btop-B);
		w8(32);
		Btop=B;
	}else w8(33);
	Pe-=48;
	Pi=64;
}
void deceh(obje*e){
	w8(e-E);
	w8(36);
	e->h--;
}
void seteh(obje*e,int8_t h){
	w8(e->h);
	w8(e-E);
	w8(29);
	e->h=h;
}
void setexy(obje*e,float x,float y){
	wfloat(e->x);
	wfloat(e->y);
	w8(e-E);
	w8(27);
	e->x=x;
	e->y=y;
}
void add2ey(obje*e){
	w8(e-E);
	w8(31);
	e->y+=2;
}
void incPB(){
	w8(17);
	PBtop++;
}
void incB(){
	w8(26);
	Btop++;
}
void incE(uint8_t l){
	w8(l);
	w8(8);
	Etop++;
}
void marke(){
	w8(25);
}
void markpb(){
	w8(14);
}
int killpb(bxy*b){
	wbxy(*b);
	w8(b-PB);
	w8(20);
	if(b==--PBtop)return 1;
	*b=*PBtop;
	return 0;
}
int killb(bxy*b){
	wbxy(*b);
	w16(b-B);
	w8(21);
	if(b==--Btop)return 1;
	*b=*Btop;
	return 0;
}
int kille(obje*e){
	wobje(*e);
	w8(e-E);
	w8(9);
	if(e==--Etop)return 1;
	*e=*Etop;
	return 0;
}
void seted(obje*e,float d){
	wfloat(e->d);
	w8(e-E);
	w8(28);
	e->d=d;
}
void setexdyd(obje*e,float x,float y){
	wfloat(e->xd);
	wfloat(e->yd);
	w8(e-E);
	w8(30);
	e->xd=x;
	e->yd=y;
}
void eb2xy(obje*e,int xy,int x,int y){
	w8(xy);
	w8(e-E);
	w8(34);
	e->a[xy]^=1;
	if(x<3)e->a[xy+1]^=1;
	if(x>0)e->a[xy-1]^=1;
	if(y<3)e->a[xy+4]^=1;
	if(y>0)e->a[xy-4]^=1;
}
void ince18(obje*e){
	w8(e-E);
	w8(18);
	e->a[18]++;
}
void ince19(obje*e){
	w8(e-E);
	w8(19);
	e->a[19]++;
}
void rwDrawLag(int isudp){
	glRect(136,64,144,64+T-mxT);
	glRect(144,64,152,64+T-mnT);
	if(isudp)glRect(152,64,160,64+T-moT);
}
void rwInit(){
	mbuf=malloc(mlen=5);
}
void rwEnd(int isudp){
	while(any()){
		if(isudp){
			int len=psize();
			printf("udp%d",len);
			if(!len){
				printf("\n");
				exit(0);
			}else(len==1){
				if(++welt&1)nsend(&Pt,1);
				nrecv(0,0);
				printf("\n");
				continue;
			}
			assert(len>=5);
			uint8_t p[len];
			struct{uint16_t t;uint8_t c;}__attribute__((packed))*m=(void*)p;
			nrecv(p,len);
			if(m->t*2<piT||m->t*2+2>piT+pip)
				printf(" !!t2 outofrange%d:%d:%d ",m->t*2,piT,piT+pip);
			if(m->t>=moT){
				moT=m->t;
				printf(" %d",moT);
				if(moT<T&&mlen<512){
					for(int i=2;i<mlen;i+=3)
						if(*(uint16_t*)(mbuf+i)==moT)
							goto rereq;
					mbuf=realloc(mbuf,mlen+=3);
					*(uint16_t*)(mbuf+mlen-3)=moT;
					mbuf[mlen-1]=pin[moT*2-piT+Pt];
				}
			}
			rereq:m=(void*)(p+2);
			len-=2;
			assert(!(len%3));
			do{
				printf(" %d:%.2x",m->t,m->c);
				if(m->t*2<piT){
					printf(" t2<piT%d:%d:%d",m->t*2,piT,piT+pip);
					goto nextm;
				}
				if(m->t*2+2-piT>pip){
					int hip=pip;
					pin=realloc(pin,pip=m->t*2+2-piT);
					memset(pin+hip,m->c,pip-hip);
				}else(m->t<mnT||(pin[m->t*2-piT+!Pt]&128)){
					printf(" Already%d:%d",m->t,mnT);
					goto nextm;
				}
				if(m->t<T&&(pin[m->t*2-piT+!Pt]&127)!=m->c){
					for(int i=m->t*2+2-piT+!Pt;i<pip;i+=2){
						if(pin[i]&128)break;
						pin[i]=m->c&48;
					}
					stepBack(T-m->t);
				}
				pin[m->t*2-piT+!Pt]=m->c|128;
				if(m->t>mxT)mxT=m->t;
				if(m->t==mnT)
					do mnT++; while(mnT*2-piT<pip&&(pin[mnT*2-piT+!Pt]&128));
			nextm:
				len-=3;
				m++;
			}while(len);
			printf("\n");
		}else{
			uint8_t in;
			if(nrecv(&in,1)==-1)exit(0);
			if(mnT*2+2-piT>pip){
				int hip=pip;
				pin=realloc(pin,pip=mnT*2+2-piT);
				memset(pin+hip,in,pip-hip);
			}
			if(pin[mnT*2-piT+!Pt]!=in){
				pin[mnT*2-piT+!Pt]=in|128;
				if(mnT<T){
					for(int i=mnT*2+2-piT+!Pt;i<pip;i+=2)
						pin[i]=in;
					stepBack(T-mnT);
				}
			}
			mxT=mnT++;
		}
	}
}
double rwfps(){
	return mxT>T?0:1./30;
}
int gpin(int i){
	return pin[cpi+i];
}
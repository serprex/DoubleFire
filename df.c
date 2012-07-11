#include "df.h"
typedef struct{
	uint8_t*p;
	int n;
}frame;
frame*rw;
int crw=-1,rwp,rwT;
#define getZ(x) (!!((x)&1))
#define getX(x) (!!((x)&2))
#define getD(x) (!!((x)&4))
#define getA(x) (!!((x)&8))
#define getS(x) (!!((x)&16))
#define getW(x) (!!((x)&32))
int udp,tcp;
uint16_t T,MT,oT,mnT;
static uint8_t oTs[8192],rcol[3],wellc;
static const uint8_t col[]={255,0,0,255,255,255,127,63,127,255,0},
	*red=col,*blu=col+1,*wht=col+3,*shr=col+5,*shb=col+7;
void rndcol(){
	glColor3ubv(rcol);
}
float rnorm(float a){
	a=fmodf(a,M_PI*2);
	return a>M_PI?a-M_PI*2:a;
}
int any(int s){
	struct pollfd pfd={.fd=s,.events=POLLIN};
	return poll(&pfd,1,0);
}
int readch(int s){
	uint8_t c;
	ssize_t a=read(s,&c,1);
	return a?c:-1;
}
typedef struct{
	uint16_t p;
	float x,y,xd,yd;
}bxy;
typedef union{
	uint8_t a[20];
	struct{
		uint8_t t;
		int8_t h;
		uint16_t c;
		float x,y,d,xd,yd;
	};
}obje;
typedef enum{
	BMI,BEX,
	ECAN=32,ETAR,EB1,EB2
}oid;
static void pushrw(){
	crw++;
	printf(">>%d\n",crw);
	if(crw==rwp){
		rw=realloc(rw,sizeof(*rw)*++rwp);
		rw[crw].p=0;
		rw[crw].n=0;
	}
}
static void shiftrw(){
	rwT++;
	crw--;
	printf("<<%d\n",rwT);
	free(rw->p);
	memmove(rw,rw+1,sizeof(*rw)*--rwp);
}
#define w(x) static void w##x(uint##x##_t a){rw[crw].p=realloc(rw[crw].p,rw[crw].n+x/8);*(uint##x##_t*)(rw[crw].p+rw[crw].n)=a;rw[crw].n+=x/8;}static uint##x##_t r##x(){rw[crw].n-=x/8;return*(uint##x##_t*)(rw[crw].p+rw[crw].n);}
#define wt(t) static void w##t(t a){rw[crw].p=realloc(rw[crw].p,rw[crw].n+sizeof(t));*(t*)(rw[crw].p+rw[crw].n)=a;rw[crw].n+=sizeof(t);}static t r##t(){rw[crw].n-=sizeof(t);return*(t*)(rw[crw].p+rw[crw].n);}
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
uint8_t Pc[65536][2],Pf[2],Pi,*L=Lv;
int8_t Pe=127;
int Pt,Ph[2];
uint16_t Php[2];
float Px[2]={32,96},Py[2]={160,160},Lzr[32][2];
int Lzo,Box,Boy,Bor;
void mkpb(uint8_t p,float x,float y,float xd,float yd){
	w8(19);
	bxy*b=++PBtop;
	b->p=p;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
}
void mkLzo(){
	if(!Lzo){
		Lzo=1;
		for(int i=0;i<32;i++){
			Lzr[i][0]=Px[1]+3;
			Lzr[i][1]=Py[1]+1;
		}
	}
}
void mkBor(){
	if(!Bor){
		Bor=1;
		Box=128-Px[0];
		Boy=256-Py[0];
	}
}
void xDie(){
	w16(Btop-B);
	w8(17);
	Btop=B;
	Pe-=48;
	Pi=96;
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
void mkbd(int p,float x,float y,float d,float v){
	mkb(p,x,y,cos(d)*v,-sin(d)*v);
}
void mkbxy(int p,float x,float y,float xx,float yy,float v){
	xx=xx-x;
	yy=yy-y;
	float xy=sqrt(xx*xx+yy*yy)?:1;
	mkb(p,x,y,xx*v/xy,yy*v/xy);
}
static obje*mke(uint8_t t){
	w8(7);
	obje*e=++Etop;
	e->t=t;
	e->c=T;
	return e;
}
void mkcan(uint8_t t,float x,float y,float d,float xd,float yd){
	obje*e=mke(t);
	e->h=5;
	e->x=x;
	e->y=y;
	e->d=d;
	e->xd=xd;
	e->yd=yd;
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
	memset(e->a+1,0,18);
}
void xLz(int c,float x,float y,float d){
	if(T==MT){
		glBegin(GL_TRIANGLES);
		glVertex2f(x,y);
		glVertex2f(x+cos(d+M_PI/64)*c,y-sin(d+M_PI/64)*c);
		glVertex2f(x+cos(d-M_PI/64)*c,y-sin(d-M_PI/64)*c);
		glEnd();
	}
	for(int i=0;i<2;i++)
		if(dst(x,y,Px[i],Py[i])<c&&fabsf(rnorm(d+dir(x,y,Px[i],Py[i])))<M_PI/64)Ph[i]--;
}
int rinpb(float x,float y,int r,int p){
	r*=r;
	int n=0;
	for(bxy*b=PB;b<=PBtop;b++)
		if(dst2(x,y,b->x,b->y)<=r&&b->p!=p){
			n++;
			*b--=*PBtop--;
		}
	return n;
}
int rinlz(float x,float y,int r){
	return Lzo&&y<=Py[1]&&x>=Px[1]-4&&x<=Px[1]+4;
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
	w8(Etop-E);
	w8(28);
	rrotxy(&e->d,e->x,e->y,x,y,m);
}
void stepBack(int n){
	printf("sb %d %d",n,crw);
	for(;;){
		while(rw[crw].n){
			uint8_t a;
			switch(a=r8()){
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
			case(10 ... 11)Pf[a-11]=0;
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
			case(17)Btop=B+r16();
			case(18)Pe++;
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
			case(25)
				for(bxy*b=B;b<=Btop;b++){
					b->x-=b->xd;
					b->y-=b->yd;
				}
			case(26)Btop--;
			case(27)
				for(obje*e=E;e<=Etop;e++){
					switch(e->t&127){
					case(ECAN)
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
			}
		}
		T--;
		crw--;
		if(!--n||crw==-1){
			printf(":%d\n",crw);
			return;
		}
	}
}
GLuint Stx;
void notex(){
	glBindTexture(GL_TEXTURE_2D,0);
}
void retex(){
	glBindTexture(GL_TEXTURE_2D,Stx);
}
int main(int argc,char**argv){
	glfwInit();
	glfwDisable(GLFW_AUTO_POLL_EVENTS);
	glfwOpenWindow(320,512,0,0,0,0,0,0,GLFW_WINDOW);
	glOrtho(0,160,256,0,1,-1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
	glGenTextures(1,&Stx);
	glBindTexture(GL_TEXTURE_2D,Stx);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,SF,SW,SH,0,SF,GL_UNSIGNED_BYTE,S);
	genL1();
	Pt=atoi(argv[1]);
	udp=socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in udpip={.sin_family=AF_INET,.sin_addr.s_addr=htonl(INADDR_ANY),.sin_port=2000+Pt};
	if(bind(udp,(struct sockaddr*)&udpip,sizeof(udpip))==-1){
		fprintf(stderr,"u%d\n",errno);
		return 1;
	}
	if(argc>2){
		struct sockaddr_in ip={.sin_family=AF_INET,.sin_port=2000};
		if((tcp=socket(AF_INET,SOCK_STREAM,0))<0||inet_aton(argv[2],&ip.sin_addr)<=0||connect(tcp,(struct sockaddr*)&ip,sizeof(ip))<0){
			fprintf(stderr,"c%d\n",errno);
			return 1;
		}
		ip.sin_port=2000+!Pt;
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}else{
		int lis=socket(AF_INET,SOCK_STREAM,0);
		static const uint8_t one8=1;
		setsockopt(lis,SOL_SOCKET,SO_REUSEADDR,&one8,1);
		struct sockaddr_in ip={.sin_family=AF_INET,.sin_addr.s_addr=htonl(INADDR_ANY),.sin_port=2000};
		socklen_t sip=sizeof(ip);
		if(bind(lis,(void*)&ip,sizeof(ip))<0||listen(lis,1)<0||(tcp=accept(lis,(struct sockaddr*)&ip,&sip))<0){
			fprintf(stderr,"s%d\n",errno);
			return 1;
		}
		close(lis);
		ip.sin_port=2000+!Pt;
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}
	static const int one32=1;
	setsockopt(tcp,IPPROTO_TCP,TCP_NODELAY,(char*)&one32,sizeof(int));
	for(;;){
		pushrw();
		for(int i=0;i<3;i++)
			rcol[i]=rand();
		glClear(GL_COLOR_BUFFER_BIT);
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
				mkcan(L[-1],*(uint16_t*)L,*(uint16_t*)(L+2),*(float*)(L+4),*(float*)(L+8),*(float*)(L+12));
				L+=16;
			case(ETAR)
				mktar(L[-1],*(uint16_t*)L,*(uint16_t*)(L+2));
				L+=4;
			case(EB1)
				mkb1();
			case(EB2)
				mkb2();
			}
			w8(L-LL);
			w8(8);
		}
		if(T==MT){
			printf("--%d\n",T);
			Pc[T][Pt]=glfwGetKey('Z')|glfwGetKey('X')<<1|glfwGetKey(GLFW_KEY_RIGHT)<<2|glfwGetKey(GLFW_KEY_LEFT)<<3|glfwGetKey(GLFW_KEY_DOWN)<<4|glfwGetKey(GLFW_KEY_UP)<<5;
			uint8_t pcm[3];
			*(uint16_t*)pcm=T;
			pcm[2]=Pc[T][Pt];
			write(tcp,pcm,3);
		}
		if(!(Pc[T][!Pt]&128)){
			int ot=T;
			while(ot){
				ot--;
				if(Pc[oT][!Pt]&128)Pc[T][!Pt]=Pc[ot][!Pt]&127;
			}
		}
		for(int i=0;i<2;i++){
			float Pxx=getD(Pc[T][i])-getA(Pc[T][i]),Pyy=getS(Pc[T][i])-getW(Pc[T][i]);
			if(i){
				Pxx*=1.5;
				Pyy*=1.5;
			}
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
					if(Pe){
						w8(18);
						Pe--;
					}
					if(i){
						mkpb(1,Px[1],Py[1]-3,0,-3);
					}else{
						mkpb(0,Px[0],Py[0],0,4);
						float xd=64-Px[0],yd=128-Py[0],xy=sqrt(xd*xd+yd*yd)?:1;
						mkpb(0,Px[0],Py[0],xd*3/xy,yd*3/xy);
					}
				}
			}
			if(getZ(Pc[T][i])){
				if(!Pf[i]){
					w8(10+i);
					Pf[i]=1;
				}
			}else{
				w8(Pf[i]);
				w8(22+i);
				Pf[i]=0;
			}
			if(getX(Pc[T][i])){
				if(i){
					if(!Lzo&&Pe>0)mkLzo();
				}else(!Bor){
					Px[0]=128-Px[0];
					Py[0]=256-Py[0];
					mkBor();
				}
			}else(Lzo&&i)Lzo=0;
		}
		if(T==MT)notex();
		if(Lzo||Bor){
			if(Lzo){
				memmove(Lzr+1,Lzr,248);
				Lzr[0][0]=Px[1]+(rand()%3);
				Lzr[0][1]=Py[1]-3+(rand()&3);
				if(T==MT){
					glBegin(GL_QUAD_STRIP);
					for(int i=0;i<32;i++){
						glColor3ub(255-i*8,255-i*8,255-i*8);
						glVertex2f(Lzr[i][0],0);
						glVertex2fv(Lzr[i]);
					}
					glEnd();
				}
			}
			if(Bor){
				if(T==MT){
					glColor3ubv(wht);
					glCirc(Box,Boy,Bor);
				}
				if(++Bor>16)Bor=0;
			}
		}
		if(PBtop>=PB)w8(14);
		glColor3ubv(wht);
		for(bxy*b=PB;b<=PBtop;b++){
			glRectf(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
			b->x+=b->xd;
			b->y+=b->yd;
			if(b->y<-1||b->x<-1||b->x>129||b->y>257){
				wbxy(*b);
				w8(b-PB);
				w8(20);
				*b--=*PBtop--;
			}else glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
		}
		Ph[0]=2;
		Ph[1]=2-Lzo;
		Php[0]=0;
		Php[1]=0;
		w8(27);
		for(obje*e=E;e<=Etop;e++){
			float r;
			int et=!!(e->t&128);
			switch(e->t&127){
			case(ECAN)
				if(T==MT){
					glColor3ubv(red+et);
					glCirc(e->x,e->y,min(T-e->c,e->h));
				}
				e->x+=e->xd;
				e->y+=e->yd;
				erotxy(e,Px[et],Py[et],M_PI/16);
				if(T-e->c&8||!(T-e->c&3))
					mkbd(e->c,e->x,e->y,e->d,6);
				if(e->x<-5||e->x>133||e->y<-5||e->y>261||e->h<1)goto kille;
				else(e->h<3||rdmg(e->x,e->y,e->h*3/2,2)){
					w8(e->h);
					w8(e-E);
					w8(29);
					e->h--;
				}
			case(ETAR)
				r=min(T-e->c,abs(e->h));
				if(T==MT){
					glDisable(GL_BLEND);
					glColor3ubv(wht);
					glCirc(e->x,e->y,r);
					rndcol();
					glCirc(e->x,e->y,min(r,24));
					glColor3ubv(wht);
					glCirc(e->x,e->y,min(r,16));
					rndcol();
					glCirc(e->x,e->y,min(r,8));
					glEnable(GL_BLEND);
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
					glBegin(GL_LINES);
					for(double a=0;a<M_PI;a+=M_PI/48){
						double aa=a+T/256.;
						glColor3ub(rand(),rand(),rand());
						glVertex2f(e->x+cos(aa)*r,e->y+sin(aa)*r);
						glColor3ubv(red+!(e->h&8));
						glVertex2f(e->x-cos(aa)*r,e->y-sin(aa)*r);
					}
					glEnd();
				}
				mkbxy(e->c,e->x,e->y,Px[0],Py[0],4);
				mkbxy(e->c+1,e->x,e->y,Px[1],Py[1],4);
				mkbxy(e->c+2,e->x,e->y,Px[!(e->h&8)],Py[!(e->h&8)],1);
				if(rdmg(e->x,e->y,e->h,!(e->h&8))){
					w8(e->h);
					w8(e-E);
					w8(29);
					e->h++;
				}else(rdmg(e->x,e->y,e->h,!!(e->h&8))){
					w8(e->h);
					w8(e-E);
					w8(29);
					e->h--;
					if(e->h<8)goto kille;
				}
			case(EB2)
				int allz=1;
				for(int y=0;y<4;y++)
					for(int x=0;x<4;x++){
						int xx=28+x*e->a[18]/2,yy=92+y*e->a[18]/2,xy=x+y*4+1;
						if((!(T+x+y*3&15))&&(x==0&&y==0||x==3&&y==0||x==3&&y==3||x==0&&y==3))
							mkbxy(e->c+x+y*4,xx,yy,Px[e->a[xy]],Py[e->a[xy]],1);
						if(!e->a[19]&&e->a[18]==48&&rdmg(xx,yy,8,!e->a[xy])){
							e->a[xy]^=1;
							if(x<3)e->a[xy+1]^=1;
							if(x>0)e->a[xy-1]^=1;
							if(y<3)e->a[xy+4]^=1;
							if(y>0)e->a[xy-4]^=1;
						}
						if(!e->a[xy])allz=0;
						if(T==MT){
							glColor3ubv(red+e->a[xy]);
							glCirc(28+x*e->a[18]/2,92+y*e->a[18]/2-e->a[19],e->a[18]/4-e->a[19]/2);
						}
					}
				if(allz){
					e->a[19]++;
					if(e->a[19]==16)goto kille;
				}else(e->a[18]<48)e->a[18]++;
			}
			if(0)kille:{
				wobje(*e);
				w8(e-E);
				w8(9);
				*e--=*Etop--;
			}
		}
		rndcol();
		if(Btop>=B)w8(25);
		for(bxy*b=B;b<=Btop;b++){
			if(T==MT)glRectf(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
			b->x+=b->xd;
			b->y+=b->yd;
			for(int i=0;i<2;i++)
				if(btop(i,b)<256){
					w8(16);
					Pe++;
					wbxy(*b);
					w16(b-B);
					w8(21);
					*b--=*Btop--;
					Ph[i]--;
					Php[i]=b->p;
					break;
				}
			if(b->y<-1||b->x<-1||b->x>257||b->y>257){
				wbxy(*b);
				w16(b-B);
				w8(21);
				*b--=*Btop--;
			}else(T==MT)glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
		}
		if(Pi){
			w8(15);
			Pi--;
		}else{
			for(int i=0;i<2;i++){
				if(Ph[i]==1){
					if(Pe<128){
						w8(16);
						Pe++;
					}
				}else(Ph[i]<1&&!Pi){
					xDie();
				}
			}
		}
		if(T==MT){
			glRecti(128,0,136,Pe*2);
			glRecti(136,64,144,64+T-oT);
			retex();
			drawSpr(Rev,Px[0]-3,Py[0]-4,Pf[0]>3,shr);
			drawSpr(Ika,Px[1]-3,Py[1]-4,Pf[1]>3,shb);
			glfwSwapBuffers();
			double gT=1./30-glfwGetTime();
			if(gT>0&&(T>oT||1))glfwSleep(gT);
			else printf("%f\n",gT);
			glfwSetTime(0);
			MT++;
		}
		T++;
		if(any(tcp)){
			int len=3;
			//ioctl(tcp,FIONREAD,&len);
			unsigned char ubu[len],*up=ubu+2;
			if(read(tcp,ubu,len)==-1)return 0;
			printf("tcp:");
			for(int i=0;i<len;i++)
				printf(" %d",ubu[i]);
			printf("\n");
			uint16_t mt=*(uint16_t*)ubu;
			if(oTs[mt>>3]&=1<<(mt&7))continue;
			oTs[mt>>3]|=1<<(mt&7);
			Pc[mt][!Pt]=*up|128;
			if(mt<T)stepBack(T-mt);
			if(mt>oT)oT=mt;
		}
		while(rwp&&rwT<oT&&rwT<T)shiftrw();
		//if(++T>MT){
			glfwPollEvents();
			if(glfwGetKey(GLFW_KEY_ESC)||!glfwGetWindowParam(GLFW_OPENED))return 0;
		//}
	}
}
#include "df.h"
int tcp,udp;
int any(int s){
	struct pollfd pfd={.fd=s,.events=POLLIN};
	while((s=poll(&pfd,1,0))==-1);
	return s;
}
int readch(int s){
	uint8_t c;
	ssize_t A;
	while((A=read(s,&c,1))==-1);
	return A?c:-1;
}
int sendch(int s,uint8_t c){
	while(send(s,&c,1,MSG_MORE)==-1);
	return c;
}
typedef struct{
	uint8_t t;
	float x,y;
	union{
		struct{float xd,yd;};
		struct{float d,v;};
	};
}objb;
typedef struct{
	uint8_t t;
	int8_t h;
	float x,y,d;
	union{
		struct{
			float xd,yd;
		};
		void*p;
	};
}obje;
typedef enum{
	BXY,BD,BM,
	ECAN=128,
}oid;
objb B[8192];
objb*Btop=B-1;
obje E[64];
obje*Etop=E-1;
objb PB[256];
objb*PBtop=PB-1;
int Pt,Pf,Of;
float Px[2]={60,100},Py[2]={160,160};
int Lzo,Box,Boy,Bor;
float Lzr[32][2];
void bmake(uint8_t t,float x,float y,float xd,float yd){
	objb*b=++Btop;
	b->t=t;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
}
void bmakexy(float x,float y,float xd,float yd){
	bmake(BXY,x,y,xd,yd);
}
void bmakexyd(float x,float y,float d,float v){
	bmake(BXY,x,y,cos(d)*v,-sin(d)*v);
}
void bmaked(float x,float y,float d,float v){
	bmake(BD,x,y,d,v);
}
void bmakem(float x,float y,float d,float v){
	bmake(BM,x,y,d,v);
}
void emakecan(float x,float y,float d,float xd,float yd){
	obje*e=++Etop;
	e->t=ECAN;
	e->h=5;
	e->x=x;
	e->y=y;
	e->d=d;
	e->xd=xd;
	e->yd=yd;
}
void pbmake(float x,float y,float xd,float yd){
	objb*b=++PBtop;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
	sendch(udp,161);
	send(udp,b,sizeof(objb),MSG_MORE);
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
		Box=(Pt?128-Px[0]:Px[0])+3;
		Boy=(Pt?256-Py[0]:Py[0])+3;
	}
}
int rinpb(float x,float y,int r){
	r*=r;
	int n=0;
	for(objb*b=PB;b<=PBtop;b++)
		if(dst2(x,y,b->x,b->y)<=r){
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
int rdmg(float x,float y,int r){
	return rinpb(x,y,r)+rinbo(x,y,r)+rinlz(x,y,r);
}
int pinr(int x,int y,int x1,int y1,int x2,int y2){
	return x>x1&&x<x2&&y>y1&&y<y2;
}
int pinp(int x,int y,int p){
	return pinr(x,y,Px[p]-3,Py[p]-3,Px[p]+3,Py[p]+3);
}
int binp(void*b,int p){
	float*xy=b;
	return pinp(xy[0],xy[1],p);
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
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,SW,SH,0,SF,GL_UNSIGNED_BYTE,S);
	genL1();
	Pt=atoi(argv[1]);
	udp=socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in udpip={.sin_family=AF_INET,.sin_addr.s_addr=htonl(INADDR_ANY),.sin_port=2000+Pt};
	if(bind(udp,(struct sockaddr*)&udpip,sizeof(udpip))==-1){
		fprintf(stderr,"u%d\n",errno);
		return 1;
	}
	if(argc>2){
		struct sockaddr_in ip={.sin_family=AF_INET,.sin_port=htons(argc>3?atoi(argv[3]):2000)};
		if((tcp=socket(AF_INET,SOCK_STREAM,0))<0||inet_aton(argv[2],&ip.sin_addr)<=0||connect(tcp,(struct sockaddr*)&ip,sizeof(ip))<0){
			fprintf(stderr,"c%d\n",errno);
			return 1;
		}
		ip.sin_port=2000+!Pt;
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}else{
		int lis=socket(AF_INET,SOCK_STREAM,0);
		static const uint8_t one=1;
		setsockopt(lis,SOL_SOCKET,SO_REUSEADDR,&one,1);
		struct sockaddr_in ip={.sin_family=AF_INET,.sin_addr.s_addr=htonl(INADDR_ANY),.sin_port=htons(argc>2?atoi(argv[2]):2000)};
		socklen_t sip=sizeof(ip);
		if(bind(lis,(void*)&ip,sizeof(ip))<0||listen(lis,1)<0||(tcp=accept(lis,(struct sockaddr*)&ip,&sip))<0){
			fprintf(stderr,"s%d\n",errno);
			return 1;
		}
		close(lis);
		ip.sin_port=2000+!Pt;
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}
	uint16_t t=0,ot=0;
	send(udp,&t,2,MSG_MORE);
	unsigned char*L=Lv;
	srand(glfwGetTime()*10e5);
	for(;;){
		glClear(GL_COLOR_BUFFER_BIT);
		while(t==*(uint16_t*)L){
			L+=2;
			switch(*L++){
			case(128)
				emakecan(*(uint16_t*)L,*(uint16_t*)(L+2),*(float*)(L+4),*(float*)(L+8),*(float*)(L+12));
				L+=16;
			}
		}
		int Pxx=glfwGetKey(GLFW_KEY_RIGHT)-glfwGetKey(GLFW_KEY_LEFT),Pyy=glfwGetKey(GLFW_KEY_DOWN)-glfwGetKey(GLFW_KEY_UP);
		if(Pt){
			Pxx*=1.5;
			Pyy*=1.5;
		}
		if(Pxx&&Pyy){
			Pxx*=sqrt(2);
			Pyy*=sqrt(2);
		}
		Px[Pt]=fmin(fmax(Px[Pt]+Pxx,8),120);
		Py[Pt]=fmin(fmax(Py[Pt]+Pyy,8),248);
		if(!glfwGetKey('Z'))
			Pf=0;
		else(++Pf==2||!(Pf%12)){
			if(Pt){
				if(!Lzo)
					pbmake(Px[Pt],Py[Pt]-3,0,-3);
			}else{
				float xd=60-Px[Pt],yd=124-Py[Pt],xy=sqrt(xd*xd+yd*yd);
				if(xy)
					pbmake(Px[Pt],Py[Pt],xd*3/xy,yd*3/xy);
				pbmake(Px[Pt],Py[Pt],0,4);
			}
		}
		if(glfwGetKey('X')){
			if(Pt){
				if(!Lzo)mkLzo();
			}else(!Bor){
				mkBor();
				Px[Pt]=128-Px[Pt];
				Py[Pt]=256-Py[Pt];
			}
		}else(Lzo&&Pt)Lzo=0;
		notex();
		if(Lzo||Bor){
			if(Lzo){
				memmove(Lzr+1,Lzr,248);
				Lzr[0][0]=Px[1]+(rand()%3);
				Lzr[0][1]=Py[1]-3+(rand()&3);
				glBegin(GL_QUAD_STRIP);
				for(int i=0;i<32;i++){
					glColor3ub(255-i*8,255-i*8,255-i*8);
					glVertex2f(Lzr[i][0],0);
					glVertex2fv(Lzr[i]);
				}
				glEnd();
			}
			if(Bor){
				glRecti(Box-Bor,Boy-Bor,Box+Bor,Boy+Bor);
				if(++Bor>16)Bor=0;
			}
		}
		glColor3ub(255,255,255);
		for(objb*b=PB;b<=PBtop;b++){
			glRectf(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
			b->x+=b->xd;
			b->y+=b->yd;
			if(b->y<-1||b->x<-1||b->x>129||b->y>257)
				*b--=*PBtop--;
			else glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
		}
		for(obje*e=E;e<=Etop;e++){
			switch(e->t){
			case(ECAN)
				glRectf(e->x-e->h,e->y-e->h,e->x+e->h,e->y+e->h);
				e->x+=e->xd;
				e->y+=e->yd;
				if(t&16)
					bmakexyd(e->x,e->y,e->d,8);
				if(rdmg(e->x,e->y,e->h*3/2))
					e->h--;
				if(e->x<-5||e->x>133||e->y<-5||e->y>261)*e--=*Etop--;
				else(e->h<0)*e--=*Etop--;
				else(e->h<3)e->h--;
			}
		}
		for(objb*b=B;b<=Btop;b++){
			switch(b->t){
			case(BXY)
				glRectf(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
				b->x+=b->xd;
				b->y+=b->yd;
				if(b->y<-1||b->x<-1||b->x>257||b->y>257)
					*b--=*Btop--;
				else glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
				if(binp(b,Pt)){
					Px[Pt]=64;
				}
			case(BD)
				glRectf(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
				b->x+=cos(b->d)*b->v;
				b->y+=sin(b->d)*b->v;
				glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
			case(BM)
				b->x+=cos(b->d)*b->v;
				b->y+=sin(b->d)*b->v;
			}
			if(b->x<-64||b->x>128+64||b->y<-64||b->y>512+64)
				*b--=*Btop--;
		}
		retex();
		drawSpr(Pt?Ika:Rev,Px[Pt]-3,Py[Pt]-4,Pf>3,255,128,64);
		drawSpr(Pt?Rev:Ika,Px[!Pt]-3,Py[!Pt]-4,Of,64,128,255);
		glfwSwapBuffers();
		while(any(tcp)){
			if(readch(tcp)==-1)return 0;
		}
		while(any(udp)){
			int len;
			ioctl(udp,FIONREAD,&len);
			unsigned char ubu[len],*up=ubu+2;
			read(udp,ubu,len);
			uint16_t mt=*(uint16_t*)ubu;
			if(mt==ot)continue;
			while(up-ubu<len){
				if(*up<128){
					if(mt>ot){
						Px[!Pt]=up[0];
						Py[!Pt]=up[1];
						Of=up[2]&1;
						if(up[2]&2){
							if(Pt){
								mkBor();
							}else{
								mkLzo();
							}
						}else{
							if(Pt){
								Bor=0;
							}else{
								Lzo=0;
							}
						}
					}
					up+=3;
				}else(*up==161){
					Of=1;
					objb*b=(objb*)++up;
					*++PBtop=*b;
					b->x+=b->xd*(mt-ot);
					b->y+=b->yd*(mt-ot);
					up+=sizeof(objb);
				}else{
					printf("Unknown %d\n",*up);
					ot=mt;
					break;
				}
			}
			if(mt>ot)ot=mt;
		}
		if(++t&1){
			unsigned char pxy[]={Px[Pt],Py[Pt],!!Pf|(Pt?Lzo:!!Bor)<<1};
			write(udp,pxy,3);
			send(udp,&t,2,MSG_MORE);
		}
		double gT=1./30-glfwGetTime();
		if(gT>0&&t>ot)glfwSleep(gT);
		else printf("%f\n",gT);
		glfwSetTime(0);
		glfwPollEvents();
		if(glfwGetKey(GLFW_KEY_ESC)||!glfwGetWindowParam(GLFW_OPENED))return 0;
	}
}
#include "df.h"
int tcp,udp;
uint16_t t,ot;
float rnorm(float a){
	a=fmodf(a,M_PI*2);
	return a>M_PI?a-M_PI*2:a;
}
int any(int s){
	struct pollfd pfd={.fd=s,.events=POLLIN};
	while((s=poll(&pfd,1,0))==-1);
	return s;
}
int readch(int s){
	uint8_t c;
	ssize_t a;
	while((a=read(s,&c,1))==-1);
	return a?c:-1;
}
void sendch(int s,uint8_t c){
	while(send(s,&c,1,MSG_MORE)==-1);
}
typedef struct{
	uint32_t t;
	float x,y;
	union{
		struct{float xd,yd;};
		struct{float d,v;};
	};
}objb;
typedef struct{
	uint8_t t;
	int8_t h;
	uint16_t c;
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
	ECAN=32,ETAR,ELZ
}oid;
objb B[8192];
objb*Btop=B-1;
obje E[64];
obje*Etop=E-1;
objb PB[256];
objb*PBtop=PB-1;
int Pt,Pf,Of,Ph,Pe;
float Px[2]={60,100},Py[2]={160,160};
int Lzo,Box,Boy,Bor;
float Lzr[32][2];
void bmake(uint32_t t,float x,float y,float xd,float yd){
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
	e->c=t;
	e->x=x;
	e->y=y;
	e->d=d;
	e->xd=xd;
	e->yd=yd;
}
void emaketar(float x,float y){
	obje*e=++Etop;
	e->t=ETAR;
	e->h=64;
	e->c=t;
	e->x=x;
	e->y=y;
}
void execLz(int c,float x,float y,float d){
	glBegin(GL_TRIANGLES);
	glVertex2f(x,y);
	glVertex2f(x+cos(d+M_PI/64)*c,y-sin(d+M_PI/64)*c);
	glVertex2f(x+cos(d-M_PI/64)*c,y-sin(d-M_PI/64)*c);
	glEnd();
	if(dst(x,y,Px[Pt],Py[Pt])<c&&fabsf(rnorm(d)-rnorm(dir(x,y,Px[Pt],Py[Pt])))<M_PI/64)Ph--;
}
void pbmake(float x,float y,float xd,float yd){
	if(Pe>0){
		Pe--;
		objb*b=++PBtop;
		b->x=x;
		b->y=y;
		b->xd=xd;
		b->yd=yd;
		sendch(udp,161);
		send(udp,b,sizeof(objb),MSG_MORE);
	}
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
float dtop(float x,float y){
	return dst2(x,y,Px[Pt],Py[Pt]);
}
float btop(objb*b){
	return dtop(b->x,b->y);
}
float rrot(float a,float b,float m){
	b=rnorm(b+a);
	if(fabsf(b)<=m)return a-b;
	return b>0?a-m:a+m;
}
void rrotx(float*a,float b,float m){
	*a=rrot(*a,b,m);
}
void erota(obje*e,float a,float m){
	rrotx(&e->d,a,m);
}
void erotxy(obje*e,float x,float y,float m){
	erota(e,dir(e->x,e->y,x,y),m);
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
	send(udp,&t,2,MSG_MORE);
	unsigned char*L=Lv;
	srand(glfwGetTime()*10e5);
	Pe=128;
	for(;;){
		glClear(GL_COLOR_BUFFER_BIT);
		while(t==*(uint16_t*)L){
			L+=2;
			switch(*L++){
			default:printf("Unknown E%d",L[-1]);
			case(ECAN)
				emakecan(*(uint16_t*)L,*(uint16_t*)(L+2),*(float*)(L+4),*(float*)(L+8),*(float*)(L+12));
				L+=16;
			case(ETAR)
				emaketar(*(uint16_t*)L,*(uint16_t*)(L+2));
				L+=4;
			}
		}
		float Pxx=glfwGetKey(GLFW_KEY_RIGHT)-glfwGetKey(GLFW_KEY_LEFT),Pyy=glfwGetKey(GLFW_KEY_DOWN)-glfwGetKey(GLFW_KEY_UP);
		if(Pt){
			Pxx*=1.5;
			Pyy*=1.5;
		}
		if(Pxx&&Pyy){
			Pxx*=sqrt(2);
			Pyy*=sqrt(2);
		}
		Px[Pt]=fminf(fmaxf(Px[Pt]+Pxx,8),120);
		Py[Pt]=fminf(fmaxf(Py[Pt]+Pyy,8),248);
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
				if(!Lzo&&Pe>0)mkLzo();
			}else(!Bor&&Pe>32){
				Pe-=32;
				mkBor();
				Px[Pt]=128-Px[Pt];
				Py[Pt]=256-Py[Pt];
			}
		}else(Lzo&&Pt)Lzo=0;
		notex();
		if(Lzo||Bor){
			if(Lzo){
				if(Pe--<=0)Lzo=0;
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
				glCirc(Box,Boy,Bor);
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
		Ph=2;
		for(obje*e=E;e<=Etop;e++){
			switch(e->t){
			case(ECAN)
				glCirc(e->x,e->y,min(t-e->c,e->h));
				e->x+=e->xd;
				e->y+=e->yd;
				erotxy(e,Px[Pt],Py[Pt],M_PI/16);
				if(t&16)
					bmakexyd(e->x,e->y,e->d,8);
				if(rdmg(e->x,e->y,e->h*3/2))
					e->h--;
				if(e->x<-5||e->x>133||e->y<-5||e->y>261)*e--=*Etop--;
				else(e->h<1)*e--=*Etop--;
				else(e->h<3)e->h--;
			case(ETAR)
				glDisable(GL_BLEND);
				glCirc(e->x,e->y,min(t-e->c,e->h)/2);
				glColor3ub(rand(),rand(),rand());
				glCirc(e->x,e->y,min(min(t-e->c,e->h)/2,24));
				glColor3ub(255,255,255);
				glCirc(e->x,e->y,min(min(t-e->c,e->h)/2,16));
				glColor3ub(rand(),rand(),rand());
				glCirc(e->x,e->y,min(min(t-e->c,e->h)/2,8));
				glColor3ub(255,255,255);
				glEnable(GL_BLEND);
				e->h-=rdmg(e->x,e->y,e->h*2/3);
				for(int i=0;i<2;i++)
					execLz(min(t-e->c,99+e->h),e->x,e->y,dir(e->x,e->y,Px[i],Py[i]));
				if(e->h<-99)*e--=*Etop--;
				else(e->h<6)e->h--;
			}
		}
		for(objb*b=B;b<=Btop;b++){
			switch(b->t&255){
			case(BXY)
				glRectf(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
				b->x+=b->xd;
				b->y+=b->yd;
				if(b->y<-1||b->x<-1||b->x>257||b->y>257)
					*b--=*Btop--;
				else glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
				if(btop(b)<16)Ph--;
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
		if(Ph==1)Pe+=(Pe<128);
		else(Ph<1)Px[Pt]=20;
		glRecti(128,0,136,Pe*2);
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
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
	float x,y,xd,yd;
}bxy;
typedef struct{
	float x,y,d,v;
}bd;
typedef struct{
	float x,y,d,v;
}bm;
typedef struct{
	uint8_t t;
	union{
		struct{float x,y;};
		bxy xy;
		bd d;
		bm m;
	};
}objb;
typedef enum{
	BNO,BXY,BD,BM
}bid;
objb B[512];
int Blen;
bxy PB[256];
int PBlen=-1;
int Pt,Pf;
float Px[2]={60,100},Py[2]={160,160};
int Lzo,Box,Boy,Bor;
float Lzr[32][2];
static void*bmake(uint8_t t,float x,float y){
	while(B[Blen].t)
		if(Blen++==512)
			Blen=0;
	objb*b=B+Blen;
	b->t=t;
	b->x=x;
	b->y=y;
	return &b->m;
}
void bmakexy(float x,float y,float xd,float yd){
	bxy*b=bmake(BXY,x,y);
	b->xd=xd;
	b->yd=yd;
}
void bmakexydv(float x,float y,float d,float v){
	bxy*b=bmake(BXY,x,y);
	b->xd=cos(d)*v;
	b->yd=sin(d)*v;
}
void bmaked(float x,float y,float d,float v){
	bd*b=bmake(BD,x,y);
	b->d=d;
	b->v=v;
}
void bmakem(float x,float y,float d,float v){
	bm*b=bmake(BM,x,y);
	b->d=d;
	b->v=v;
}
void pbmake(float x,float y,float xd,float yd){
	bxy*b=PB+(++PBlen);
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
	sendch(udp,161);
	send(udp,b,sizeof(bxy),MSG_MORE);
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
int pinr(int x,int y,int x1,int y1,int x2,int y2){
	return x>x1&&x<x2&&y>y1&&y<y2;
}
int pinp(int x,int y,int p){
	return pinr(x,y,Px[p]+1,Py[p]+1,Px[p]+6,Py[p]+7);
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
		ip.sin_port=2000+!Pt;
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}
	uint16_t ts=0;
	send(udp,&ts,2,MSG_MORE);
	srand(glfwGetTime()*10e5);
	int t=0;
	uint16_t ot=0;
	for(;;){
		glClear(GL_COLOR_BUFFER_BIT);
		int Pxx=glfwGetKey(GLFW_KEY_RIGHT)-glfwGetKey(GLFW_KEY_LEFT),Pyy=glfwGetKey(GLFW_KEY_DOWN)-glfwGetKey(GLFW_KEY_UP);
		if(Pt){
			Pxx*=1.5;
			Pyy*=1.5;
		}
		if(Pxx&&Pyy){
			Pxx*=sqrt(2);
			Pyy*=sqrt(2);
		}
		Px[Pt]=fmin(fmax(Px[Pt]+Pxx,9),247);
		Py[Pt]=fmin(fmax(Py[Pt]+Pyy,9),247);
		if(!glfwGetKey('Z'))
			Pf=0;
		else if(++Pf==2||!(Pf%12)){
			if(Pt){
				if(!Lzo)
					pbmake(Px[Pt]+3,Py[Pt],0,-3);
			}else{
				float xd=60-Px[Pt],yd=124-Py[Pt],xy=sqrt(xd*xd+yd*yd);
				if(xy)
					pbmake(Px[Pt]+3,Py[Pt]+4,xd*3/xy,yd*3/xy);
				pbmake(Px[Pt]+3,Py[Pt]+4,0,4);
			}
		}
		if(glfwGetKey('X')){
			if(Pt){
				if(!Lzo){
					mkLzo();
				}
			}else if(!Bor){
				mkBor();
				Px[Pt]=128-Px[Pt];
				Py[Pt]=256-Py[Pt];
			}
		}else if(Lzo&&Pt){
			Lzo=0;
		}
		notex();
		if(Lzo||Bor){
			if(Lzo){
				memmove(Lzr+1,Lzr,248);
				Lzr[0][0]=Px[1]+3+(rand()%3);
				Lzr[0][1]=Py[1]+(rand()&3);
				glBegin(GL_QUAD_STRIP);
				for(int i=0;i<32;i++){
					glColor3ub(255-i*8,255-i*8,255-i*8);
					glVertex2f(Lzr[i][0],0);
					glVertex2fv(Lzr[i]);
					printf("%d:%f,%f\n",i,Lzr[i][0],Lzr[i][1]);
				}
				glEnd();
			}
			if(Bor){
				glRecti(Box-Bor,Boy-Bor,Box+Bor,Boy+Bor);
				Bor++;
				if(Bor>16)
					Bor=0;
			}
		}
		glColor3ub(255,255,255);
		for(int i=0;i<=PBlen;i++){
			again:;
			bxy*b=PB+i;
			glRectf(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
			b->x+=b->xd;
			b->y+=b->yd;
			if(b->y<-1||b->x<-1||b->x>257||b->y>257)
				PB[i--]=PB[PBlen--];
			else glRectf(b->x-1,b->y-1,b->x+1,b->y+1);
		}
		retex();
		drawSpr(Pt?Ika:Rev,Px[Pt],Py[Pt],Pf>3);
		drawSpr(Pt?Rev:Ika,Px[!Pt],Py[!Pt],Pf>3);
		/*for(int i=0;i<512;i++){
			objb*b=B+i;
			switch(B[i].t){
				case(BNO)continue;
				case(BXY){
					bxy*Bxy=&b->xy;
					Bxy->x+=Bxy->xd;
					Bxy->y+=Bxy->yd;
					drawSpr(EggA,Bxy->x,Bxy->y,0,0);
					if(binp(Bxy)){
					}
				}
				case(BD){
					bd*Bd=&b->d;
					Bd->x+=cos(Bd->d)*Bd->v;
					Bd->y+=sin(Bd->d)*Bd->v;
					drawSpr(EggA,Bd->x,Bd->y,0,0);
				}
				case(BM){
					bm*Bm=&b->m;
					drawSpr(EggB,Bm->x,Bm->y,0,0);
					Bm->x+=cos(Bm->d)*Bm->v;
					Bm->y+=sin(Bm->d)*Bm->v;
					drawSpr(EggB,Bm->x,Bm->y,0,0);
				}
			}
			if(B[i].x<-64||B[i].x>1024+64||B[i].y<-64||B[i].y>512+64){
				B[i].t=BNO;
			}
		}*/
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
				if(*up<161){
					if(mt>ot){
						Px[!Pt]=up[0];
						Py[!Pt]=up[1];
						if(up[2]){
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
				}else if(*up==161){
					bxy*b=(bxy*)++up;
					PB[++PBlen]=*b;
					b->x+=b->xd*(mt-ot);
					b->y+=b->yd*(mt-ot);
					up+=sizeof(bxy);
				}else{
					printf("Unknown %d\n",*up);
					ot=mt;
					continue;
				}
			}
			ot=mt;
		}
		if(++t&1){
			unsigned char pxy[]={Px[Pt],Py[Pt],(Pt?Lzo:!!Bor)};
			write(udp,pxy,3);
			printf("%d %d\n",pxy[0],pxy[1]);
			uint16_t ts=t>>1;
			send(udp,&ts,2,MSG_MORE);
		}
		double gT=1./30-glfwGetTime();
		if(gT>0)glfwSleep(gT);
		else printf("%f\n",-gT);
		glfwSetTime(0);
		glfwPollEvents();
		if(glfwGetKey(GLFW_KEY_ESC)||!glfwGetWindowParam(GLFW_OPENED))return 0;
	}
}
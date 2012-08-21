#include "df.h"
obje E[64],*Etop=E;
bxy B[8192],*Btop=B,PB[256],*PBtop=PB;
void mkpb(int p,float x,float y,float xd,float yd){
	bxy*b=PBtop;
	b->p=p;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
	incPB();
}
void mkb(int p,float x,float y,float xd,float yd){
	bxy*b=Btop;
	b->p=p;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
	incB();
}
void mkbd(int p,float x,float y,float v,float d){
	mkb(p,x,y,cos(d)*v,sin(d)*v);
}
void mkbxy(int p,float x,float y,float xx,float yy,float v){
	xx-=x;
	yy-=y;
	float xy=sqrt(xx*xx+yy*yy);
	if(xy)mkb(p,x,y,xx*v/xy,yy*v/xy);
}
static int rinpb(float x,float y,int r){
	r*=r;
	int n=0;
	for(bxy*b=PB;b<PBtop;b++){
		if(0)killpb:if(killpb(b))break;
		if(dst2(x,y,b->x,b->y)<=r){
			n+=b->p?256:1;
			goto killpb;
		}
	}
	return n;
}
static int rinlz(float x,float y,int r){
	return Lzo&&x>=Px[1]-r&&x<=Px[1]+r&&(y>=Py[1]||dst2(x,y,Px[1],Py[1])<=sqr(r));
}
static int rinbo(float x,float y,int r){
	return Bor&&dst2(x,y,Box,Boy)<=sqr(r+Bor);
}
static int rdmg(float x,float y,int r){
	int p=rinpb(x,y,r);
	return (p&255)+(p>>8)+rinbo(x,y,r)+rinlz(x,y,r);
}
static int rdmg2(float x,float y,int r){
	return rinpb(x,y,r)+rinbo(x,y,r)+rinlz(x,y,r)*256;
}
static int getb(int x,int i){
	return i?x>>8:x&255;
}
static float dtop(int p,float x,float y){
	return dst2(x,y,Px[p],Py[p]);
}
static float btop(int p,bxy*b){
	return dtop(p,b->x,b->y);
}
static float rnorm(float a){
	a=fmodf(a,M_PI*2);
	return a>M_PI?a-M_PI*2:a;
}
static void xLz(float c,float x,float y,float d){
	if(T==MT)
		glTriangle(x,y,x+cos(d+M_PI/64)*c,y+sin(d+M_PI/64)*c,x+cos(d-M_PI/64)*c,y+sin(d-M_PI/64)*c);
	c*=c;
	for(int i=0;i<2;i++)
		if(dst2(x,y,Px[i],Py[i])<c&&fabsf(rnorm(d-dir(x,y,Px[i],Py[i])))<M_PI/64)Ph[i]--;
}
static float rrot(float a,float b,float m){
	float c=rnorm(b-a);
	return fabsf(c)<=m?b:c>0?a+m:a-m;
}
static float rrotxy(float a,float x1,float y1,float x2,float y2,float m){
	return rrot(a,dir(x1,y1,x2,y2),m);
}
static void erotxy(obje*e,float x,float y,float m){
	seted(e,rrotxy(e->d,e->x,e->y,x,y,m));
}
static int nerdest(obje*e){
	return fabsf(rnorm(e->d-dir(e->x,e->y,Px[0],Py[0])))>fabsf(rnorm(e->d-dir(e->x,e->y,Px[1],Py[1])));
}
static int nearest(obje*e){
	return dst2(e->x,e->y,Px[0],Py[0])>dst2(e->x,e->y,Px[1],Py[1]);
}
static obje*mke(uint8_t t,float x,float y){
	obje*e=Etop;
	e->t=t;
	e->c=T;
	e->x=x;
	e->y=y;
	incE();
	return e;
}
void eloop(){
	if(Btop>B||Etop>E)marke();
	for(bxy*b=B;b<Btop;b++){
		if(0)killb:if(killb(b))break;
		float bx=b->x,by=b->y;
		b->x+=b->xd;
		b->y+=b->yd;
		if(b->y<0||b->x<0||b->x>128||b->y>256)goto killb;
		if(T==MT){
			glLine(bx,by,b->x,b->y);
			glRect(b->x-1,b->y-1,b->x+1,b->y+1);
		}
		for(int i=0;i<2;i++)
			if(btop(i,b)<256){
				if(Php[i]!=b->p){
					Ph[i]--;
					Php[i]=b->p;
				}
				goto killb;
			}
	}
	for(obje*e=E;e<Etop;e++){
		if(0)kille:if(kille(e))break;
		float x=e->x,y=e->y,r;
		int et=!!(e->t&128);
		switch(e->t&127){
		case(MIS)
			seteh(e,e->h+(e->h<32?2:0));
			if(e->h>12)
				setexy(e,x+=e->xd*(e->h-8)/16,y+=e->yd*(e->h-8)/16);
			if(x+e->h<0||x-e->h>128||y+e->h<0||y-e->h>256)goto kille;
			for(int i=0;i<2;i++)
				if(dst2(x,y,Px[i],Py[i])<sqr(e->h))Ph[i]--;
			if(T==MT){
				glColor(red+et);
				glCirc(x,y,e->h);
			}
		case(CAN)
			e->x=x+=e->xd;
			e->y=y+=e->yd;
			et=nerdest(e);
			erotxy(e,Px[et],Py[et],M_PI/16);
			if(!(T-e->c&3))
				mkbd(e->c,x+cos(e->d)*r*2,y+sin(e->d)*r*2,6,e->d);
			if(x<-5||x>133||y<-5||y>261||e->h<1)goto kille;
			else(e->h<4||rdmg(x,y,e->h*3/2)){
				deceh(e);
			}
			if(T==MT){
				float r=min(T-e->c,e->h);
				glColor(wht);
				glLine(x,y,x+cos(e->d)*r*2,y+sin(e->d)*r*2);
				glColor(red+et);
				glCirc(x,y,r);
			}
		case(TAR)
			r=min(T-e->c,abs(e->h));
			et=e->h<6?4:rdmg(x,y,r);
			if(et)
				seteh(e,e->h-et);
			setexdyd(e,rrotxy(e->xd,x,y,Px[0],Py[0],M_PI/72),rrotxy(e->yd,x,y,Px[1],Py[1],M_PI/72));
			if(e->h<-120)goto kille;
			if(T==MT){
				glColor(wht);
				glCirc(x,y,r);
				rndcol();
				glCirc(x,y,min(r,24));
				glColor(wht);
				glCirc(x,y,min(r,16));
				rndcol();
				glCirc(x,y,min(r,8));
			}
			for(int i=0;i<2;i++)
				xLz(min(T-e->c,127+e->h),x,y,i?e->yd:e->xd);
		case(ROT)
			e->x=x+=e->xd;
			e->y=y+=e->yd;
			e->d+=M_PI/(et?128:-128);
			for(int i=0;i<3;i++)
				mkbd(e->c,x+cos(e->d+i*M_PI*2/3)*32,y+sin(e->d+i*M_PI*2/3)*32,8,e->d+i*M_PI*2/3);
			if(x<-5||x>133||y<-5||y>261||e->h<1)goto kille;
			else(e->h<8||rdmg(x,y,e->h)){
				deceh(e);
			}
			if(T==MT){
				rndcol();
				glCirc(x,y,e->h);
				glColor(wht);
				for(int i=0;i<3;i++)
					glLine(x+cos(e->d+i*M_PI*2/3)*(32-e->h*3),y+sin(e->d+i*M_PI*2/3)*(32-e->h*3),x+cos(e->d+i*M_PI*2/3)*32,y+sin(e->d+i*M_PI*2/3)*32);
			}
		case(DOG)
			et=nearest(e);
			if(dst2(x,y,Px[et],Py[et])<64){
				setPx(et,e->x);
				setPy(et,e->y);
				if(!et)setBor(0);
				if(!(T&7)){
					deceh(e);
					if(!e->h)goto kille;
				}
			}else{
				erotxy(e,Px[et],Py[et],M_PI/32);
				setexy(e,x+=cos(e->d)*e->xd,y+=sin(e->d)*e->xd);
				et=2;
			}
			if(rdmg2(x,y,16)&(et==2?0xFFFF:et==1?0x00FF:0xFF00)){
				deceh(e);
				if(!e->h)goto kille;
			}
			if(T==MT){
				glColor(et==2?wht:red+et);
				glCirc(x,y,min(e->h,T-e->c));
			}
		case(POO)
			et=nearest(e);
			erotxy(e,Px[et],Py[et],M_PI/48);
			setexy(e,x+=cos(e->d)*e->xd,y+=sin(e->d)*e->xd);
			if(!(T-e->c&7))
				mkbd(e->c,x,y,0,0);
			if(T==MT){
				rndcol();
				glCirc(x,y,min(T-e->c,e->h)/2);
			}
			if(dst2(x,y,Px[et],Py[et])<sqr(e->h)||e->h<16){
				deceh(e);
				if(!e->h)goto kille;
			}
		case(B1)
			if(y<64){
				add2ey(e);
				y+=2;
			}
			mkbxy(e->c,x,y,Px[0],Py[0],4);
			mkbxy(e->c+1,x,y,Px[1],Py[1],4);
			mkbxy(e->c+2,x,y,Px[!(e->h&8)],Py[!(e->h&8)],1);
			et=rdmg2(x,y,e->h);
			if((e->h&7)!=7&&getb(et,e->h&8)){
				seteh(e,e->h+1);
			}else(getb(et,!(e->h&8))||e->h<7){
				deceh(e);
				if(!e->h)goto kille;
			}
			if(T==MT)
				for(double a=0;a<M_PI;a+=M_PI/48){
					double aa=a+T/256.;
					rndrndcol();
					float xd=cos(aa)*e->h,yd=sin(aa)*e->h;
					glLineC(x+xd,y+yd,x-xd,y-yd,red+!(e->h&8));
				}
		case(B2)
			for(int y=0;y<4;y++)
				for(int x=0;x<4;x++){
					int xx=28+x*(e->a[18]/2-e->a[19]),yy=92+y*(e->a[18]/2-e->a[19]),xy=x+y*4+1;
					if((!(T+x+y*3&15))&&(x==0&&y==0||x==3&&y==3))
						mkbxy(e->c+xy,xx,yy,Px[e->a[xy]],Py[e->a[xy]],1);
					if(getb(rdmg2(xx,yy,8),e->a[xy]))
						eb2xy(e,xy,x,y);
					if(T==MT){
						glColor(red+e->a[xy]);
						glCirc(xx,yy,e->a[18]/4-e->a[19]/2);
					}
				}
			for(int i=0;i<16;i++)
				if(!e->a[i+1]){
					if(e->a[18]<48)
						ince18(e);
					goto b2nodeath;
				}
			ince19(e);
			if(e->a[19]==24)goto kille;
			b2nodeath:if(!(T&127)){
				obje*m=mke(DOG,64,128);
				m->h=16;
				m->xd=.5;
				m->d=M_PI*3/2;
			}
		case(B3)
			if(x==32&&y==32)y+=2;
			else(x==32&&y==224)x+=2;
			else(x==96&&y==224)y-=2;
			else(x==96&&y==32)x-=2;
			else(x==32)y+=2;
			else(x==96)y-=2;
			else(y==32)x-=2;
			else(y==224)x+=2;
			setexy(e,x,y);
			seted(e,e->d+M_PI/32);
			xLz(min(T-e->c,127+e->h),x,y,dir(x,y,Px[!et],Py[!et]));
			if((T-e->c&63)>23&&(T-e->c&63)<49){
				mkbd(e->c,x,y,3,e->d);
			}else(!((T-e->c)&63)){
				obje*m=mke(e->t&128|MIS,x,y);
				m->h=0;
				m->xd=cos(dir(x,y,Px[et],Py[et]));
				m->yd=sin(dir(x,y,Px[et],Py[et]));
			}
			for(obje*m=E;m<Etop;m++)
				if(m->t==(MIS|((e->t&128)^128))&&dst2(x,y,m->x,m->y)<sqr((e->h>>1)+m->h)){
					deceh(e);
					break;
				}
			if(!(T&7)&&rdmg(x,y,e->h))deceh(e);
			if(e->h<24)deceh(e);
			if(!e->h)goto kille; //Should explode
			if(T==MT){
				rndcol();
				glCirc(e->x,e->y,e->h>>1);
			}
		}
	}
}
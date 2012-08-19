#define RWSHIM
#include "df.h"
obje E[64],*Etop=E;
bxy B[8192],*Btop=B,PB[256],*PBtop=PB;
void mkpb(int p,float x,float y,float xd,float yd){
	w8(17);
	bxy*b=PBtop++;
	b->p=p;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
}
void mkb(int p,float x,float y,float xd,float yd){
	w8(26);
	bxy*b=Btop++;
	b->p=p;
	b->x=x;
	b->y=y;
	b->xd=xd;
	b->yd=yd;
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
int rinpb(float x,float y,int r){
	r*=r;
	int n=0;
	for(bxy*b=PB;b<PBtop;b++)
	pbagain:
		if(dst2(x,y,b->x,b->y)<=r){
			n+=b->p?256:1;
			wbxy(*b);
			w8(b-PB);
			w8(20);
			if(b==--PBtop)break;
			*b=*PBtop;
			goto pbagain;
		}
	return n;
}
int rinlz(float x,float y,int r){
	return Lzo&&x>=Px[1]-r&&x<=Px[1]+r&&(y>=Py[1]||dst2(x,y,Px[1],Py[1])<=sqr(r));
}
int rinbo(float x,float y,int r){
	return Bor&&dst2(x,y,Box,Boy)<=sqr(r+Bor);
}
int rdmg(float x,float y,int r){
	int p=rinpb(x,y,r);
	return (p&255)+(p>>8)+rinbo(x,y,r)+rinlz(x,y,r);
}
int rdmg2(float x,float y,int r){
	return rinpb(x,y,r)+rinbo(x,y,r)+rinlz(x,y,r)*256;
}
int getb(int x,int i){
	return i?x>>8:x&255;
}
float dtop(int p,float x,float y){
	return dst2(x,y,Px[p],Py[p]);
}
float btop(int p,bxy*b){
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
float rrot(float a,float b,float m){
	float c=rnorm(b-a);
	return fabsf(c)<=m?b:c>0?a+m:a-m;
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
int nerdest(obje*e){
	return fabsf(rnorm(e->d-dir(e->x,e->y,Px[0],Py[0])))>fabsf(rnorm(e->d-dir(e->x,e->y,Px[1],Py[1])));
}
int nearest(obje*e){
	return dst2(e->x,e->y,Px[0],Py[0])>dst2(e->x,e->y,Px[1],Py[1]);
}
void eloop(){
	if(Btop>B||Etop>E)w8(25);
	for(bxy*b=B;b<Btop;b++){
	bagain:;
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
		if(0){killb:
			wbxy(*b);
			w16(b-B);
			w8(21);
			if(b==--Btop)break;
			*b=*Btop;
			goto bagain;
		}
	}
	for(obje*e=E;e<Etop;e++){
	eagain:;
		float r;
		int et=!!(e->t&128);
		switch(e->t&127){
		case(ECAN)
			e->x+=e->xd;
			e->y+=e->yd;
			et=nerdest(e);
			erotxy(e,Px[et],Py[et],M_PI/16);
			if(!(T-e->c&3))
				mkbd(e->c,e->x+cos(e->d)*r*2,e->y+sin(e->d)*r*2,6,e->d);
			if(e->x<-5||e->x>133||e->y<-5||e->y>261||e->h<1)goto kille;
			else(e->h<4||rdmg(e->x,e->y,e->h*3/2)){
				w8(e-E);
				w8(38);
				e->h--;
			}
			if(T==MT){
				float r=min(T-e->c,e->h);
				glColor(wht);
				glLine(e->x,e->y,e->x+cos(e->d)*r*2,e->y+sin(e->d)*r*2);
				glColor(red+et);
				glCirc(e->x,e->y,r);
			}
		case(ETAR)
			r=min(T-e->c,abs(e->h));
			et=e->h<6?4:rdmg(e->x,e->y,r);
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
			if(e->h<-120)goto kille;
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
			for(int i=0;i<2;i++)
				xLz(min(T-e->c,120+e->h),e->x,e->y,i?e->yd:e->xd);
		case(EB1)
			if(e->y<64){
				w8(e-E);
				w8(31);
				e->y+=2;
			}
			mkbxy(e->c,e->x,e->y,Px[0],Py[0],4);
			mkbxy(e->c+1,e->x,e->y,Px[1],Py[1],4);
			mkbxy(e->c+2,e->x,e->y,Px[!(e->h&8)],Py[!(e->h&8)],1);
			et=rdmg2(e->x,e->y,e->h);
			if((e->h&7)!=7&&getb(et,e->h&8)){
				w8(e->h++);
				w8(e-E);
				w8(29);
			}else(getb(et,!(e->h&8))||e->h<7){
				w8(e-E);
				w8(38);
				if(!--e->h)goto kille;
			}
			if(T==MT)
				for(double a=0;a<M_PI;a+=M_PI/48){
					double aa=a+T/256.;
					rndrndcol();
					float xd=cos(aa)*e->h,yd=sin(aa)*e->h;
					glLineC(e->x+xd,e->y+yd,e->x-xd,e->y-yd,red+!(e->h&8));
				}
		case(EB2)
			for(int y=0;y<4;y++)
				for(int x=0;x<4;x++){
					int xx=28+x*(e->a[18]/2-e->a[19]),yy=92+y*(e->a[18]/2-e->a[19]),xy=x+y*4+1;
					if((!(T+x+y*3&15))&&(x==0&&y==0||x==3&&y==0||x==3&&y==3||x==0&&y==3))
						mkbxy(e->c+x+y*4,xx,yy,Px[e->a[xy]],Py[e->a[xy]],1);
					if(getb(rdmg2(xx,yy,8),e->a[xy])){
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
					w8(18);
					e->a[18]++;
				}
			}else{
				w8(e-E);
				w8(19);
				if(++e->a[19]==24)goto kille;
			}
		case(EROT)
			e->x+=e->xd;
			e->y+=e->yd;
			e->d+=M_PI/(et?128:-128);
			for(int i=0;i<3;i++)
				mkbd(e->c,e->x+cos(e->d+i*M_PI*2/3)*32,e->y+sin(e->d+i*M_PI*2/3)*32,8,e->d+i*M_PI*2/3);
			if(e->x<-5||e->x>133||e->y<-5||e->y>261||e->h<1)goto kille;
			else(e->h<8||rdmg(e->x,e->y,e->h)){
				w8(e-E);
				w8(38);
				e->h--;
			}
			if(T==MT){
				rndcol();
				glCirc(e->x,e->y,e->h);
				glColor(wht);
				for(int i=0;i<3;i++)
					glLine(e->x+cos(e->d+i*M_PI*2/3)*(32-e->h*3),e->y+sin(e->d+i*M_PI*2/3)*(32-e->h*3),e->x+cos(e->d+i*M_PI*2/3)*32,e->y+sin(e->d+i*M_PI*2/3)*32);
			}
		case(EDOG)
			et=nearest(e);
			if(dst2(e->x,e->y,Px[et],Py[et])<64){
				wfloat(Px[et]);
				w8(2+et);
				wfloat(Py[et]);
				w8(4+et);
				Px[et]=e->x;
				Py[et]=e->y;
			}else{
				erotxy(e,Px[et],Py[et],M_PI/32);
				wfloat(e->x);
				wfloat(e->y);
				w8(e-E);
				w8(41);
				e->x+=cos(e->d)*e->xd;
				e->y+=sin(e->d)*e->xd;
				et=2;
			}
			if(rdmg2(e->x,e->y,16)&(et==2?0xFFFF:et==1?0x00FF:0xFF00)){
				w8(e-E);
				w8(38);
				if(!--e->h)goto kille;
			}
			if(T==MT){
				glColor(et==2?wht:red+et);
				glCirc(e->x,e->y,min(e->h,T-e->c));
			}
		case(EPOO)
			et=nearest(e);
			erotxy(e,Px[et],Py[et],M_PI/64);
			wfloat(e->x);
			wfloat(e->y);
			w8(e-E);
			w8(41);
			e->x+=cos(e->d)*e->xd;
			e->y+=sin(e->d)*e->xd;
			if(!(T-e->c&7))
				mkbd(e->c,e->x,e->y,0,0);
			if(T==MT){
				rndcol();
				glCirc(e->x,e->y,min(T-e->c,e->h)/2);
			}
			if(dst2(e->x,e->y,Px[et],Py[et])<e->h*e->h/2||e->h<8){
				w8(e-E);
				w8(38);
				if(!--e->h)goto kille;
			}
		}
		if(0)kille:{
			wobje(*e);
			w8(e-E);
			w8(9);
			if(e==--Etop)break;
			*e=*Etop;
			goto eagain;
		}
	}
}
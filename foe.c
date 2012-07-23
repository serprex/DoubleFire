#include "df.h"
obje E[64],*Etop=E-1;
bxy B[8192],*Btop=B-1,PB[256],*PBtop=PB-1;
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
static float rnorm(float a){
	a=fmodf(a,M_PI*2);
	return a>M_PI?a-M_PI*2:a;
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
		if(dst2(x,y,b->x,b->y)<=r){
			if(b->p!=p)n++;
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
int nearest(float x,float y){
	return dst2(x,y,Px[0],Py[0])>dst2(x,y,Px[1],Py[1]);
}
void eloop(){
	if(Btop>=B||Etop>=E)w8(25);
	for(bxy*b=B;b<=Btop;b++){
		float bx=b->x,by=b->y;
		b->x+=b->xd;
		b->y+=b->yd;
		if(T==MT){
			glLine(bx,by,b->x,b->y);
			glRect(b->x-1,b->y-1,b->x+1,b->y+1);
		}
		if(b->y<0||b->x<0||b->x>128||b->y>256)goto killb;
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
			*b--=*Btop--;
		}
	}
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
			if((e->h&7)!=7&&rdmg(e->x,e->y,e->h,!(e->h&8))){
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
				if(++e->a[19]==24)goto kille;
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
			else(e->h<8||rdmg(e->x,e->y,e->h,2)){
				w8(e-E);
				w8(38);
				e->h--;
			}
		case(EDOG)
			et=nearest(e->x,e->y);
			if(dst2(e->x,e->y,Px[et],Py[et])<64){
				Px[et]=e->x;
				Py[et]=e->y;
			}else{
				erotxy(e,Px[et],Py[et],M_PI/32);
				wfloat(e->x);
				wfloat(e->y);
				w8(e-E);
				w8(41);
				e->x+=cos(e->d)*e->xd;
				e->y-=sin(e->d)*e->xd;
				et=2;
			}
			if(T==MT){
				glColor(et==2?wht:red+et);
				glCirc(e->x,e->y,min(e->h,T-e->c));
			}
			if(rdmg(e->x,e->y,16,et)){
				w8(e-E);
				w8(38);
				e->h--;
				if(!e->h)goto kille;
			}
		}
		if(0)kille:{
			wobje(*e);
			w8(e-E);
			w8(9);
			*e--=*Etop--;
		}
	}
}
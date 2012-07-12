#include "df.h"
struct spr spr[LSPR]={
	{0,0,7,8},
	{0,8,7,8},
};
void drawRect_(int x,int y,int w,int h,float tx,float ty,float tw,float th){
	glTexCoord2f(tx,ty);
	glVertex2i(x,y);
	glTexCoord2f(tx+tw,ty);
	glVertex2i(x+w,y);
	glTexCoord2f(tx+tw,ty+th);
	glVertex2i(x+w,y+h);
	glTexCoord2f(tx,ty+th);
	glVertex2i(x,y+h);
}
void drawRect(int x,int y,int w,int h,float tx,float ty,float tw,float th){
	glBegin(GL_QUADS);
	drawRect_(x,y,w,h,tx,ty,tw,th);
	glEnd();
}
void drawSpr_(sprid s,int x,int y,int f,int hv){
	int h=!(hv&1),v=!(hv&2);
	drawRect(h?x:x+spr[s].w,v?y:y+spr[s].h,spr[s].w*(h?1:-1),spr[s].h*(v?1:-1),(spr[s].x+spr[s].w*f)/(float)SW,spr[s].y/(float)SH,spr[s].w/(float)SW,spr[s].h/(float)SH);
}
void drawSpr(sprid s,int x,int y,int f,const uint8_t*c){
	glColor3ub(rand(),rand(),rand());
	drawSpr_(s,x,y,f*2+1,0);
	glColor3ubv(c);
	drawSpr_(s,x,y,f*2,0);
}
static void dVine(float x,float y1,float y2){
	glVertex2f(x,y1);
	glVertex2f(x,y2);
	glVertex2f(x+.5,y1);
	glVertex2f(x+.5,y2);
}
void glCirc(float xx,float yy,float r){
	float f=1-r,fx=1,fy=r*2,x=0,y=r;
	glBegin(GL_LINES);
	dVine(xx,yy+r,yy-r);
	while(x<y)
	{
		if(f>=0)
		{
			y--;
			fy-=2;
			f-=fy;
		}
		x++;
		fx+=2;
		f+=fx;
		dVine(xx+x,yy+y,yy-y);
		dVine(xx-x,yy+y,yy-y);
		dVine(xx+y,yy+x,yy-x);
		dVine(xx-y,yy+x,yy-x);
	}
	glEnd();
}
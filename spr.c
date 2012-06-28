#include "df.h"
struct spr spr[LSPR]={
	{0,0,8,8},
	{0,8,8,8},
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
	drawRect(h?x:x+spr[s].w,v?y:y+spr[s].h,spr[s].w*(h?1:-1),spr[s].h*(v?1:-1),(spr[s].x+spr[s].w*f)/256.,spr[s].y/256.,spr[s].w/256.,spr[s].h/256.);
}
void drawSpr(sprid s,int x,int y,int f){
	glColor3ub(255,255,255);
	drawSpr_(s,x,y,f*2,0);
	glColor3ub(rand(),rand(),rand());
	drawSpr_(s,x,y,f*2+1,0);
}
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
void drawSpr(sprid s,int x,int y,int f,uint8_t*c){
	glColor3ub(rand(),rand(),rand());
	drawSpr_(s,x,y,f*2+1,0);
	glColor3ubv(c);
	drawSpr_(s,x,y,f*2,0);
}
void glCirc(float x,float y,float r)
{
	r=fabsf(r);
	const float r2=r*r,r12=r*M_SQRT1_2;
	glBegin(GL_LINES);
	for(float xc=0,yc=r;xc<r12;xc++)
	{
		if(sqr(xc)+sqr(yc)>r2)yc--;
		glVertex2f(x+xc,y+yc);
		glVertex2f(x+xc,y-yc);
		glVertex2f(x-xc,y+yc);
		glVertex2f(x-xc,y-yc);
		glVertex2f(x+yc,y+xc);
		glVertex2f(x+yc,y-xc);
		glVertex2f(x-yc,y+xc);
		glVertex2f(x-yc,y-xc);
		glVertex2f(x+xc+.5,y+yc);
		glVertex2f(x+xc+.5,y-yc);
		glVertex2f(x-xc+.5,y+yc);
		glVertex2f(x-xc+.5,y-yc);
		glVertex2f(x+yc+.5,y+xc);
		glVertex2f(x+yc+.5,y-xc);
		glVertex2f(x-yc+.5,y+xc);
		glVertex2f(x-yc+.5,y-xc);
	}
	glEnd();
}
#include "df.h"
#include "tgen.h"
#include <GL/glfw.h>
struct spr{
	int x,y,w,h;
};
static struct spr spr[LSPR]={
	{0,0,7,8},
	{0,8,7,8},
};
static uint8_t rcol[3];
static GLuint Stx;
void notex(){
	glBindTexture(GL_TEXTURE_2D,0);
}
void retex(){
	glBindTexture(GL_TEXTURE_2D,Stx);
}
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
void glCirc(float xx,float yy,float r){
	float f=1-r,fx=1,fy=r*2,x=0,y=r;
	glBegin(GL_QUADS);
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
		glVertex2f(xx+x,yy+y);
		glVertex2f(xx-x,yy+y);
		glVertex2f(xx-x,yy-y);
		glVertex2f(xx+x,yy-y);
		glVertex2f(xx+y,yy+x);
		glVertex2f(xx-y,yy+x);
		glVertex2f(xx-y,yy-x);
		glVertex2f(xx+y,yy-x);
	}
	glEnd();
}
static void glLine1(float x1,float y1,float x2,float y2){
	glBegin(GL_QUADS);
	if(fabsf(x1-x2)<fabsf(y1-y2)){
		glVertex2f(x1-.5,y1);
		glVertex2f(x1+.5,y1);
	}else{
		glVertex2f(x1,y1-.5);
		glVertex2f(x1,y1+.5);
	}
}
static void glLine2(float x1,float y1,float x2,float y2){
	if(fabsf(x1-x2)<fabsf(y1-y2)){
		glVertex2f(x2-.5,y2);
		glVertex2f(x2+.5,y2);
	}else{
		glVertex2f(x2,y2-.5);
		glVertex2f(x2,y2+.5);
	}
	glEnd();
}
void glLine(float x1,float y1,float x2,float y2){
	glLine1(x1,y1,x2,y2);
	glLine2(x1,y1,x2,y2);
}
void glLineC(float x1,float y1,float x2,float y2,colt c){
	glLine1(x1,y1,x2,y2);
	glColor(c);
	glLine2(x1,y1,x2,y2);
}
void glTriangle(float x1,float y1,float x2,float y2,float x3,float y3){
	glBegin(GL_TRIANGLES);
	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	glVertex2f(x3,y3);
	glEnd();
}
void glLzr(){
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<32;i++){
		glColor3ub(255-i*8,255-i*8,255-i*8);
		glVertex2f(Lzr[i][0],0);
		glVertex2fv(Lzr[i]);
	}
	glEnd();
}
void glColor(colt c){
	glColor3ubv((const GLubyte*)c);
}
void rndcol(){
	glColor3ubv(rcol);
}
void rndrndcol(){
	glColor3ub(rand(),rand(),rand());
}
void enableBlend(){
	glEnable(GL_BLEND);
}
void disableBlend(){
	glDisable(GL_BLEND);
}
void sprInit(){
	glfwInit();
	glfwDisable(GLFW_AUTO_POLL_EVENTS);
	glfwOpenWindow(320,512,0,0,0,0,0,0,GLFW_WINDOW);
	glOrtho(0,160,256,0,1,-1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glGenTextures(1,&Stx);
	glBindTexture(GL_TEXTURE_2D,Stx);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,SF,SW,SH,0,SF,GL_UNSIGNED_BYTE,S);
}
void sprBeginFrame(){
	for(int i=0;i<3;i++)
		rcol[i]=rand();
	glClear(GL_COLOR_BUFFER_BIT);
}
void sprEndFrame(int fskip){
	glfwSwapBuffers();
	double gT=1./30-glfwGetTime();
	if(gT>0&&!fskip)glfwSleep(gT);
	else printf("sleep %f\n",gT);
	glfwSetTime(0);
}
int sprInput(){
	glfwPollEvents();
	if(glfwGetKey(GLFW_KEY_ESC)||!glfwGetWindowParam(GLFW_OPENED)){
		uint8_t a=0;
		nsend(&a,1);
		exit(0);
	}
	return glfwGetKey('Z')|glfwGetKey('X')<<1|glfwGetKey(GLFW_KEY_RIGHT)<<2|glfwGetKey(GLFW_KEY_LEFT)<<3|glfwGetKey(GLFW_KEY_DOWN)<<4|glfwGetKey(GLFW_KEY_UP)<<5;
}
#include "df.h"
#include "tgen.h"
#include <GL/glfw.h>
#define XXX 0
#define XOX 1
#define OOX 2
#define XOO 3
#define A(a,b,c,d,e) a|b<<2|c<<4|d<<6,e
#define B(a,b,c,d,e) |a<<2|b<<4|c<<6,d|e<<2
#define C(a,b,c,d,e) |a<<4|b<<6,c|d<<2|e<<4
#define D(a,b,c,d,e) |a<<6,b|c<<2|d<<4|e<<6,
static const uint8_t abc[]={
	A(
	XXX,
	XOX,
	XOX,
	XOX,
	XXX)
	B(
	OOX,
	OOX,
	OOX,
	OOX,
	OOX)
	C(
	XXX,
	OOX,
	XXX,
	XOO,
	XXX)
	D(
	XXX,
	OOX,
	XXX,
	OOX,
	XXX)
	A(
	XOX,
	XOX,
	XXX,
	OOX,
	OOX)
	B(
	XXX,
	XOO,
	XXX,
	OOX,
	XXX)
	C(
	XOO,
	XOO,
	XXX,
	XOX,
	XXX)
	D(
	XXX,
	OOX,
	OOX,
	OOX,
	OOX)
	A(
	XXX,
	XOX,
	XXX,
	XOX,
	XXX)
	B(
	XXX,
	XOX,
	XXX,
	OOX,
	OOX)
	C(
	XXX,
	XXX,
	XXX,
	XXX,
	XXX)
	D(
	OOX,
	XOO,
	XXX,
	OOX,
	XOO)
};
void tfChar(float x,float y,int c){
	glBegin(GL_QUADS);
	for(int j=0;j<5;j++)
		for(int i=0;i<5;i++){
			int bit=(c=='t'?110:c=='.'?100:(c-'0')*10)+j*2;
			if((49727>>5*((abc[bit>>3]>>(bit&7))&3))&1<<i){
				glVertex2f(x+i,y+j);
				glVertex2f(x+i+1,y+j);
				glVertex2f(x+i+1,y+j+1);
				glVertex2f(x+i,y+j+1);
			}
		}
	glEnd();
}
static struct spr{
	int x,y,w,h;
}spr[LSPR]={
	{0,0,7,8},
	{0,8,7,8},
};
static uint8_t rcol[4];
static GLuint Stx;
void notex(){
	glBindTexture(GL_TEXTURE_2D,0);
}
void retex(){
	glBindTexture(GL_TEXTURE_2D,Stx);
}
void drawRect_(int x,int y,int w,int h,float tx,float ty,float tw,float th){
	glTexCoord2f(tx,ty+th);
	glVertex2i(x,y);
	glTexCoord2f(tx+tw,ty+th);
	glVertex2i(x+w,y);
	glTexCoord2f(tx+tw,ty);
	glVertex2i(x+w,y+h);
	glTexCoord2f(tx,ty);
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
	rndcol();
	drawSpr_(s,x,y,f*2+1,0);
	glColor3ubv(c);
	drawSpr_(s,x,y,f*2,0);
}
void glCirc(float x,float y,float r){
	if(!r)return;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x,y);
	for(float a=0;a<M_PI*2;a+=2/r){
		glVertex2f(x+cosf(a)*r,y+sinf(a)*r);
	}
	glVertex2f(x+r,y);
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
void glRect(float x1,float y1,float x2,float y2){
	glRectf(x1,y1,x2,y2);
}
void glLzr(){
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<32;i++){
		glColor3ub(255-i*8,255-i*8,255-i*8);
		glVertex2f(Lzr[i][0],256);
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
	uint32_t x=rnd();
	glColor3ubv((uint8_t*)&x);
}
void enableBlend(){
	glEnable(GL_BLEND);
}
void disableBlend(){
	glDisable(GL_BLEND);
}
uint32_t rnd(){
	static uint32_t v=0x5A5E4943;
	return v=36969*(v&65535)+(v>>16);
}
void sprInit(){
	glfwInit();
	glfwDisable(GLFW_AUTO_POLL_EVENTS);
	glfwOpenWindow(320,512,0,0,0,0,0,0,GLFW_WINDOW);
	glOrtho(0,160,0,256,1,-1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glGenTextures(1,&Stx);
	glBindTexture(GL_TEXTURE_2D,Stx);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,SF,SW,SH,0,SF,GL_UNSIGNED_BYTE,S);
}
int sprMenu(){
	char ipstr[17],*ipstrp=ipstr,lch;
	do{
		int in=sprInput();
		if(gA(in))Pt=0;
		else(gD(in))Pt=1;
		char llch=lch;
		if(glfwGetKey(GLFW_KEY_BACKSPACE)&&ipstrp>ipstr)
			ipstrp--;
		else(ipstrp-ipstr<17){
			if(glfwGetKey('T'))lch=*ipstrp++='t';
			else(glfwGetKey('.'))lch=*ipstrp++='.';
			else{
				for(int i='0';i<='9';i++)
					if(glfwGetKey(i)){
						lch=*ipstrp++=i;
						goto lchset;
					}
				lch=0;
			}
		}
	lchset:
		if(lch&&llch==lch)ipstrp--;
		sprBeginFrame();
		notex();
		rndcol();
		glRect(0,0,160,256);
		int x=0;
		for(char*p=ipstr;p<ipstrp;p++){
			rndrndcol();
			tfChar(x,32,*p);
			x+=6;
		}
		retex();
		drawSpr(Pt?Ika:Kae,16,16,0,Pt?shb:shr);
		sprEndFrame(1./20);
	}while(!glfwGetKey(GLFW_KEY_ENTER));
	*ipstrp=0;
	return netinit(ipstr);
}
void sprBeginFrame(){
	*(uint32_t*)rcol=rnd();
	glClear(GL_COLOR_BUFFER_BIT);
}
void sprEndFrame(float fps){
	glfwSwapBuffers();
	double gT=fps-glfwGetTime();
	if(gT>0)glfwSleep(gT);
	else printf("sleep %f\n",gT);
	glfwSetTime(0);
}
int sprInput(){
	glfwPollEvents();
	if(glfwGetKey(GLFW_KEY_ESC)||!glfwGetWindowParam(GLFW_OPENED)){
		nsend(0,0);
		exit(0);
	}
	return glfwGetKey('Z')|glfwGetKey('X')<<1|glfwGetKey(GLFW_KEY_RIGHT)<<2|glfwGetKey(GLFW_KEY_LEFT)<<3|glfwGetKey(GLFW_KEY_DOWN)<<4|glfwGetKey(GLFW_KEY_UP)<<5;
}
typedef enum{Ika,Kae,LSPR}sprid;
extern struct spr{
	short x,y;
	unsigned char w,h;
}spr[LSPR];
void drawRect_(int x,int y,int w,int h,float tx,float ty,float tw,float th);
void drawRect(int x,int y,int w,int h,float tx,float ty,float tw,float th);
void drawSpr(sprid s,int x,int y,int f,const uint8_t*c);
void glCirc(float x,float y,float r);
void glLine(float x1,float y1,float x2,float y2);
typedef enum{Ika,Kae,LSPR}sprid;
void notex();
void retex();
void drawRect_(int x,int y,int w,int h,float tx,float ty,float tw,float th);
void drawRect(int x,int y,int w,int h,float tx,float ty,float tw,float th);
void drawSpr(sprid s,int x,int y,int f,const uint8_t*c);
void glCirc(float x,float y,float r);
void glLine(float x1,float y1,float x2,float y2);
void glLineC(float x1,float y1,float x2,float y2,colt c);
void glTriangle(float,float,float,float,float,float);
void glLzr();
void sprInit();
void sprBeginFrame();
void sprEndFrame(float);
int sprInput();
void glColor(colt);
void rndcol();
void rndrndcol();
void enableBlend();
void disableBlend();
void glRect(float,float,float,float);
int sprKey(int);
void tfChar(float x,float y,int c);
#define KEY_ENTER (256+38)
#define KEY_BACKSPACE (256+39)
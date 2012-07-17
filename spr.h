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
void sprEndFrame(int fskip);
int sprInput();
void glColor(colt);
void rndcol();
void rndrndcol();
void enableBlend();
void disableBlend();
void glRectf(float,float,float,float);
void glRecti(int,int,int,int);
void glVertex2f(float,float);
void glVertex2fv(const float*);
#define GL_POINTS 0
#define GL_LINES 1
#define GL_LINE_LOOP 2
#define GL_LINE_STRIP 3
#define GL_TRIANGLES 4
#define GL_TRIANGLE_STRIP 5
#define GL_TRIANGLE_FAN 6
#define GL_QUADS 7
#define GL_QUAD_STRIP 8
#define GL_POLYGON 9
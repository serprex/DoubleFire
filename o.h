struct jray;
typedef struct obj{
	uint8_t t,w,h;
	uint16_t x,y;
	struct jray*c;
	struct obj*o;
	uint8_t d[];
}obj;
typedef struct jray{
	int n;
	obj*o[];
}jray;
//int getbxyi(int x,int y);
//int getbxy(int x,float dy);
obj*omake(int sz,int t,int x,float y,int w,int h);
int ohit(obj*a,obj*b);
static inline int pino(int x,int y,obj*o){
	return x>=o->x&&x<=o->x+o->w&&y>=o->y&&y<=o->y+o->h;
}
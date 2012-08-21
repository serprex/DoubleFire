typedef union{
	uint8_t a[24];
	struct{
		uint8_t t;
		int8_t h;
		uint16_t c;
		float x,y,xd,yd,d;
	};
}obje;
typedef struct{
	float x,y,xd,yd;
	uint16_t p;
}bxy;
enum{
	CAN,TAR,ROT,DOG,POO,B1,B2,B3,MIS
};
extern obje E[64],*Etop;
extern bxy B[8192],*Btop,PB[256],*PBtop;
void mkpb(int p,float x,float y,float xd,float yd);
void mkb(int p,float x,float y,float xd,float yd);
void mkbd(int p,float x,float y,float v,float d);
void mkbxy(int p,float x,float y,float xx,float yy,float v);
void eloop();
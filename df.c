#define FIRE
#include "df.h"
const uint8_t col[13]={255,0,0,255,255,255,63,47,95,255,0,0,0};
colt red=col,blu=col+1,wht=col+3,shr=col+5,shb=col+7,blk=col+10;
uint8_t Pt,Pf[2],Pi;
int8_t Pe=127;
int Ph[2];
uint16_t Php[2];
float Px[2]={32,96},Py[2]={96,96},Lzr[32][2];
int Lzo,Box,Boy,Bor;
int main(int argc,char**argv){
	sprInit();
	int isudp;
	if(argc<2)
		isudp=sprMenu();
	else{
		if(argc==3)Pt=atoi(argv[2]);
		isudp=netinit(argv[1]);
	}
	if(isudp)rwInit();
	genL1();
	for(;;){
		rwBegin(isudp);
		if(T==MT){
			rwInput(isudp);
			sprBegin();
			notex();
			disableBlend();
		}
		if(Pe<0){
			setPx(0,20);
			setPx(1,20);
			setPe(127);
		}
		mke();
		if(Bor){
			if(Bor==24)setBor(0);
			else setBor(Bor+1);
		}
		for(int i=0;i<2;i++){
			float Pxx=(gD(gpin(i))-gA(gpin(i)))*2,Pyy=(gW(gpin(i))-gS(gpin(i)))*2;
			if(Pxx&&Pyy){
				Pxx*=M_SQRT2;
				Pyy*=M_SQRT2;
			}
			if(Pxx)
				setPx(i,fminf(fmaxf(Px[i]+Pxx,8),120));
			if(Pyy)
				setPy(i,fminf(fmaxf(Py[i]+Pyy,8),248));
			if(Pf[i]){
				if(Pf[i]<8)setPf(i,Pf[i]+1);
				else setPf(i,8+(Pf[i]+1&7));
				if(Pe&&(Pf[i]&3)==1){
					decPe();
					if(i){
						mkpb(1,Px[1]-2,Py[1]-4,0,3);
						mkpb(1,Px[1]+2,Py[1]-4,0,3);
					}else{
						mkpb(0,Px[0],Py[0],0,-4);
						float xd=64-Px[0],yd=128-Py[0],xy=sqrt(xd*xd+yd*yd)?:1;
						mkpb(0,Px[0],Py[0],xd*3/xy,yd*3/xy);
					}
				}
			}
			if(gZ(gpin(i))){
				if(!Pf[i])
					onPf(i);
			}else{
				setPf(i,0);
			}
			if(gX(gpin(i))){
				if(i){
					if(!Lzo){
						onLzo();
						if(T==MT)
							for(int i=0;i<32;i++){
								Lzr[i][0]=Px[1];
								Lzr[i][1]=Py[1]-4;
							}
					}
				}else(!Bor)
					onBor();
			}else(Lzo&&i)
				offLzo();
		}
		if(PBtop>PB)markpb();
		for(bxy*b=PB;b<PBtop;b++){
		pbagain:
			b->x+=b->xd;
			b->y+=b->yd;
			if(b->y<0||b->x<0||b->x>128||b->y>256){
				if(killpb(b))break;
				goto pbagain;
			}else(T==MT){
				glColor(red+b->p);
				glRect(b->x-1,b->y-1,b->x+1,b->y+1);
				glColor(b->p?wht:blk);
				glRect(b->x-.5,b->y-.5,b->x+.5,b->y+.5);
			}
		}
		Ph[0]=Ph[1]=2;
		Php[0]=Php[1]=0;
		if(T==MT)rndcol();
		eloop();
		if(Pi)
			decPi();
		else{
			if(Lzo&&Ph[1]==1)Ph[1]=0;
			for(int i=0;i<2;i++)
				if(Ph[i]==1&&Pe<127)
					incPe();
				else(Ph[i]<1){
					plHit();
					break;
				}
		}
		if(T==MT){
			if(Bor){
				glColor(wht);
				glCirc(Box,Boy,Bor);
			}
			glColor(blk);
			glRect(128,0,160,256);
			rndcol();
			glRect(128,0,136,Pe*2);
			rwDrawLag(isudp);
			enableBlend();
			if(Lzo){
				memmove(Lzr+1,Lzr,248);
				int r=rnd();
				Lzr[0][0]=Px[1]-1+((r>>2)%3);
				Lzr[0][1]=Py[1]-3+(r&3);
				glLzr();
			}
			retex();
			drawSpr(Kae,Px[0]-3,Py[0]-4,(Pf[0]&3)>1,shr);
			drawSpr(Ika,Px[1]-3,Py[1]-4,(Pf[1]&3)>1,shb);
			sprEnd(rwfps());
			MT++;
		}
		T++;
		rwEnd(isudp);
	}
}
#define FIRE
#include "df.h"
#define gZ(x) (!!((x)&1))
#define gX(x) (!!((x)&2))
#define gD(x) (!!((x)&4))
#define gA(x) (!!((x)&8))
#define gS(x) (!!((x)&16))
#define gW(x) (!!((x)&32))
uint16_t T,MT;
static uint16_t mnT,mxT,moT;
const uint8_t col[13]={255,0,0,255,255,255,63,47,95,255,0,0,0};
colt red=col,blu=col+1,wht=col+3,shr=col+5,shb=col+7,blk=col+10;
uint8_t*pin,Pf[2],Pi;
int8_t Pe=127;
int Pt,Ph[2];
uint16_t Php[2];
float Px[2]={32,96},Py[2]={160,160},Lzr[32][2];
int Lzo,Box,Boy,Bor;
void stepBack(int n){
	printf("sb %d %d",n,crw);
	for(;;){
		while(rw[crw].n){
			uint8_t a=r8();
			switch(a){
			case(2 ... 3)Px[a-2]=rfloat();
			case(4 ... 5)Py[a-4]=rfloat();
			case(6)Pe=r8();
			case(7)Etop--;
			case(8)Lp-=r8();printf("!!!!\n");
			case(9){
				obje*e=E+r8();
				*++Etop=*e;
				*e=robje();
			}
			case(10 ... 11)Pf[a-10]=0;
			case(12)PBtop--;
			case(13){
				obje*e=E+r8();
				*++Etop=*e;
				*e=robje();
			}
			case(14)
				for(bxy*b=PB;b<PBtop;b++){
					b->x-=b->xd;
					b->y-=b->yd;
				}
			case(15)Pi++;
			case(16)Pe--;
			case(17)
				Btop=B+r16();
				for(bxy*b=Btop;b>=B;b--)
					*b=rbxy();
			case 18:
				Pe+=48;
				Pi=0;
			case(19)PBtop--;
			case(20){
				bxy*b=PB+r8();
				*PBtop++=*b;
				*b=rbxy();
			}
			case(21){
				bxy*b=B+r16();
				*++Btop=*b;
				*b=rbxy();
			}
			case(22 ... 23)Pf[a-22]=r8();
			case(24)
				Px[0]=128-Px[0];
				Py[0]=256-Py[0];
				Bor=0;
			case(25)
				for(obje*e=E;e<=Etop;e++){
					switch(e->t&127){
					case(EROT)
						e->d-=M_PI/(e->t&128?128:-128);
					case ECAN:
						e->x-=e->xd;
						e->y-=e->yd;
					}
				}
				for(bxy*b=B;b<=Btop;b++){
					b->x-=b->xd;
					b->y-=b->yd;
				}
			case(26)Btop--;
			case(28){
				obje*e=E+r8();
				e->d=rfloat();
			}
			case(29){
				obje*e=E+r8();
				e->h=r8();
			}
			case(30){
				obje*e=E+r8();
				e->yd=rfloat();
				e->xd=rfloat();
			}
			case(31){
				obje*e=E+r8();
				e->y=r16();
			}
			case(32 ... 33)Lzo=a-32;
			case(34){
				obje*e=E+r8();
				int xy=r8(),x=xy-1&3,y=xy-1>>2;
				e->a[xy]^=1;
				if(x<3)e->a[xy+1]^=1;
				if(x>0)e->a[xy-1]^=1;
				if(y<3)e->a[xy+4]^=1;
				if(y>0)e->a[xy-4]^=1;
			}
			case(35 ... 36)E[r8()].a[a-17]--;
			case(37)Bor=r8();
			case(38)E[r8()].h++;
			case(40)Pe++;
			case(41){
				obje*e=E+r8();
				e->y=rfloat();
				e->x=rfloat();
			}
			}
		}
		T--;
		crw--;
		cpi-=2;
		if(!--n||crw==-1){
			printf(":%d\n",crw);
			return;
		}
	}
}
int main(int argc,char**argv){
	sprInit();
	genL1();
	int isudp;
	if(argc<2){
		char ipstr[17],*ipstrp=ipstr,lch;
		do{
			int in=sprInput();
			if(gA(in)){
				Pt=0;
			}else(gD(in)){
				Pt=1;
			}
			char llch=lch;
			if(sprKey(KEY_BACKSPACE)&&ipstrp>ipstr)
				ipstrp--;
			else(ipstrp-ipstr<17){
				if(sprKey('T'))lch=*ipstrp++='t';
				else(sprKey('.'))lch=*ipstrp++='.';
				else{
					for(int i='0';i<='9';i++)
						if(sprKey(i)){
							lch=*ipstrp++=i;
							goto lchset;
						}
					lch=0;
				}
			}
			lchset:if(lch&&llch==lch)ipstrp--;
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
		}while(!sprKey(KEY_ENTER));
		*ipstrp=0;
		isudp=netinit(ipstr);
	}else{
		Pt=atoi(argv[1]);
		isudp=netinit(argv[2]);
	}
	if(isudp){
		notex();
		for(;;){
			nsend(0,0);
			if(any()){
				nrecv(0,0);
				break;
			}
			sprInput();
			sprBeginFrame();
			rndcol();
			glRect(0,0,160,256);
			sprEndFrame(1./5);
		}
		retex();
	}
	for(;;){
		printf("-%d\n",T);
		int shift=min(mnT,T)-rwT;
		if(shift>0){
			printf("<<%d %d %d\n",shift,rwp,rwT);
			crw-=shift;
			rwT+=shift;
			for(int i=0;i<shift;i++)
				free(rw[i].p);
			memmove(rw,rw+shift,sizeof(frame)*(rwp-=shift));
		}
		if(++crw==rwp){
			rw=realloc(rw,sizeof(frame)*++rwp);
			rw[crw].p=0;
			rw[crw].n=0;
		}
		shift=(min(min(mnT,T),moT)-piT)*2;
		if(shift>0){
			cpi-=shift;
			piT+=shift;
			memmove(pin,pin+shift,pip-=shift);
		}
		if((cpi+=2)==pip){
			pin=realloc(pin,pip+=2);
			memset(pin+cpi,cpi<2?0:pin[cpi-2+!Pt]&127,2);
		}
		if(Pe<0){
			wfloat(Px[0]);
			w8(2);
			wfloat(Px[1]);
			w8(3);
			w8(Pe);
			w8(6);
			Px[0]=Px[1]=20;
			Pe=127;
		}
		mke();
		if(T==MT){
			printf("==%d\n",T);
			sprBeginFrame();
			if(isudp){
				int len=3;
				uint8_t pcm[5];
				*(uint16_t*)pcm=T;
				pcm[2]=pin[cpi+Pt]=sprInput();
				if(mxT-mnT>4){
					len+=2;
					*(uint16_t*)(pcm+3)=mnT;
				}
				nsend(pcm,len);
			}else{
				pin[cpi+Pt]=sprInput();
				nsend(pin+cpi+Pt,1);
			}
			notex();
			disableBlend();
		}
		for(int i=0;i<2;i++){
			float Pxx=(gD(pin[cpi+i])-gA(pin[cpi+i]))*1.5,Pyy=(gS(pin[cpi+i])-gW(pin[cpi+i]))*1.5;
			if(Pxx&&Pyy){
				Pxx*=M_SQRT2;
				Pyy*=M_SQRT2;
			}
			if(Pxx){
				wfloat(Px[i]);
				w8(2+i);
				Px[i]=fminf(fmaxf(Px[i]+Pxx,8),120);
			}
			if(Pyy){
				wfloat(Py[i]);
				w8(4+i);
				Py[i]=fminf(fmaxf(Py[i]+Pyy,8),248);
			}
			if(Pf[i]){
				w8(Pf[i]);
				w8(22+i);
				if(Pf[i]<8)Pf[i]++;
				else Pf[i]=8+(Pf[i]+1&7);
				if(Pe&&(Pf[i]&3)==1){
					w8(40);
					Pe--;
					if(i){
						mkpb(1,Px[1]-2,Py[1]-4,0,-3);
						mkpb(1,Px[1]+2,Py[1]-4,0,-3);
					}else{
						mkpb(0,Px[0],Py[0],0,4);
						float xd=64-Px[0],yd=128-Py[0],xy=sqrt(xd*xd+yd*yd)?:1;
						mkpb(0,Px[0],Py[0],xd*3/xy,yd*3/xy);
					}
				}
			}
			if(gZ(pin[cpi+i])){
				if(!Pf[i]){
					w8(10+i);
					Pf[i]=1;
				}
			}else{
				w8(Pf[i]);
				w8(22+i);
				Pf[i]=0;
			}
			if(gX(pin[cpi+i])){
				if(i){
					if(!Lzo){
						w8(32);
						Lzo=1;
						if(T==MT)
							for(int i=0;i<32;i++){
								Lzr[i][0]=Px[1];
								Lzr[i][1]=Py[1]-4;
							}
					}
				}else(!Bor){
					w8(24);
					Px[0]=128-Px[0];
					Py[0]=256-Py[0];
					Bor=1;
					Box=128-Px[0];
					Boy=256-Py[0];
				}
			}else(Lzo&&i){
				w8(33);
				Lzo=0;
			}
		}
		if(Bor){
			w8(Bor);
			w8(37);
			if(++Bor>24)Bor=0;
		}
		if(PBtop>PB)w8(14);
		for(bxy*b=PB;b<PBtop;b++){
			b->x+=b->xd;
			b->y+=b->yd;
			if(b->y<0||b->x<0||b->x>128||b->y>256){
				wbxy(*b);
				w8(b-PB);
				w8(20);
				*b--=*--PBtop;
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
		printf("Pi %d\n",Pi);
		if(Pi){
			printf("PI %d Pe",Pe);
			w8(15);
			Pi--;
		}else{
			printf("PH %d %d\n",Ph[0],Ph[1]);
			if(Lzo&&Ph[1]==1)Ph[1]=0;
			for(int i=0;i<2;i++)
				if(Ph[i]==1&&Pe<127){
					w8(16);
					Pe++;
				}else(Ph[i]<1){
					if(Btop>=B){
						for(bxy*b=B;b<=Btop;b++)
							wbxy(*b);
						w16(Btop-B);
						w8(17);
						Btop=B-1;
					}else w8(18);
					Pe-=48;
					Pi=64;
					break;
				}
		}
		if(T==MT){
			if(Bor){
				glColor(wht);
				glCirc(Box,Boy,Bor);
			}
			rndcol();
			glRect(128,0,136,Pe*2);
			glRect(136,64,144,64+T-mnT);
			glRect(144,64,152,64+T-mxT);
			enableBlend();
			if(Lzo){
				memmove(Lzr+1,Lzr,248);
				Lzr[0][0]=Px[1]+(rand()%3)-1;
				Lzr[0][1]=Py[1]-3+(rand()&3);
				glLzr();
			}
			retex();
			drawSpr(Kae,Px[0]-3,Py[0]-4,(Pf[0]&3)>1,shr);
			drawSpr(Ika,Px[1]-3,Py[1]-4,(Pf[1]&3)>1,shb);
			sprEndFrame(mxT>T?0:1./30);
			MT++;
		}
		T++;
		while(any()){
			if(isudp){
				uint8_t ubu[6];
				int len=nrecv(ubu,5);
				uint8_t oin=ubu[2];
				if(!len){
					if(++welt&1)nsend(0,0);
				}else(len==1){
					return 0;
				}else(len>=3){
					uint16_t mt=*(uint16_t*)ubu;
					printf("udp%d: %d %d\n",len,mt,oin);
					if((mt-piT)*2>=pip){
						int hip=pip;
						pin=realloc(pin,pip=(mt-piT+1)*2);
						memset(pin+hip,oin,pip-hip);
					}
					if(!(pin[(mt-piT)*2+!Pt]&128)){
						if(len==5){
							uint16_t rt=*(uint16_t*)(ubu+3);
							if(rt>=moT){
								moT=rt;
								ubu[5]=pin[(rt-piT)*2+Pt];
								printf("ubu5pin %d\n",(rt-piT)*2+Pt);
								nsend(ubu+3,3);
							}
						}
						pin[(mt-piT)*2+!Pt]=oin|128;
						if(mt<T){
							for(int i=(1+mt-piT)*2+!Pt;i<pip;i+=2){
								if(pin[i]&128)break;
								pin[i]=oin;
							}
							stepBack(T-mt);
						}
						if(mt==mnT)mnT++;
						if(mt>mxT)mxT=mt;
					}
				}
			}else{
				uint8_t oin;
				if(nrecv(&oin,1)==-1)return 0;
				if((mnT-piT)*2>=pip){
					int hip=pip;
					pin=realloc(pin,pip=(mnT-piT+1)*2);
					memset(pin+hip,oin,pip-hip);
				}
				if(pin[(mnT-piT)*2+!Pt]!=oin){
					pin[(mnT-piT)*2+!Pt]=oin|128;
					if(mnT<T){
						for(int i=(1+mnT-piT)*2+!Pt;i<pip;i+=2){
							if(pin[i]&128)break;
							pin[i]=oin;
						}
						stepBack(T-mnT);
					}
				}else pin[(mnT-piT)*2+!Pt]|=128;
				mnT++;
				mxT=mnT-1;
			}
		}
	}
}
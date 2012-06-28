#include "df.h"
/*int getbxyi(int x,int y)
{
	x>>=3;
	y>>=3;
	return mapB[(2047-y)*256+(x>>3)]&1<<(x&7);
}
int getbxy(int x,float dy)
{
	return getbxyi(x,ceil(dy));
}*/
int ohit(obj*a,obj*b){
	int ax2=a->x+a->w,ay2=a->y+a->h,bx2=b->x+b->w,by2=b->y+b->h;
	return a!=b&&(pino(a->x,a->y,b)||pino(ax2,a->y,b)||pino(a->x,ay2,b)||pino(ax2,ay2,b)||pino(b->x,b->y,a)||pino(bx2,b->y,a)||pino(b->x,by2,a)||pino(bx2,by2,a)||a->x>b->x&&a->x<bx2&&a->y>b->y&&a->y<by2||b->x>a->x&&b->x<ax2&&b->y>a->y&&b->y<ay2);
}
void orm(obj*qo,obj*o){//UNSAFE
	for(;qo;qo=qo->o)
		if(qo->o==o){
			qo->o=o->o;
			break;
		}
}
void jadd(jray**l,obj*o){
	jray*ll;
	if(*l){
		(*l)->n++;
		ll=*l=realloc(*l,sizeof(jray)+(*l)->n*sizeof(obj*));
	}else{
		ll=*l=malloc(sizeof(jray)+sizeof(obj*));
		ll->n=1;
	}
	ll->o[ll->n-1]=o;
}
obj*omake(int sz,int t,int x,float y,int w,int h){
	obj*o=malloc(sizeof(obj)+sz);
	o->t=t;
	o->x=x;
	o->y=y;
	o->w=w;
	o->h=h;
	o->o=0;
	o->c=calloc(1,sizeof(jray));
	memset(&o->d,0,sz);
	return o;
}
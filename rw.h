extern uint16_t T,MT;
void rwBegin();
void rwEnd();
void rwDrawLag();
void incBor();
void setBor(uint8_t);
void setPx(int,float);
void setPy(int,float);
void setPf(int,uint8_t);
void onPf(int);
void setPe(int8_t);
void decPe();
void incPe();
void onLzo();
void onBor();
void decPi();
void plHit();
void marklp(uint8_t);
void markpb();
void marke();
void rwInit();
void offLzo();
void setLzr();
void pushLzr();
void incPB();
void incB();
void incE();
void deceh(obje*);
void seteh(obje*,int8_t h);
void setexy(obje*,float,float);
void seted(obje*,float);
void setexdyd(obje*,float,float);
void eb2xy(obje*,int,int,int);
void ince18(obje*);
void ince19(obje*);
void add2ey(obje*);
double rwfps();
int gpin(int);
int killpb(bxy*);
int killb(bxy*);
int kille(obje*);
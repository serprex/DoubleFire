#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <GL/glfw.h>
#include <errno.h>
#include <sys/unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "tgen.h"
#include "o.h"
#include "spr.h"
#define case(x) break;case x:;
#define else(x) else if(x)
int tcp,udp;
int any(int s){
	struct pollfd pfd={.fd=s,.events=POLLIN};
	while((s=poll(&pfd,1,0))==-1);
	return s;
}
int readch(int s){
	uint8_t c;
	ssize_t A;
	while((A=read(s,&c,1))==-1);
	return A?c:-1;
}
int sendch(int s,uint8_t c){
	while(send(s,&c,1,MSG_MORE)==-1);
	return c;
}
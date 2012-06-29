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
#include "lv.h"
#include "spr.h"
#define case(x) break;case x:;
#define else(x) else if(x)
#define sqr(x) ({__typeof__(x) _x=(x);_x*_x;})
#define dst2(x1,y1,x2,y2) sqr((x1)-(x2))+sqr((y1)-(y2))

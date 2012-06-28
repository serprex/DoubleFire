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
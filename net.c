#include "df.h"
static int udp;
#ifdef _WIN32
#include <winsock2.h>
struct timeval timeout;
int any(){
struct timeval timeout={};
	struct fd_set fds={};
	FD_SET(udp,&fds);
    return select(0,&fds,0,0,&timeout);
}
#else
#include <sys/unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
static struct pollfd pfd={.events=POLLIN};
int any(){
	return poll(&pfd,1,0);
}
#endif
int nsend(void*p,int n){
	return send(udp,p,n,0);
}
int nrecv(void*p,int n){
	return recv(udp,p,n,0);
}
int netinit(char*ipstr){
	#ifdef _WIN32
	WSADATA WsaData;
    WSAStartup(MAKEWORD(2,2),&WsaData);
	#endif
	int type;
	if(ipstr&&*ipstr=='t'){
		type=SOCK_STREAM;
		if(!*++ipstr)ipstr=0;
	}else type=SOCK_DGRAM;
	if(ipstr&&!*ipstr)ipstr=0;
	udp=socket(AF_INET,type,0);
	struct sockaddr_in udpip={.sin_family=AF_INET,.sin_addr.s_addr=htonl(INADDR_ANY),.sin_port=htons(2000+!!ipstr)};
	bind(udp,(struct sockaddr*)&udpip,sizeof(udpip));
	if(ipstr){
		struct sockaddr_in ip={.sin_family=AF_INET,.sin_port=htons(2000)};
		ip.sin_addr.s_addr=inet_addr(ipstr);
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}else{
		if(type==SOCK_STREAM){
			listen(udp,1);
			udp=accept(udp,0,0);
		}else{
			struct sockaddr_in ip;
			int sip=sizeof(ip);
			recvfrom(udp,0,0,0,(struct sockaddr*)&ip,&sip);
			connect(udp,(struct sockaddr*)&ip,sizeof(ip));
		}
	}
	#ifndef _WIN32
	pfd.fd=udp;
	#endif
	if(type==SOCK_STREAM){
		int one=1;
		setsockopt(udp,IPPROTO_TCP,TCP_NODELAY,(char*)&one,sizeof(int));
		if(ipstr){
			recv(udp,&Pt,1,0);
			Pt^=1;
		}else send(udp,&Pt,1,0);
		return 0;
	}
	notex();
	for(;;){
		uint8_t pt=Pt,*ptp=ipstr?&Pt:&pt;
		nsend(ptp,1);
		if(any()){
			nrecv(ptp,1);
			*ptp^=1;
			break;
		}
		sprInput();
		sprBeginFrame();
		rndcol();
		glRect(0,0,160,256);
		sprEndFrame(1./5);
	}
	retex();
	return 1;
}
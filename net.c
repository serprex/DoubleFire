static int udp;
#ifdef _WIN32
#include <winsock2.h>
int any(){
	struct timeval timeout={};
	struct fd_set fds;
    FD_ZERO(&fds);
    FD_SET(udp,&fds);
    return select(0,&fds,0,0,&timeout);
}
#else
#include <sys/unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
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
void netinit(char*ipstr){
	#ifdef _WIN32
	WSADATA WsaData;
    WSAStartup(MAKEWORD(2,2),&WsaData);
	#endif
	udp=socket(AF_INET,SOCK_DGRAM,0);
	#ifndef _WIN32
	pfd.fd=udp;
	#endif
	struct sockaddr_in udpip={.sin_family=AF_INET,.sin_addr.s_addr=htonl(INADDR_ANY),.sin_port=2000+!!ipstr};
	bind(udp,(struct sockaddr*)&udpip,sizeof(udpip));
	if(ipstr){
		struct sockaddr_in ip={.sin_family=AF_INET,.sin_port=2000+!ipstr};
		ip.sin_addr.s_addr=inet_addr(ipstr);
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}else{
		struct sockaddr_in ip;
		size_t sip=sizeof(ip);
		recvfrom(udp,0,0,0,(struct sockaddr*)&ip,&sip);
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}
}
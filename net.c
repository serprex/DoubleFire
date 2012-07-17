#include <sys/unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <arpa/inet.h>
static int udp;
static struct pollfd pfd={.events=POLLIN};
int any(){
	return poll(&pfd,1,0);
}
int nsend(void*p,int n){
	return write(udp,p,n);
}
int nrecv(void*p,int n){
	return read(udp,p,n);
}
void netinit(char*ipstr){
	udp=socket(AF_INET,SOCK_DGRAM,0);
	pfd.fd=udp;
	struct sockaddr_in udpip={.sin_family=AF_INET,.sin_addr.s_addr=htonl(INADDR_ANY),.sin_port=2000+!!ipstr};
	bind(udp,(struct sockaddr*)&udpip,sizeof(udpip));
	if(ipstr){
		struct sockaddr_in ip={.sin_family=AF_INET,.sin_port=2000+!ipstr};
		inet_aton(ipstr,&ip.sin_addr);
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}else{
		struct sockaddr_in ip;
		socklen_t sip=sizeof(ip);
		recvfrom(udp,0,0,0,(struct sockaddr*)&ip,&sip);
		connect(udp,(struct sockaddr*)&ip,sizeof(ip));
	}
}
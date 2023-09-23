#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>
#include "jagsock.h"

JNIEXPORT void JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1init(JNIEnv *env, jclass obj) {
}

JNIEXPORT jint JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1listen__I(JNIEnv *env, jclass obj, jint port) {
	//Variables
	int fd;
	struct sockaddr_in tcpSock;
	unsigned long val_one=1;
	//Create socket
	fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (fd<0) return -2;
	//Set socket options
	if (setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(char *)&val_one,sizeof(val_one))!=0) return -4;
	if (fcntl(fd,F_SETFL,O_NONBLOCK)<0) return -5;
	if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(int *)&val_one,sizeof(val_one))!=0) return -6;
	//Create sockaddr_in structure
	memset(&tcpSock,0,sizeof(tcpSock));
	tcpSock.sin_family = AF_INET;
	tcpSock.sin_addr.s_addr = INADDR_ANY;
	tcpSock.sin_port = htons((short)port);
	//Bind socket
	if (bind(fd,(struct sockaddr *)&tcpSock,sizeof(tcpSock))!=0) return -3;
	//Listen on port
	if (listen(fd,500)!=0) return -4;
	//Done
	return fd;
}

JNIEXPORT jint JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1listen__II(JNIEnv *env, jclass obj, jint ip, jint port) {
	//Variables
	int fd;
	struct sockaddr_in tcpSock;
	unsigned long val_one=1;
	//Create socket
	fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (fd<0) return -2;
	//Set socket options
	if (setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(char *)&val_one,sizeof(val_one))!=0) return -4;
	if (fcntl(fd,F_SETFL,O_NONBLOCK)<0) return -5;
	if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(int *)&val_one,sizeof(val_one))!=0) return -6;
	//Create sockaddr_in structure
	memset(&tcpSock,0,sizeof(tcpSock));
	tcpSock.sin_family = AF_INET;
	tcpSock.sin_port = htons((short)port);
	tcpSock.sin_addr.s_addr = htonl(ip);
	//Bind socket
	if (bind(fd,(struct sockaddr *)&tcpSock,sizeof(tcpSock))!=0) return -3;
	//Listen on port
	if (listen(fd,500)!=0) return -4;
	//Done
	return fd;
}

JNIEXPORT jint JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1connect(JNIEnv *env, jclass obj, jint ip, jint port) {
	//Variables
	int fd;
	struct sockaddr_in socadr;
	unsigned long val_one=1;
	//Create socket
	fd=socket(AF_INET,SOCK_STREAM,0);
	if (fd<0) return -2;
	//Set socket options
	if (setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(char *)&val_one,sizeof(val_one))!=0) return -4;
	if (fcntl(fd,F_SETFL,O_NONBLOCK)<0) return -5;
	//Create sockaddr_in structure
	memset(&socadr,0,sizeof(socadr));
	socadr.sin_family = AF_INET;
	socadr.sin_port=htons((short)port);
	socadr.sin_addr.s_addr = htonl(ip);
	//Connect socket
	if (connect(fd,(struct sockaddr *)&socadr,sizeof(socadr))!=0) {
		if (errno!=EINPROGRESS) return -3;
	}
	//Done
	return fd;
}

JNIEXPORT jint JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1isconnected(JNIEnv *env, jclass obj, jint fd, jint ip, jint port) {
	//Create sockaddr_in structure
	struct sockaddr_in socadr;
	memset(&socadr,0,sizeof(socadr));
	socadr.sin_family = AF_INET;
	socadr.sin_port=htons((short)port);
	socadr.sin_addr.s_addr = htonl(ip);
	//Check if socket is connected yet
	if (connect(fd,(struct sockaddr *)&socadr,sizeof(socadr))!=0) {
		if (errno == EINPROGRESS) return 0;
   		if (errno == EALREADY) return 0;
    		if (errno == EISCONN) return 1;
    		return -1;
	}
	return 1;
}

JNIEXPORT jint JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1accept(JNIEnv *env, jclass obj, jint fd, jintArray ipbuf) {
	jint *ipbuf2;
	unsigned long val_one=1;
	int newfd;
	struct sockaddr_in clientAddr; int casize=sizeof(clientAddr);
	//Do accept
	newfd = accept(fd,(struct sockaddr*)&clientAddr,&casize);
	if (newfd==-1) {
		if (errno==EWOULDBLOCK) return -1;
		return -2;
	}
	//Store ip address
	ipbuf2=(*env)->GetIntArrayElements(env,ipbuf,0);
	ipbuf2[0]=clientAddr.sin_addr.s_addr;
	(*env)->ReleaseIntArrayElements(env,ipbuf,ipbuf2,0);
	//Set Socket options
	if (setsockopt(newfd,IPPROTO_TCP,TCP_NODELAY,(char *)&val_one,sizeof(val_one))!=0) return -4;
	if (fcntl(newfd,F_SETFL,O_NONBLOCK)<0) return -5;
	//Done
	return newfd;
}

JNIEXPORT void JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1close(JNIEnv *env, jclass obj, jint fd) {
	close(fd);
}

JNIEXPORT jint JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1read(JNIEnv *env, jclass obj, jint fd, jbyteArray buf, jint off, jint len) {
	//pin the memory in java whilst we read from it
	unsigned char* c_buf=(*env)->GetPrimitiveArrayCritical(env, buf, 0);
	int got=recv(fd,c_buf+off,len,0);
	//Read errno before doing anything else!
	int errorNumber=errno;
	//unpin the memory
	(*env)->ReleasePrimitiveArrayCritical(env, buf, c_buf, 0);
	if (got==0) return -1;
	if (got<0) { if (errorNumber==EWOULDBLOCK) return 0; else return -2 - errorNumber; }
	return got;
}

JNIEXPORT jint JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1write(JNIEnv *env, jclass obj, jint fd, jbyteArray buf, jint off, jint len) {
	//pin the memory in java whilst we read from it
	unsigned char* c_buf=(*env)->GetPrimitiveArrayCritical(env, buf, 0);
	int sent=send(fd,c_buf+off,len,0);
	//Read errno before doing anything else!
	int errorNumber=errno;
	//unpin the memory
	(*env)->ReleasePrimitiveArrayCritical(env, buf, c_buf, 0);
	if (sent<0) {
		if (errorNumber==EWOULDBLOCK) return 0;
		else if (errorNumber > 0) return -2 - errorNumber;
	}
	return sent;
}

JNIEXPORT jint JNICALL Java_com_jagex_framework_net_socket_jagsock_imp_1bufsize(JNIEnv *env, jclass obj, jint fd, jint inbuf, jint outbuf) {
	unsigned long in=inbuf;
	unsigned long out=outbuf;
	int insize=sizeof(in);
	int outsize=sizeof(out);
	//Set receive buffer
	if (setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(const char *)&in,sizeof(in))!=0) return -2;
	if (getsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char *)&in,&insize)!=0) return -3;
	if (in<inbuf) return -4;
	//Set send buffer
	if (setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(const char *)&out,sizeof(out))!=0) return -5;
	if (getsockopt(fd,SOL_SOCKET,SO_SNDBUF,(char *)&out,&outsize)!=0) return -6;
	if (out<outbuf) return -7;
	return 0;
}

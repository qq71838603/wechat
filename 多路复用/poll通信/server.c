#include "myhead.h"


int main(int argc, char const *argv[])
{
	int sock,ret ;
	int addrsize = sizeof(struct sockaddr_in);

	struct sockaddr_in bindaddr;
	struct sockaddr_in clientaddr;
	struct pollfd fds[1];

	char buf[50];

	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = inet_addr("192.168.110.7");
	bindaddr.sin_port = htons(10005);


	sock = socket(AF_INET,SOCK_STREAM,0);	
	if(sock == -1)
	{
		perror("创建套接字失败!\n");
		return -1;
	}

	int on =1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
 	ret = bind(sock,(struct sockaddr *)&bindaddr,addrsize);
 	if(ret == -1)
 	{
 		perror("绑定失败!\n");
 		return -1;
 	}

 	ret = listen(sock,5);
 	if(ret == -1)
 	{
 		perror("监听失败!\n");
 		return -1;
 	}

 	int newsocket = accept(sock,(struct sockaddr *)&clientaddr,&addrsize);
 	if(newsocket == -1)
 	{
 		perror("连接失败!\n");
 		return -1;
 	}

 	fds[0].fd = newsocket;
 	fds[0].events =  POLLRDNORM;

 	while(1)
 	{
	  	ret = poll(fds,1,-1);
	 	if(ret >0)
	 	{
	 		if(fds[0].revents & POLLRDNORM)
	 		{
	 			recv(newsocket,buf,50,0);
	 			printf("接收到的信息为%s\n",buf );
	 		}
	 	}		
 	}


 	close(sock);
 	close(newsocket);
	return 0;
}
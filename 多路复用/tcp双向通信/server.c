#include "myhead.h"
/*
	多路复用   
	server端代码
 */


int main(int argc, char const *argv[])
{
	int ret;
	int addrsize = sizeof(struct sockaddr_in);
	char buf[20];
	//定义ipv4地址结构体变量，用于存放绑定的ip和端口号
	struct sockaddr_in bindaddr;
	struct sockaddr_in clientaddr;
	
	//初始化该结构体变量
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = inet_addr("192.168.110.7");
	bindaddr.sin_port = htons(11111);

	//创建tcp套接字
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		perror("创建套接字失败!\n");
		return -1;
	}

	int on = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	//绑定
	ret = bind(sock,(struct sockaddr*)&bindaddr,addrsize);	
	if(ret == -1)
	{
		perror("绑定失败!\n");
		return -1;
	}
	//监听
	ret = listen(sock,1);
	if(ret == -1)
	{
		perror("监听失败!\n");
		return -1;
	}
	//接收连接请求
	int newsock = accept(sock,(struct sockaddr*)&clientaddr,&addrsize);
	if(newsock == -1)
	{
		perror("连接失败!\n");
		return -1;
	}
	//定义集合
	fd_set fdset;

	FD_ZERO(&fdset);
	//接收信息
	while(1)
	{
		bzero(buf,20);
		//添加监测文件描述符
		FD_SET(newsock,&fdset);
		FD_SET(0,&fdset);
		//监测
		ret = select(newsock+1,&fdset,NULL,NULL,NULL);
		if(ret == -1)
		{
			perror("select 失败!\n");
			return -1;
		}

		if(FD_ISSET(newsock,&fdset))
		{
			ret = recv(newsock,buf,20,0);
			printf("接收到的消息为%s\n",buf );
			if(strncmp(buf,"quit",4)==0)
			{
				break;
			}
			else if(ret == 0)
			{
				break;
			}
		}
		
	
		if(FD_ISSET(0,&fdset))
		{
			printf("请输入需要发送的消息\n");
			fgets(buf,20,stdin);
			send(newsock,buf,20,0);
			if(strncmp(buf,"quit",4)==0)
			{
				break;
			}


		}

	}

	//挂机
	close(sock);
	close(newsock);
	return 0;
}
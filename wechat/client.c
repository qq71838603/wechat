#include "myhead.h"

/*
	多路复用   
	server端代码
 */


int main(int argc, char const *argv[])
{
	int addrsize = sizeof(struct sockaddr_in);
	int ret;
	char buf[2000];
	//定义ipv4地址结构体变量，用于存放绑定的ip和端口号
	struct sockaddr_in recvaddr;

	//初始化该结构体变量
	bzero(&recvaddr,sizeof(recvaddr));
	recvaddr.sin_family = AF_INET;
	recvaddr.sin_addr.s_addr = inet_addr("192.168.110.8");
	recvaddr.sin_port = htons(12345);
	//创建tcp套接字
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		perror("创建tcp套接字失败\n");
		return -1;
	}
	//拨号
	ret = connect(sock,(struct sockaddr*)&recvaddr,addrsize);
	if(ret == -1)
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
		bzero(buf,2000);
		//添加监测文件描述符
		FD_SET(0,&fdset);
		FD_SET(sock,&fdset);
		//监测
		ret = select(sock+1,&fdset,NULL,NULL,NULL);
		if(ret == -1)
		{
			perror("select失败!\n");
			return -1;
		}

		if(FD_ISSET(sock,&fdset))
		{
			
			ret = recv(sock,buf,2000,0);
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
			printf("输入需要发送的信息\n");
			fgets(buf,2000,stdin);
			send(sock,buf,2000,0);
			if(strncmp(buf,"quit",4)==0)
			{
				break;
			}			
		}
	}
	//挂机
	close(sock);
	return 0;
}
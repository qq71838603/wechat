#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
/*
	使用select多路复用来实现，不用多线程
	tcp通信，实现广播(多个客户端连接服务器，其中一个客户端输入的内容，通过服务器转发给其他所有的客户端)
	客户端端的代码
*/
int iphonex;

int main(int argc,char **argv)
{
	int addrsize=sizeof(struct sockaddr_in);
	int ret;
	char buf[15];
	//定义ipv4地址结构体变量，用于存放对方的ip和端口号
	struct sockaddr_in girladdr;
	struct sockaddr_in bindaddr;
	//初始化该结构体变量
	bzero(&girladdr,sizeof(girladdr)); //memset()也行
	girladdr.sin_family=AF_INET;
	girladdr.sin_addr.s_addr=inet_addr(argv[1]);
	girladdr.sin_port=htons(atoi(argv[2]));//

	//买手机 --》创建tcp套接字
	iphonex=socket(AF_INET,SOCK_STREAM,0);
	if(iphonex==-1)
	{
		perror("create sock failed!\n");
		return -1;
	}

	//拨号，连接女朋友
	ret=connect(iphonex,(struct sockaddr *)&girladdr,addrsize);
	if(ret==-1)
	{
		perror("拨号失败!\n");
		return 0;
	}
	//定义一个集合变量用于存放你要监测的文件描述符
	fd_set myset;
	while(1)
	{
		FD_ZERO(&myset);
		//往集合中添加你要监测的对象   键盘跟iphonex
		FD_SET(iphonex,&myset);
		FD_SET(0,&myset);
		bzero(buf,15);
		//调用select来监测集合中文件描述符
		ret=select(iphonex+1,&myset,NULL,NULL,NULL);
		if(ret==-1)
		{
			perror("监测失败!\n");
			return -1;
		}
		//判断旧的套接字是否读就绪
		if(FD_ISSET(iphonex,&myset))
		{
			recv(iphonex,buf,15,0);
			printf("收到的广播信息是:%s\n",buf);
		}
		//判断键盘是否读就绪
		if(FD_ISSET(0,&myset))
		{
			printf("请输入要广播的信息!\n");
			scanf("%s",buf);
			//发送信息给服务器，然后由服务器遍历链表转发 
			send(iphonex,buf,strlen(buf),0);
		}
	}
	
	//挂机
	close(iphonex);
	return 0;
}
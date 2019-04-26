#include "myhead.h"
/*
	tcp通信，通过男女朋友的例子形象理解tcp的工作流程
	男朋友的代码
*/


int main(int argc ,char *argv[])
{
	int addrsize=sizeof(struct sockaddr_in);
	int ret;
	char buf[15];
	int iphonex;
	fd_set fdset;

	//定义ipv4地址结构体变量，用于存放对方的ip和端口号
	struct sockaddr_in girladdr;

	//初始化该结构体变量
	bzero(&girladdr,sizeof(girladdr)); //memset()也行
	girladdr.sin_family=AF_INET;
	girladdr.sin_addr.s_addr=inet_addr("192.168.110.7");
	girladdr.sin_port=htons(10005);
	
	FD_ZERO(&fdset);

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

	while(1)
	{	

		FD_SET(iphonex,&fdset);
		FD_SET(0,&fdset);
		bzero(buf,15);
		ret = select(iphonex+1,&fdset,NULL,NULL,NULL);
		if(ret == -1)
		{
			perror("监测失败\n");
			return -1;
		}

		if(FD_ISSET(iphonex,&fdset))
		{
			ret=recv(iphonex,buf,12,0);
			printf("女盆友发送过来的信息是:%s 有%d字节\n",buf,ret);
		}

		if(FD_ISSET(0,&fdset))
		{
			printf("输入要发送给女朋友的信息\n");
			fgets(buf,15,stdin);
			send(iphonex,buf,15,0);
		}
		
	}
	//挂机
	close(iphonex);
	return 0;
}
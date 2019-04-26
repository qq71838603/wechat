#include "myhead.h"
/*
    使用select多路复用来实现，不用多线程
	tcp通信，实现广播(多个客户端连接服务器，其中一个客户端输入的内容，通过服务器转发给其他所有的客户端)
	服务器端的代码
*/
int iphonex;

int main(int argc,char **argv)
{
	int addrsize=sizeof(struct sockaddr_in);
	int ret;
	char buf[20];
	//定义ipv4地址结构体变量，用于存放对方的ip和端口号
	struct sockaddr_in girladdr;

	//创建多路复用集合
	fd_set fdset;
	FD_ZERO(&fdset);


	//初始化该结构体变量
	bzero(&girladdr,sizeof(girladdr)); //memset()也行
	girladdr.sin_family=AF_INET;
	girladdr.sin_addr.s_addr=inet_addr("192.168.110.7");
	girladdr.sin_port=htons(11111);//
	

	//创建tcp套接字
	iphonex=socket(AF_INET,SOCK_STREAM,0);
	if(iphonex==-1)
	{
		perror("create sock failed!\n");
		return -1;
	}

	//拨号
	ret=connect(iphonex,(struct sockaddr *)&girladdr,addrsize);
	if(ret==-1)
	{
		perror("拨号失败!\n");
		return 0;
	}
	while(1)
	{
		FD_ZERO(&fdset);
		FD_SET(iphonex,&fdset);
		FD_SET(0,&fdset);
		bzero(buf,20);
		
		ret = select(iphonex+1,&fdset,NULL,NULL,NULL);
		if(ret == -1)
		{
			perror("监测失败!\n");
			return -1;
		}

		if(FD_ISSET(iphonex,&fdset))
		{
			recv(iphonex,buf,20,0);
			printf("接收到的广播信息为:%s\n",buf );
		}
		

		if(FD_ISSET(0,&fdset))
		{
			printf("请输入要广播的信息\n");
			fgets(buf,20,stdin);
			//发送信息给服务器，然后由服务器遍历链表转发 
			ret=send(iphonex,buf,strlen(buf),0);
			printf("发送了%d字节\n",ret);			
		}

	}
	
	//挂机
	close(iphonex);
	return 0;
}
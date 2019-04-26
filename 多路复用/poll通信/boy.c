#include "myhead.h"

/*
	使用多路复用poll去实现tcp双向通信
*/


int main()
{
	int addrsize=sizeof(struct sockaddr_in);
	int ret;
	int iphonex;
	char buf[20];
	//定义ipv4地址结构体变量，用于存放对方的ip和端口号
	struct sockaddr_in girladdr;
	struct sockaddr_in bindaddr;
	//初始化该结构体变量
	bzero(&girladdr,sizeof(girladdr)); //memset()也行
	girladdr.sin_family=AF_INET;
	girladdr.sin_addr.s_addr=inet_addr("192.168.110.4");
	girladdr.sin_port=htons(10000);
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

	//定义一个struct pollfd类型的数组用于存放你要监测的文件描述符
	//经过前面的分析：我们知道需要监测键盘跟套接字
	struct pollfd myarray[2];
	bzero(myarray,sizeof(myarray));
	myarray[0].fd=0; //键盘的文件描述符
	myarray[0].events=POLLRDNORM;
	myarray[1].fd=iphonex; //套接字文件描述符
	myarray[1].events=POLLRDNORM;
	
	while(1)
	{
		bzero(buf,20);
		//调用poll来监测文件描述符
		ret=poll(myarray,2,-1); //没有设置超时时间，永远等待
		if(ret==-1)
		{
			perror("监测文件描述符集合失败!\n");
			return -1;
		}
		else if(ret==0)
		{
			perror("超时!\n");
			return -1;
		}
		//判断键盘是否发生了读就绪
		if(myarray[0].revents&POLLRDNORM)
		{
			printf("请输入要发送的内容!\n");
			//获取键盘输入的字符串并将字符串发送出去
			scanf("%s",buf);
			send(iphonex,buf,20,0);
		}
		//判断套接字是否发生了读就绪
		if(myarray[1].revents&POLLRDNORM)
		{
			//接收对方发送过来的数据
			recv(iphonex,buf,20,0);
			printf("对方给我发送的信息是:%s\n",buf);
		}
	}
	//挂机
	close(iphonex);
	return 0;
}
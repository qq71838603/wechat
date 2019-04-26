#include "myhead.h"
/*
	使用多路复用poll去实现tcp双向通信
*/
int main()
{
	int addrsize=sizeof(struct sockaddr_in);
	int ret;
	char buf[20];
	//定义ipv4地址结构体变量，用于存放绑定的ip和端口号
	struct sockaddr_in bindaddr;
	struct sockaddr_in boyaddr; //存放对方的ip和端口号
	//初始化该结构体变量
	bzero(&bindaddr,sizeof(bindaddr)); //memset()也行
	bindaddr.sin_family=AF_INET;
	bindaddr.sin_addr.s_addr=inet_addr("192.168.110.4");//自己的ip
	bindaddr.sin_port=htons(10000);//你自己指定要绑定的端口号
	int iphonex;
	int iphonexplus;
	
	//买手机 --》创建tcp套接字
	iphonex=socket(AF_INET,SOCK_STREAM,0);
	if(iphonex==-1)
	{
		perror("create sock failed!\n");
		return -1;
	}
	//取消端口绑定限制
	int on = 1;
	setsockopt(iphonex, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	//绑定
	ret=bind(iphonex,(struct sockaddr *)&bindaddr,addrsize);
	if(ret==-1)
	{
		perror("绑定失败!\n");
		return 0;
	}
	//监听
	ret=listen(iphonex,6);
	if(ret==-1)
	{
		perror("监听失败!\n");
		return 0;
	}
	//接收连接请求
	iphonexplus=accept(iphonex,(struct sockaddr *)&boyaddr,&addrsize);//返回新的电话
	if(iphonexplus==-1)
	{
		perror("接收连接失败!\n");
		return 0;
	}
	//定义一个struct pollfd类型的数组用于存放你要监测的文件描述符
	//经过前面的分析：我们知道需要监测键盘跟新的套接字
	struct pollfd myarray[2];
	bzero(myarray,sizeof(myarray));
	myarray[0].fd=0; //键盘的文件描述符
	myarray[0].events=POLLRDNORM;
	myarray[1].fd=iphonexplus; //新的套接字文件描述符
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
		//判断键盘是否发生了读就绪
		if(myarray[0].revents&POLLRDNORM)
		{
			printf("请输入要发送的内容!\n");
			//获取键盘输入的字符串并将字符串发送出去
			scanf("%s",buf);
			send(iphonexplus,buf,20,0);
		}
		//判断新的套接字是否发生了读就绪
		if(myarray[1].revents&POLLRDNORM)
		{
			//接收对方发送过来的数据
			recv(iphonexplus,buf,20,0);
			printf("对方给我发送的信息是:%s\n",buf);
		}
	}
	//挂机
	close(iphonex);
	close(iphonexplus);
	return 0;
}
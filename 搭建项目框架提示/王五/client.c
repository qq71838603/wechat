#include "myhead.h"
/*
	tcp实现多人聊天
	多客户端连接服务器，服务器作为中转站
	客户端的代码
*/
int iphonex;
int bmpnum=1;
void *recvserver(void *arg)
{
	char rbuf[2000];
	int ret;
/* 	int bmpfd;
	char bmppath[10]={0};
	sprintf(bmppath,"%d.bmp",bmpnum++);
	bmpfd=open(bmppath,O_CREAT|O_TRUNC|O_RDWR);
	while(1)//循环分段接收图片信息，每次接收2000字节
	{
		bzero(rbuf,2000);
		ret=recv(iphonex,rbuf,2000,0);//两种情况：文件大小<=2000   文件>2000字节
		//将接收的内容写入新建的bmp文件中
		write(bmpfd,rbuf,ret);
		if(ret<2000)
			break;
	}
	close(bmpfd); */
	while(1)
	{
		bzero(rbuf,2000);
		ret=recv(iphonex,rbuf,2000,0);
		if(ret==0) //说明服务器断开连接了、整个程序全部崩溃
		{
			exit(0);//结束整个进程
		}
		printf("服务器转发过来的信息是:\n%s\n",rbuf);
	}
}

//封装显示UI
void show_ui()
{
	printf("*************************\n");
	printf("输入信息的格式如下:\n");
	printf("ip:真实信息     --》发送给对应ip的客户端\n");
	printf("server:真实信息 --》发送服务器\n");
	printf("all:真实信息    --》发送给目前连接成功的所有客户端(排除自己)\n");
	printf("输入getlist可以获取服务器上存放的用户链表信息!\n");
}
int main(int argc,char **argv)
{
	int ret;
	pthread_t id;
	char sbuf[100];
	//定义一个ipv4地址结构体变量并初始化
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;
	//由于存在字节序的问题，需要转换
	bindaddr.sin_port=htons(20002);//程序员自己指定,客户端的端口号
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY);//绑定,客户端自己的ip
	
	struct sockaddr_in girladdr;
	bzero(&girladdr,sizeof(girladdr));
	girladdr.sin_family=AF_INET;
	girladdr.sin_port=htons(10000); //服务器的端口号
	girladdr.sin_addr.s_addr=inet_addr("192.168.11.3"); //服务器自己的ip
	
	//买手机--》创建tcp套接字
	iphonex=socket(AF_INET,SOCK_STREAM,0);
	if(iphonex==-1)
	{
		perror("创建tcp失败!\n");
		return -1;
	}
	int value=1;
	//取消绑定限制
	setsockopt(iphonex,SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value));
	//绑定ip地址和端口号
	ret=bind(iphonex,(struct sockaddr *)&bindaddr,sizeof(bindaddr));
	if(ret==-1)
	{
		perror("绑定失败!\n");
		return -1;
	}
	//拨号连接服务器
	ret=connect(iphonex,(struct sockaddr *)&girladdr,sizeof(girladdr));
	if(ret==-1)
	{
		perror("连接失败!\n");
		return -1;
	}
	
	//一旦连接成功，立马上传自己的大头贴
	struct stat mystat;
	int overplus;
	int i=0;
	int fd=open(argv[1],O_RDWR);
	stat(argv[1],&mystat);
	overplus=mystat.st_size;
	char buf[mystat.st_size];
	read(fd,buf,mystat.st_size);
	close(fd);
	//分段发送给ubuntu服务器
	while(1) 
	{
		if(overplus>=2000)//i从0开始
			send(iphonex,buf+2000*i,2000,0); //两种情况：文件<=2000字节   文件>2000字节
		else
		{
			send(iphonex,buf+2000*i,overplus,0);
			break;
		}
		i++;	
		overplus=overplus-2000;
	}
	//显示人机交互界面
	show_ui();
	
	//创建线程专门接收服务器转发过来的信息
	pthread_create(&id,NULL,recvserver,NULL);
	while(1)
	{
		//循环发送信息给服务器
		bzero(sbuf,100); 
		printf("请输入要发送给服务器的信息!\n");
		scanf("%s",sbuf);
		ret=send(iphonex,sbuf,strlen(sbuf),0);
	}
	
	
	//挂机
	close(iphonex);
	return 0;
}
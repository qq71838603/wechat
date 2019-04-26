#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include "kernel_list.h"
/*
    使用select多路复用来实现，不用多线程
	tcp通信，实现广播(多个客户端连接服务器，其中一个客户端输入的内容，通过服务器转发给其他所有的客户端)
	服务器端的代码
*/
//定义一个结构体描述内核链表
typedef struct clientlist 
{
	char someip[16];//存放客户端ip
	unsigned short someport;//存放端口号 
	int somesock;//存放新的套接字
	struct list_head mylist;
}clist,*pclist;
pclist myhead;
//封装函数用于创建表头 
pclist create_head()
{
	pclist head=malloc(sizeof(clist));
	//初始化头节点 
	INIT_LIST_HEAD(&(head->mylist));
	return head;
}
int iphonexplus;
int main(int argc,char **argv)
{
	int addrsize=sizeof(struct sockaddr_in);
	int ret;
	pclist pos,otherpos;
	int maxfd; //保存集合中最大的那个套接字
	char buf[15];
	
	//定义ipv4地址结构体变量，用于存放绑定的ip和端口号
	struct sockaddr_in bindaddr;
	struct sockaddr_in boyaddr; //存放对方的ip和端口号
	//初始化该结构体变量
	bzero(&bindaddr,sizeof(bindaddr)); //memset()也行
	bindaddr.sin_family=AF_INET;
	bindaddr.sin_addr.s_addr=inet_addr(argv[1]);//自己的ip
	bindaddr.sin_port=htons(atoi(argv[2]));//你自己指定要绑定的端口号
	int iphonex;
	//准备好内核链表的表头，方便后面在死循环中将新的客户端添加到该内核链表中 
	myhead=create_head();
	//买手机 --》创建tcp套接字
	iphonex=socket(AF_INET,SOCK_STREAM,0);
	if(iphonex==-1)
	{
		perror("create sock failed!\n");
		return -1;
	}
	//绑定
	ret=bind(iphonex,(struct sockaddr *)&bindaddr,addrsize);
	if(ret==-1)
	{
		perror("绑定失败!\n");
		return -1;
	}
	//监听
	ret=listen(iphonex,6);
	if(ret==-1)
	{
		perror("监听失败!\n");
		return -1;
	}
	//定义一个集合变量用于存放你要监测的文件描述符
	fd_set myset;
	maxfd=iphonex;
	while(1)
	{
		FD_ZERO(&myset);
		bzero(buf,15);
		//往集合中添加你要监测的对象  旧的套接字跟accept产生的新套接字
		FD_SET(iphonex,&myset);
		//遍历内核链表将所有的已经连接上服务器的客户端套接字添加到集合中
		list_for_each_entry(pos,&(myhead->mylist),mylist)
		{
			FD_SET(pos->somesock,&myset);
		}
		//调用select来监测集合中文件描述符
		ret=select(maxfd+1,&myset,NULL,NULL,NULL);
		if(ret==-1)
		{
			perror("监测失败!\n");
			return -1;
		}
		//判断旧的套接字是否读就绪
		if(FD_ISSET(iphonex,&myset))
		{
			//接收连接请求，可以接收多个客户端的连接
			iphonexplus=accept(iphonex,(struct sockaddr *)&boyaddr,&addrsize);//返回新的电话
			if(iphonexplus==-1)
			{
				perror("接收连接失败!\n");
				return -1;
			}
			//打印连接上的客户端的ip和端口号
			printf("目前连接上的客户端ip是:%s 端口号:%hu\n",inet_ntoa(boyaddr.sin_addr),ntohs(boyaddr.sin_port));
			//将连接上的客户端添加到内核链表中 
			pclist newclient=calloc(1,sizeof(clist));//calloc  realloc
			//初始化新的客户端
			strcpy(newclient->someip,inet_ntoa(boyaddr.sin_addr));
			newclient->someport=ntohs(boyaddr.sin_port);
			newclient->somesock=iphonexplus;
			INIT_LIST_HEAD(&(newclient->mylist));
			//将新的客户端添加到链表中
			list_add_tail(&(newclient->mylist),&(myhead->mylist));
			//求最大的文件描述符
			if(maxfd<iphonexplus)
				maxfd=iphonexplus;
		}
		//判断客户端是否有发送数据过来
		list_for_each_entry(pos,&(myhead->mylist),mylist)
		{
			if(FD_ISSET(pos->somesock,&myset))
			{
				recv(pos->somesock,buf,15,0);
				break;
			}
		}
		list_for_each_entry(otherpos,&(myhead->mylist),mylist)
		{
			//排除自己
			if(otherpos->somesock!=pos->somesock)
				send(otherpos->somesock,buf,15,0);
		}
		
	}
	close(iphonex);
	close(iphonexplus);
	return 0;
}
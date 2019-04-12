#include "myhead.h"
#include "kernel_list.h"
/*
	ubuntu作为服务器，其他所有的开发板连接ubuntu，开发板作为客户端
	ubuntu的代码
	all:
	server:
	ip:
	pic:大头贴数据64*64大小是12342字节
*/
int bmpnum=0;
int linkflag=0;
pthread_mutex_t mytex;
//定义内核链表对应的结构体
struct clientlist
{
	char ip[16];//存放ip
	unsigned short someport;//存端口号
	int somesock; //存新套接字的
	int picnum; //存放自己大头贴的编号(重点)
	struct list_head  mylist;
};
struct clientlist *myhead;

//封装函数初始化内核链表的表头
struct clientlist *list_init()
{
	struct clientlist *head=malloc(sizeof(struct clientlist));
	INIT_LIST_HEAD(&(head->mylist));
	return head;
}

//封装线程的功能函数
void *recvclient(void *arg)
{
	int ret;
	int bmpfd;
	struct clientlist *p=(struct clientlist *)arg;
	struct clientlist *pos;
	char rbuf[2000];
	char allbuf[200];
	char buf1[20];
	char buf2[100];
	char sbuf[4096];
	char bmppath[10]={0};
	bmpnum++;
	p->picnum=bmpnum;//重点
	sprintf(bmppath,"%d.bmp",bmpnum);
	int flag=0;
	bmpfd=open(bmppath,O_CREAT|O_TRUNC|O_RDWR);
	while(1)//循环分段接收图片信息，每次接收2000字节
	{
		bzero(rbuf,2000);
		ret=recv(p->somesock,rbuf,2000,0);//两种情况：文件大小<=2000   文件>2000字节
		//将接收的内容写入新建的bmp文件中
		write(bmpfd,rbuf,ret);
		if(ret<2000)
			break;
	}
	close(bmpfd);
	while(1)
	{
		printf("进入到线程里面了!\n");
		flag=0; //重新置零
		bzero(rbuf,2000);
		bzero(buf1,20);
		bzero(buf2,100);
		bzero(sbuf,4096);
		//接收某个客户端发送过来的信息
		ret=recv(p->somesock,rbuf,2000,0);
		if(ret==0) //说明客户端断开连接了
		{
			//遍历内核链表删除断开的客户端
			list_for_each_entry(pos,&(myhead->mylist),mylist)
			{
				if(pos->somesock==p->somesock)
				{
					list_del(&(pos->mylist));
					//结束线程
					pthread_exit(NULL);
				}
			}
		}
		//判断客户端发送过来是不是getlist
		if(strcmp(rbuf,"getlist")==0)
		{
			//将用户链表的信息发送给客户端,遍历链表将所有的信息发送给客户端
			list_for_each_entry(pos,&(myhead->mylist),mylist)
			{
				sprintf(sbuf+strlen(sbuf),"%s:%hu\n",pos->ip,pos->someport);
				//将除开自己以外的其他客户端的大头贴发送过去
				
			}
			//将所有拼接好的信息发送给客户端
			send(p->somesock,sbuf,strlen(sbuf),0);
			continue;
		}
		//对rbuf内容进行分割，分析  192.168.11.3:hello
		strcpy(buf1,strtok(rbuf,":"));//以冒号作为切割标准
		strcpy(buf2,strtok(NULL,":"));
		//判断切割出来的结果
		if(strncmp(buf1,"all",3)==0)//说明想广播
		{
			//遍历链表发送给除开自己以外的所有人
			list_for_each_entry(pos,&(myhead->mylist),mylist)
			{
				if(pos->somesock!=p->somesock)//排除自己
					send(pos->somesock,buf2,strlen(buf2),0);
			}
		}
		else if(strncmp(buf1,"server",6)==0)//说明是发送给服务器的
		{
			printf("ip是:%s 端口号是:%hu的客户端发送信息过来了:%s\n",p->ip,p->someport,buf2);
		}
		else
		{
			//遍历链表找到对应ip地址的客户端
			list_for_each_entry(pos,&(myhead->mylist),mylist)
			{
				//本来应该是用ip作为标准去寻找，但是我是在同一台主机上运行，ip都一样，打算用端口号
				if(pos->someport==atoi(buf1))  
				{
					flag=1;
					//字符串的拼接，将发送方的ip+端口号+真实信息一起发送
					bzero(allbuf,200);
					sprintf(allbuf,"%s:%hu:%s",p->ip,p->someport,buf2);
					send(pos->somesock,allbuf,strlen(allbuf),0);
				}
			}
			if(flag==0)
				send(p->somesock,"对不起，没有你想要的客户端!",100,0);//将找不到客户端的提示发送给客户端
		}
	}
}

//线程专门用来发送大头贴
void *sendpic(void *arg)
{
	int j;
	//int i=0;
	//int fd;
	char curbmp[10];
	//char buf[12342];
	//int overplus;
	struct clientlist *pos;
	while(1)
	{
		if(linkflag==1)
		{
			list_for_each_entry(pos,&(myhead->mylist),mylist)
			{
				for(j=1; j<=bmpnum; j++)//bmpnum记录了目前ubuntu上大头贴的个数
				{
					bzero(curbmp,10);
					if(j!=pos->picnum) //排除自己的大头贴
					{
						sprintf(curbmp,"%d.bmp",j);
						printf("curbmp is:%s  sock is:%d\n",curbmp,pos->somesock);
						send(pos->somesock,curbmp,strlen(curbmp),0);   
					}
				}
			}
			linkflag=0;
		}
	}
	
	
/* 	while(1)
	{
		pthread_mutex_lock(&mytex);
		if(linkflag==1)
		{
			for(j=1; j<=bmpnum; j++)
			{
				bzero(curbmp,10);
				overplus=12342;
				i=0;
				if(j!=p->picnum)
				{
					sprintf(curbmp,"%d.bmp",j);
					fd=open(curbmp,O_RDWR);
					if(fd==-1)
					{
						perror("打开出错!\n");
						return -1;
					}
					read(fd,buf,12342);
					close(fd);
					while(1) 
					{
						if(overplus>=2000)//i从0开始
							send(p->somesock,buf+2000*i,2000,0); //两种情况：文件<=2000字节   文件>2000字节
						else
						{
							send(p->somesock,buf+2000*i,overplus,0);
							break;
						}
						i++;	
						overplus=overplus-2000;
					}
				}
			}
		}
		linkflag=0;
	} */
}
int main(int argc,char **argv)
{
	int iphonex;
	pthread_t id;
	//定义新的变量存放新的套接字
	int newsock;
	int ret;
	//定义一个ipv4地址结构体变量并初始化
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;
	//由于存在字节序的问题，需要转换
	bindaddr.sin_port=htons(10000);//程序员自己指定,服务器的端口号
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY);//绑定服务器自己的ip
	
	struct sockaddr_in boyaddr;
	int addrsize=sizeof(boyaddr);
	
	//初始化锁
	pthread_mutex_init(&mytex,NULL);
	//初始化内核链表的头结点
	myhead=list_init();
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
	//监听
	ret=listen(iphonex,6);
	if(ret==-1)
	{
		perror("监听失败!\n");
		return -1;
	}
	printf("阻塞在accept!\n");
	//创建一个线程专门用来发送大头贴
	pthread_create(&id,NULL,sendpic,NULL);
	while(1)
	{
		//接受多个客户端的连接请求
		newsock=accept(iphonex,(struct sockaddr *)&boyaddr,&addrsize);  
		if(newsock==-1)
		{
			perror("接受连接请求失败！\n");
			return -1;
		}
		printf("连接成功的客户端ip是:%s  端口号是:%hu  对应的套接字是:%d\n",inet_ntoa(boyaddr.sin_addr),ntohs(boyaddr.sin_port),newsock);
		//将boyaddr里面存放的ip和端口号，newsock一起打包放入链表中
		//准备新的节点
		struct clientlist *newnode=malloc(sizeof(struct clientlist));
		strcpy(newnode->ip,inet_ntoa(boyaddr.sin_addr));
		newnode->someport=ntohs(boyaddr.sin_port);
		newnode->somesock=newsock;
		INIT_LIST_HEAD(&(newnode->mylist));
		//尾插
		list_add_tail(&(newnode->mylist),&(myhead->mylist));
		
		//创建一个线程专门用来接收这个客户端的发送的信息
		pthread_create(&id,NULL,recvclient,newnode);
		sleep(1);
		linkflag=1;
	}
	//挂机
	close(iphonex);
	return 0;
}
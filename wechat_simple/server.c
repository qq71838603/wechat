#include "myhead.h"

int num;//用户图像编号

 typedef struct node
{
	char ip_address[20];//存储客户端ip
	unsigned short someport;//存端口号
	int new_fd;//套接字文件描述符
	int number;//用户图像编号
	
	struct node * next;
	
	
}node,*pnode;

pnode init_list()//初始化链表
{
	pnode pHead=malloc(sizeof(node));//头结点
	if(pHead==NULL)
	{
		printf("内存分配失败！\n");
		exit(-1);
	}
	pHead->next=NULL;
	
	return pHead;
}


void add(pnode phead,char * ip,unsigned short someport,int fd,int number)//添加结点
{
	pnode p=phead;
	pnode newnode=malloc(sizeof(node));
	if(newnode==NULL)
	{
		printf("内存分配失败！\n");
		exit(-1);
	}
	strcpy(newnode->ip_address,ip);
	newnode->someport=someport;
	newnode->new_fd=fd;
	newnode->number=number;
	newnode->next=NULL;
	while(p->next!=NULL)
	{
		p=p->next;
	}
	p->next=newnode;
}

void delete(pnode head,char * ip)//删除节点
{
	while(head->next!=NULL)
	{
		pnode q=head;
		head=head->next;
		if(strcmp(head->ip_address,ip)==0)
		{
			q->next=head->next;
			free(head);
			break;
		}
	}
}

pnode seek(pnode head,char * ip)//查找节点函数
{
	pnode p=head->next;
	while(p!=NULL)
	{
		if(strcmp(p->ip_address,ip)==0)
		{
			return p;
		}
		p=p->next;
	}
	
	return NULL;
}

void * routine(void * arg)
{
	int ret;
	int count=0;//除开自己在内的用户总数
	char buffer[100];
	char buf[100];
	char buf1[100];
	char buf2[100];
	char * str,* str1,* str2;
	pnode p=(pnode)arg;
	pnode q;
	
	//遍历到链表的尾部,找到储存该连接客户端数据的节点
	while(p->next!=NULL)
	{
		p=p->next;
		count++;
	}
	count--;
	
	//接收用户图片
	bzero(buf,sizeof(buf));
	sprintf(buf,"/home/gec/picture/%d.bmp",num);
	receive_file(buf,p->new_fd);//先接收文件大小，然后按照文件大小一口气接收MSG_WAITWALL
	
	while(1)
	{
		bzero(buffer,100);
		ret=recv(p->new_fd,buffer,100,0);
		printf("Client message:%s\n",buffer);
		if(ret==0)//如果客户端断开连接
		{
			//给所有的在线用户发送断开连接的用户的IP
			q=((pnode)arg)->next;
			while(q!=NULL)
			{
				if(strcmp(q->ip_address,p->ip_address)!=0)
				{
					bzero(buf,sizeof(buf));
					sprintf(buf,"off line:%s",p->ip_address);
					send(q->new_fd,buf,sizeof(buf),0);
				}
				q=q->next;
			}
			//删除断开连接的客户端的数据
			delete((pnode)arg,p->ip_address);
			
			pthread_exit(NULL);
		}
		//判断是否获取用户信息(新上线用户获取信息)
		if(strncmp(buffer,"user",4)==0)
		{
			//发送主界面背景图
			ret=send_file("/mnt/hgfs/share/back.bmp",p->new_fd);
			if(ret==-1)
			{
				exit(-1);
			}
			//发送聊天界面图片
			ret=send_file("/mnt/hgfs/share/timg.bmp",p->new_fd);
			if(ret==-1)
			{
				exit(-1);
			}
			//给客服端发送除开自己在内的在线用户总数
			send(p->new_fd,&count,sizeof(count),0);
			//遍历链表给新连接的客户端发送在线用户信息
			q=((pnode)arg)->next;
			while(q!=NULL)
			{
				if(strcmp(q->ip_address,p->ip_address)!=0)//如果不是自己的客户端数据
				{
					//把节点中客户端的IP地址和端口号以及图像编号拼接起来
					bzero(buf,sizeof(buf));
					sprintf(buf,"%s:%hu:%d:%d",q->ip_address,q->someport,q->new_fd,q->number);
					//将拼接起来的消息发送给目标客户端
					send(p->new_fd,buf,sizeof(buf),0);
					//发送其他用户图像
					bzero(buf,sizeof(buf));
					sprintf(buf,"/home/gec/picture/%d.bmp",q->number);
					send_file(buf,p->new_fd);
				}
				q=q->next;
			}
			//再给已上线的用户发送新上线的用户信息
			q=((pnode)arg)->next;
			while(q!=NULL)
			{
				if(strcmp(q->ip_address,p->ip_address)!=0)//如果不是自己的客户端数据
				{
					bzero(buf,sizeof(buf));
					sprintf(buf,"go online:%s:%hu:%d:%d",p->ip_address,p->someport,p->new_fd,p->number);
					send(q->new_fd,buf,sizeof(buf),0);
					
					//发送新上线用户图像
					bzero(buf,sizeof(buf));
					sprintf(buf,"/home/gec/picture/%d.bmp",p->number);
					send_file(buf,q->new_fd);
				}
				q=q->next;
			}
		}
		//判断是否发来文件
		else if(strncmp(buffer,"@file",5)==0)
		{
			strtok(buffer,":");
			str=strtok(NULL,":");//目标用户ip
			//接收文件路径名
			bzero(buf,sizeof(buf));
			recv(p->new_fd,buf,100,0);
			//循环分割
			str1=strtok(buf,"/");
			while((str2=strtok(NULL,"/"))!=NULL)
			{
				str1=str2;
			}
			bzero(buf1,sizeof(buf1));
			sprintf(buf1,"/home/gec/file/%s",str1);
			//接收文件
			receive_file(buf1,p->new_fd);
			//转发给目标客户端
			q=seek((pnode)arg,str);
			//拼接接收文件指令与文件名
			bzero(buf2,sizeof(buf2));
			sprintf(buf2,"@file:%s",str1);
			send(q->new_fd,buf2,sizeof(buf2),0);
			//发送文件
			send_file(buf1,q->new_fd);
			
		}
		//判断是否发来表情
		else if(strncmp(buffer,"emjo",4)==0)
		{
			str=strtok(buffer,":");
			str1=strtok(NULL,":");
			
			if(strcmp(str,"emjo1")==0)//表情1
			{
				q=seek((pnode)arg,str1);
				bzero(buf1,sizeof(buf1));
				sprintf(buf1,"emjo1:%s",str1);
				send(q->new_fd,buf1,sizeof(buf1),0);//发送接收表情指令
				send_file("/mnt/hgfs/share/emjo1.bmp",q->new_fd);
			}
			else if(strcmp(str,"emjo2")==0)//表情2
			{
				q=seek((pnode)arg,str1);
				bzero(buf1,sizeof(buf1));
				sprintf(buf1,"emjo2:%s",str1);
				send(q->new_fd,buf1,sizeof(buf1),0);//发送接收表情指令
				send_file("/mnt/hgfs/share/emjo2.bmp",q->new_fd);
			}
			else//表情3
			{
				q=seek((pnode)arg,str1);
				bzero(buf1,sizeof(buf1));
				sprintf(buf1,"emjo3:%s",str1);
				send(q->new_fd,buf1,sizeof(buf1),0);//发送接收表情指令
				send_file("/mnt/hgfs/share/gif1.bmp",q->new_fd);
				send_file("/mnt/hgfs/share/gif2.bmp",q->new_fd);
			}
		}
		//发来聊天消息
		else
		{
			str=strtok(buffer,":");
			q=seek((pnode)arg,str);
			str1=strtok(NULL,":");
			bzero(buf1,sizeof(buf1));
			sprintf(buf1,"%s:%s",p->ip_address,str1);
			send(q->new_fd,buf1,sizeof(buf1),0);
		}
	}
}

int main()
{
	int fd,ret;
	int newfd;//储存新的套接字文件描述符
	pthread_t id;
	int value=1;
	
	struct sockaddr_in ipv4_address;//定义一个ipv4结构体变量
	bzero(&ipv4_address,sizeof(ipv4_address));
	ipv4_address.sin_family=AF_INET;//存放地址协议
	ipv4_address.sin_port=htons(6666);//端口号
	ipv4_address.sin_addr.s_addr=inet_addr("192.168.11.135");//IP地址，注意要转换字节序
	
	struct sockaddr_in client_address;//定义一个客户端结构体变量
	int address_size=sizeof(client_address);
	
	//创建套接字
	fd=socket(AF_INET,SOCK_STREAM,0);
	if(fd==-1)
	{
		perror("创建套接字失败！\n");
		return -1;
	}
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value));//重复利用端口
	
	//绑定IP和端口号
	ret=bind(fd,(struct sockaddr *)&ipv4_address,sizeof(struct sockaddr_in));
	if(ret==-1)
	{
		perror("绑定失败！\n");
		return -1;
	}
	
	//监听
	 ret=listen(fd,3);
	 if(ret==-1)
	 {
		perror("监听失败！\n");
		return -1;
	 }
	 
	 pnode mylist=init_list();//初始化链表
	 
	 //接收连接请求并储存客户端数据
	 while(1)
	 {
		 newfd=accept(fd,(struct sockaddr *)&client_address,&address_size);
		 if(newfd==-1)
		 {
			perror("接收连接请求失败！\n");
			return -1;
		 }
		 printf("IP:%s port:%d Connection success!\n",inet_ntoa(client_address.sin_addr),\
		 ntohs(client_address.sin_port),newfd);
		 //用户图像编号加1
		 num++;
		 //储存各客户端的信息
		 add(mylist,inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port),newfd,num);
		 //创建线程
		 pthread_create(&id,NULL,routine,mylist);
		 //设置线程为分离属性
		 pthread_detach(id);
	 }
	 
	 
	close(fd);
	
	return 0;
}
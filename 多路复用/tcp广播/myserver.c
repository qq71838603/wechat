#include "myhead.h"

typedef struct clientaddr{
	int newsocket;
	char *ipaddr;
	unsigned short port;
	struct clientaddr *next;
}Node,*PLNode;

PLNode init_head(PLNode head)
{
	head = (PLNode)malloc(sizeof (Node));

	head->next = NULL;

	return head;
}

int list_add_tail(PLNode head,int newsocket,char* ipaddr,unsigned short port)
{
	if(head == NULL)
	{
		printf("list_add_tail head is NULL\n");
		return -1;
	}
	PLNode node = NULL;
	node = (PLNode)malloc(sizeof(Node));
	if(node == NULL)
	{
		printf("list_add_tail node is NULL\n");
		return -1;
	}

	node->newsocket = newsocket;
	node->ipaddr = ipaddr;
	node->port = port;

	node->next =NULL;
	//找到最后一个节点
	PLNode p =NULL;
	for(p = head;p->next != NULL;p=p->next);

	p->next = node;

	return 0;
}


int main(int argc, char const *argv[])
{
	//定义	
	int sock,ret,addrsize ; 
	addrsize = sizeof(struct sockaddr_in);	
	int maxfd;
	char buf[50];

	//定义ipv4结构体变量，用于存放ip和端口号
	struct sockaddr_in bindaddr;
	struct sockaddr_in clientaddr;

	//初始化结构体变量
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = inet_addr("192.168.110.7");\
	bindaddr.sin_port = htons(11111);

	//创建多路复用集合
	fd_set fdset;


	//创建tcp套接字
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		perror("创建tcp套接字失败\n");
		return -1;
	}

	//取消端口绑定限制
	int on = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	//绑定
	ret = bind(sock,(struct sockaddr *)&bindaddr,addrsize);
	if(ret == -1)	
	{
		perror("绑定失败\n");
		return -1;
	}

	//监听
	ret = listen(sock,5);
	if(ret == -1)
	{
		perror("监听失败\n");
		return -1;
	}

	//创建链表头
	PLNode head = init_head(head);

	int newsocket;
	maxfd = sock;
	PLNode p;

	PLNode pp;

 	PLNode pose;

	while(1)
	{


		FD_ZERO(&fdset);
		bzero(buf,50);
		FD_SET(sock,&fdset);

		for(p=head;p!= NULL; p=p->next)
		{
			FD_SET(p->newsocket,&fdset);
		}


		ret = select(maxfd +1,&fdset,NULL,NULL,NULL);
		if(ret == -1)
		{
			perror("监测失败!\n");
			return -1;
		}

		if(FD_ISSET(sock,&fdset))
		{
			newsocket = accept(sock,(struct sockaddr *)&clientaddr,&addrsize);
			if(newsocket == -1)
			{
				perror("接收连接失败!\n");
				return -1;
			}

			list_add_tail(head,newsocket,inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
			printf("连接的客户端的ip为%s 端口号为%d\n", inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
			
			if(maxfd < newsocket)
			{
				maxfd = newsocket;
			}


		}
		for(pp=head;pp != NULL; pp=pp->next)
		{
			printf("%d\n",pp->newsocket);
		}

		for(p=head;p != NULL; p=p->next)
		{

			if(FD_ISSET(p->newsocket,&fdset))
			{


				recv(p->newsocket,buf,50,0);
				printf("接收到的信息为%s\n",buf);
				break;
			}
		}

		for(pose = head;pose !=NULL;pose=pose->next)
		{
			if(pose->newsocket == p->newsocket)
			{
				continue;
			}
			send(pose->newsocket,buf,50,0);
		}

	}

	close(sock);
	close(newsocket);


	return 0;
}
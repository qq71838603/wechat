#include"header.h"
#include"infolist.c"
/**********全局变量**********/
int newsock;
stu *head;
/************************/
/*************************
项目要求:
	以图像方式显示所有开发板
	点击头像进入与某人的聊天界面
	发表情包,发文件,发文字
	
	删除好友
	好友分组
*************************/
int main()
{

	char recvbuf[255];
	char sendbuf[255];
	struct sockaddr_in bindaddr,destaddr;
	int destlen=sizeof(destaddr);
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;
	//由于存在字节序的问题,所以需要转换.
	bindaddr.sin_port=htons(20000);
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	head=init_list();
	//定义存放文件描述符的集合变量
	fd_set myset;
	int socks[10];
	int socknum=0;
	int i=0;
	//创建套接字
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		perror("mksocket error\n");
		return -1;
	}
	//绑定ip地址和端口号
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&bindaddr,sizeof(bindaddr));
	int ret=bind(sockfd,(struct sockaddr *)&bindaddr,sizeof(bindaddr));
	if(ret==-1)
	{
		perror("bind failed!\n");
		return -1;
	}
	ret=listen(sockfd,10);
	if(ret==-1)
	{
		perror("listen failed!\n");
		return -1;
	}
	int maxsock=sockfd;
	stu *p = head;
	
	serverattr=malloc(sizeof(attr));
	//初始化用户名
	bzero(serverattr,sizeof(serverattr));
	strcpy(serverattr->nicname,"server");
	//初始化用户头像
	picresolv("../client/source/head_portrait2.bmp",serverattr->head_portrait);
	head->attribute=serverattr;
	printf("My name:\n");
	printf("%s\n",head->attribute->nicname);
	while(1)
	{
		FD_ZERO(&myset);
		//遍历添加所有的已连接套接字
		add_newsock(&myset,head);
		/* FD_SET(newsock,&myset);//将已连接套接字的文件描述符加入集合 */
		FD_SET(0,&myset);//将键盘的文件描述符添加进入集合
		FD_SET(sockfd,&myset);
		//FD_SET(newsock,&myset);
		//需要存储所有文件描述符,并且比较得出最大值
		ret=select(maxsock+1,&myset,NULL,NULL,NULL);
		if(FD_ISSET(sockfd,&myset))
		{
			newsock=accept(sockfd,(struct sockaddr *)&destaddr,&destlen);
			if(maxsock<newsock)
			{
				maxsock=newsock;
			}
			tail_insert(newsock,head);
			quiery(head);
			printf("The max: %d this:\n",find_max(head));
		}
		//判断是否真的发生了读就绪
		if(FD_ISSET(0,&myset))//键盘在集合中,说明键盘发生了读就绪
		{
			//获取键盘输入的东西,存放进入一个数组中.
			scanf("%s",sendbuf);
			broadcast(sendbuf,head);
		}
		
		issetsock(&myset,head);
		//封装函数来遍历检测所有已连接套接字
	}
	close(sockfd);
	return 0;
}
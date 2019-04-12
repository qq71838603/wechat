#include "header.h"
#include "namelist1.c"
#include "listlist.c"
#include "piclist.c"
#include "thread_pool.c"
#include "screenoperation.c"
#include "showpic.c"
#include "keyboard.c"
#include "gui.c"
#include "lcd.c"
/**************************
要求能显示宽度不是4的倍数的图片
删除好友功能
分组好友功能
**************************/
/**************************
对获取到的坐标数据进行一定的处理,使误差缩小.
/*******全局变量***********/
thread_pool *pool;
listlist *listhead;
/*************************/
//每次发送的数据都会先存入到标准输出中,之后
void *msgsend()
{
	int count=0;
	while(1)
	{
		while(press==0){
			show_bmp("./source/keyboardicon.bmp",700,440,0);
			show_bmp("./source/exitbutton.bmp",770,0,0);
		}
/* 		if(xpos>=770 && xpos <=800 && ypos>=0 && ypos<35)
		{
			ifexit=1;
			printf("%d\n",ifexit);
		} */
		if(xpos>=690 && xpos <=790 && ypos>=420 && ypos<480)
		{
			system_state=KEYBOARD;
			while(system_state==KEYBOARD);
		}
	}
}

void *refresh_desktop()
{
	show_part_image(0,0,800,480,piclist_find_byname("mainpage",piclist_head));
	updateuserlist(sockfd,all_user_list);
	showuserlist(sockfd,all_user_list);
	ifrefresh==0;
}

int main(int argc,char **argv)
{
	if(argc<4)
	{
		printf("usage: %s (port) (dest ipv4addr:port) [username]\n",argv[0]);
		return -1;
	}
	all_user_list=init_list();//初始化
	listhead=listlist_init();
	screen_init();					//整体初始化
	initpic();
	all_user_list=init_list();
	//printf("here!\n");
	listhead_insert(all_user_list,"all_user_list",listhead);
	pool=malloc(sizeof(thread_pool));
	init_pool(pool,10);
	Init_Lcd();
	scrbuffer=malloc(800*480*4);
	ifshowacrossbar=0;
	add_task(pool,mainpage,NULL);
	add_task(pool,getxy,NULL);
	add_task(pool,across,NULL);
	add_task(pool,msgsend,NULL);
	//add_task(pool,refresh_desktop,NULL);
	/* add_task(pool,spy,NULL); */
	char sendbuf[255];
	char recvbuf[255];
	char showbuf[255];
	char flagbuf[20];
	char filename[255];
	long int size;
	int ret;
	int length;
	int i,count;
	attr attribute;
	
	//初始化用户名
	bzero(&attribute,sizeof(attribute));
	strcpy(attribute.nicname,argv[3]);
	printf("%s",attribute.nicname);
	//初始化用户头像
	picresolv("./source/head_portrait1.bmp",attribute.head_portrait);
	//定义存放文件描述符的集合变量
	fd_set myset;
	FD_ZERO(&myset);//清空集合
	struct sockaddr_in bindaddr,destaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bzero(&destaddr,sizeof(destaddr));
	bindaddr.sin_family=AF_INET;
	//由于存在字节序的问题,所以需要转换.
	bindaddr.sin_port=htons(atoi(argv[1]));
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	destaddr.sin_family=AF_INET;
	
	destaddr.sin_addr.s_addr=inet_addr(strtok(argv[2],":"));
	destaddr.sin_port=htons(atoi(strtok(NULL,":")));
	
	
	//创建套接字
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	
	FD_SET(sockfd,&myset);//将标准输出的文件描述符添加进入集合
	FD_SET(0,&myset);//将标准输出的文件描述符添加进入集合
	//取消绑定限制
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&bindaddr,sizeof(bindaddr));
	//绑定ip地址和端口号
	ret=bind(sockfd,(struct sockaddr *)&bindaddr,sizeof(sockfd));
	
	//连接目标
	ret=connect(sockfd,(struct sockaddr *)&destaddr,sizeof(destaddr));
	if(ret==-1)
	{
		perror("connect failed!\n");
		showtext_cleanall("Connection refused!");
		return -1;
	}
	send(sockfd,&attribute,sizeof(attr),MSG_WAITALL);
	//调用select监测标准输出
	
	show_part_image(0,0,800,480,piclist_find_byname("mainpage",piclist_head));
	updateuserlist(sockfd,all_user_list);
	showuserlist(sockfd,all_user_list);
	sleep(1);
	
	while(1)
	{
		FD_ZERO(&myset);
		FD_SET(0,&myset);
		FD_SET(sockfd,&myset);
		if(ifrefresh==1)
		{
			refresh_desktop();
			printf("refresh success!\n");
			showtext_cleanall("refresh success!\n");
			ifrefresh=0;
		}
		ret=select(sockfd+1,&myset,NULL,NULL,NULL);
		if(ret==-1)
		{
			perror("调用select监测失败!\n");
			return -1;
		}
		//判断是否真的发生了写就绪
		if(FD_ISSET(sockfd,&myset))//标准输出在集合中,说明标准输出发生了写就绪
		{
			ret=recv(sockfd,recvbuf,255,0);
			if(ret==0)
			{
				printf("Connection refused!\n");
				showtext_cleanall("Connection refused!");
				//showtext("Connection refused!");
				return -1;
			}
			else if(strncmp(recvbuf,"#",1)==0)//若接收到的字符串有#前缀,则表示它为系统指令
			{
				if(strncmp(recvbuf+1,"sendfile",strlen("sendfile"))==0)
				{
					if(strstr(recvbuf+1,":")!=NULL){
						strtok(recvbuf+1,":");
						strcpy(filename,strtok(NULL,"^"));		//获取文件名
						size=atoi(strtok(NULL,"^"));			//获取文件大小
						printf("This file %s size:%ld\n",filename,size);
						recvfile(sockfd,size,filename);
						continue;
						/* sendfile(sockfd,flagbuf); */
					}
				}
			}
			else if((length=strlen(recvbuf))<254)//接收消息,并且在储存到全局消息缓存中
			{
				bzero(sys_msgbuf,255);
				strcpy(sys_msgbuf,recvbuf);
			}
			//printf("%s\n",recvbuf);
			bzero(recvbuf,255);
		}
		
/* 		if(FD_ISSET(0,&myset))
		{
			//获取键盘输入的东西,存放进入一个数组中.
			scanf("%s",sendbuf);
			if(strcmp(sendbuf,"#getuserlist")==0)
			{
				continue;
			}
			ret=send(sockfd,sendbuf,strlen(sendbuf),0);
			if(ret==0)
			{
				printf("Connection refused!\n");
				return -1;
			}
		} */
	}
							//回收线程等待退出
	destroy_pool(pool);
	close(sockfd);
	UnInit_Lcd();
	free(scrbuffer);
} 
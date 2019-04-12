#include"header.h"
typedef struct arguments
{
	int sockfd;
	void *listhead;
}arguments;
/***********函数声明*********/
int showuserlist(int sockfd,stu *head);
int updateuserlist(int sockfd,stu *head);
void *across();
void *mainpage(void *args);
int showtext_cleanall(char *text);
/****************************/

int showtext_cleanall(char *text)
{
	Clean_Area(0,0,800,480); //默认是白色背景
	Display_characterXX(100,140,strlen(text)+1,text,2,0x23333333);
}

int showuserlist(int sockfd,stu *head)
{
	stu *p=head;
	char printbuf[275];
	short count=0;
	Display_characterXX(100,100,strlen("用户列表")+1,"用户列表",3,0x00ffffff);
	while(1)
	{
		bzero(printbuf,255);
		count++;
		p=p->next;
		if(p==head)
		{
			/* printf("return!\n"); */
			return 0;
		}
		/* printf("userlist: %s\n",p->attribute->nicname); */
		sprintf(printbuf,"用户%d: %s",count,p->attribute->nicname);
		Display_characterXX(164,120+count*70,strlen(printbuf)+1,printbuf,2,0x00ffffff);
		show_head_portrait(100,100+count*70,p->attribute->head_portrait);
	}
}

int updateuserlist(int sockfd,stu *head)
{
	/* waitevent=true; */
	/* reset_list(); */
	head=init_list();
	all_user_list=head;
	send(sockfd,"#getuserlist",strlen("#getuserlist")+1,0);
	int ret;
	attr *attribute;
	int count=0;
	stu *p=head;
	char nicnamebuf[20];
	
	while(1)
	{
		bzero(nicnamebuf,20);
		count++;
		attribute=malloc(sizeof(attr));
		bzero(attribute,sizeof(attr));
		
		ret=recv(sockfd,attribute,sizeof(attr),MSG_WAITALL);
		if(ret==-1)
		{
			printf("connection refused!\n");
			exit(0);
		}
		strncpy(nicnamebuf,(char *)attribute,20);
		if(strncmp(nicnamebuf,"#done",5)==0)
		{
			/* printf("exit!\n"); */
			free(attribute);
			return count;
		}
		printf("%s\n",nicnamebuf);
		if(strlen((char *)attribute->nicname)>=4){
			tail_insert(attribute,p);
			//printf("recv user: %s\n",attribute->nicname);
			usleep(100);
		}
		else{
			usleep(100);
			printf("failed!\n");
			return -1;
		}
		
		
	}
	return 0;
}

void *across()
{
	int i,j,xbuf,ybuf;
	char bufxy[20];
	int count;
	while(1)
	{
		if(ifshowacrossbar==0)
			pthread_exit("across exit!\n");
		xbuf=xpos;ybuf=ypos;
		/* drawcircle(xbuf,ybuf,40,0x00ffffff); */
		writescr(0,ybuf-1,800,ybuf+1,0x00ff0000);
		writescr(xbuf-1,0,xbuf+1,480,0x00ff0000);
/* 		sprintf(bufxy,"x:%d y:%d",xbuf,ybuf);
		clearscr(700,0,800,16);
		Display_characterXX(700,0,strlen(bufxy)+1,bufxy,1,0x00ffffff); */
		usleep(1000);
		while(press==0);
		if(xbuf>1 && xbuf <798 && ybuf>1 && ybuf<478)
		{
			show_part_image(0,ybuf-1,800,ybuf+1,currentimg);
			show_part_image(xbuf-1,0,xbuf+1,480,currentimg);
/* 			scr_restore(0,ybuf-1,800,ybuf+1);
			scr_restore(xbuf-1,0,xbuf+1,480); */
		}
	}
}

void *spy()
{
	int i,j,xbuf,ybuf;
	char bufxy[20];
	int count;
	while(1)
	{
		if(ifshowacrossbar==0)
			pthread_exit("across exit!\n");
		if(press==0){
			xbuf=0;ybuf=0;
		}
		else{
			xbuf=xpos;ybuf=ypos;
		}
		drawcircle(xbuf,ybuf,20,0x00ff);
		while(press!=0);
		while(press==0);
		drawcircle(xbuf,ybuf,20,0);
	}
}

void initpic()
{
	int* mainpage=malloc(800*480*4);
	picresolv("./bmps/bg5.bmp",mainpage);
	piclist_head=init_piclist("mainpage",mainpage);
	
	currentimg=piclist_head->picbuf;
}

int sendfile(char *nicname)
{
	struct stat fileinfo;
	int ret,bufsize,rdnum;
	char filepath[255];
	char filebuf[1024*1024];
	char msgbuf[255];
	bzero(filepath,255);					//清空数组
	bzero(msgbuf,255);
	bzero(filebuf,1024*1024);
	showtext("请输入要发送的文件路径!");
	strcpy(filepath,getinput());
	
	
	ret=stat(filepath,&fileinfo);			//获取文件的大小信息
	if(ret==-1)
	{
		perror("Sent failed\n");
		showtext_cleanall("Something wrong with this file!");
		return -1;
	}
	
	if(fileinfo.st_size>1024*1024)			//根据文件的大小来合理的分配每次发送的缓存大小
		bufsize=1024*1024;
	else
		bufsize=fileinfo.st_size;
	
	//printf("file size: %ld\n",fileinfo.st_size);
	int fd=open(filepath,O_RDONLY);			//以只读方式打开文件
	if(fd==-1){
		perror("Sent failed\n");
		showtext_cleanall("Something wrong with this file!");
		return -1;
	}

	if(strcmp(nicname,"server")!=0){		//针对目标进行发送,服务器需要特殊处理
		sprintf(msgbuf,"#sendfile:%s@%s^%ld",filepath,nicname,fileinfo.st_size);
		send(sockfd,msgbuf,strlen(msgbuf)+1,0);
	}
	else
		send(sockfd,msgbuf,strlen(msgbuf)+1,0);
	//printf("%s\n",msgbuf);

	do{
		bzero(filebuf,1024*1024);
		rdnum=read(fd,filebuf,bufsize);//将缓存的文件发送给服务器,服务器根据之前的信息选择转发
		if(rdnum==0){
			printf("exit!\n");
			return 0;
		}
		ret=send(sockfd,filebuf,rdnum,0);
		if(ret==0)
		{
			printf("Connection refused!\n");
			showtext_cleanall("Connection refused!");
			//showtext("Connection refused!");
			exit(0);
		}
	}while(1);
	usleep(100);
}

int recvfile(int sockfd,long int size,char *filename)			//接收文件
{
	//printf("%ld\n",size);
	char filepathbuf[255];
	char filebuf[1024*1024];
	/* printf("here!\n"); */
	mkdir("./download/",0777);
	sprintf(filepathbuf,"./download/%s",filename);
	//printf("%s\n",filepathbuf);
	int fd=open(filepathbuf,O_CREAT|O_RDWR);
	if(fd==-1){
		perror("open failed!\n");
		return 0;
	}
	chmod(filepathbuf,0777);
	int ret=0;
	int already=0;
	int wrbuf=0;
	do{
		bzero(filebuf,1024*1024);
		ret=recv(sockfd,filebuf,1024*1024,0);
		if(ret==0)
		{
			printf("connection refused!\n");
			return -1;
		}
		else if(ret==-1)
		{
			printf("recv failed!\n");
		}
		wrbuf=write(fd,filebuf,ret);
		if(wrbuf==-1)
		{
			perror("write failed\n");
			return 0;
		}
		//printf("%s  %ld\n",filebuf,ret);
		already=already+wrbuf;
		//printf("already: %d\n",already);
	}while(already<size);
	//printf("done!\n");
	close(fd);
	return 0;
}

int chat_win(attr *usrbuf)
{
	char *msgbuf=malloc(255);
	int count=0;
	int xbuf,ybuf;
	while(1)
	{
		start:show_part_image(0,0,800,480,currentimg);
		show_head_portrait(290,20,usrbuf->head_portrait);
		
		
		Display_characterXX(364,35,strlen(usrbuf->nicname)+1,usrbuf->nicname,2,0x00ffffff);
		Display_characterXX(700,190,strlen("图片")+1,"图片",2,0x00ffffff);
		Display_characterXX(700,250,strlen("文件")+1,"文件",2,0x00ffffff);
		while(press==0 && strlen(sys_msgbuf)<=0);
		
		if(strlen(sys_msgbuf)>0){
			Display_characterXX(100,200,strlen(sys_msgbuf)+1,sys_msgbuf,2,0x00ffffff);
			printf("%s\n",sys_msgbuf);
			while(press==0);
		}
		bzero(sys_msgbuf,255);
		usleep(100);
		xbuf=xpos;ybuf=ypos;
		if(system_state==KEYBOARD)
		{
			strcpy(msgbuf,getinput());
			system_state=0;
			if(strlen(msgbuf)==0)
				goto start;
			else if(strcmp(usrbuf->nicname,"server")!=0){
				strcat(msgbuf,"@");
				strcat(msgbuf,usrbuf->nicname);
			}
			printf("send buf %s\n",msgbuf);
			send(sockfd,msgbuf,strlen(msgbuf)+1,0);
			currentimg=piclist_head->picbuf;
		}
		else if(xbuf>=770 && xbuf <=800 && ybuf>=0 && ybuf<35)
		{
			break;
		}
		else if(xbuf>=700 && xbuf <=800 && ybuf>=190 && ybuf<230)
		{
			printf("图片\n");
			Display_characterXX(700,190,strlen("图片")+1,"图片",2,0x00ff0000);
			while(press!=0);
		}
		else if(xbuf>=700 && xbuf <=800 && ybuf>=240 && ybuf<290)
		{
			Display_characterXX(700,250,strlen("文件")+1,"文件",2,0x00ff0000);
			system_state=KEYBOARD;
			sendfile(usrbuf->nicname);
			while(press!=0);
		}
	}
}

int refresh()
{
	currentimg=piclist_head->picbuf;
	show_part_image(0,0,800,480,currentimg);
	showuserlist(sockfd,all_user_list);
	Display_characterXX(700,130,strlen("刷新用")+1,"刷新用",2,0x00ffffff);
	Display_characterXX(700,160,strlen("户列表")+1,"户列表",2,0x00ffffff);
	Display_characterXX(730,220,strlen("添加")+1,"添加",2,0x00ffffff);
	Display_characterXX(700,250,strlen("用户组")+1,"用户组",2,0x00ffffff);
}

void *mainpage(void *args)
{
	char *msgbuf=malloc(255);
	attr *usrbuf;
	int xbuf,ybuf;
	while(1)
	{
		refresh();
		while(press==0);
		usleep(100);
		xbuf=xpos;ybuf=ypos;
		if(system_state==KEYBOARD){
			strcpy(msgbuf,getinput());
			send(sockfd,msgbuf,strlen(msgbuf)+1,0);
			system_state=0;
		}
		else if(xbuf>700 && xbuf<800 && ybuf >110 && ybuf <200)
		{
			printf("刷新用户列表\n");
			ifrefresh=1;
			send(sockfd," @xiaoan",strlen(" @xiaoan")+1,0);
			//updateuserlist(sockfd,all_user_list);
			//showuserlist(sockfd,all_user_list);
			while(press!=0);
		}
		else if(xbuf>700 && xbuf<800 && ybuf >215 && ybuf <290)
		{
			printf("添加用户组\n");
			while(press!=0);
		}
		else if(xbuf>90 && xbuf<400)
		{
			usrbuf=get_user_bycount((ybuf-100)/70,NULL);
			if(usrbuf!=NULL)
			{
				chat_win(usrbuf);
				while(press!=0);
			}
		}
		while(press!=0);
	}
	free(msgbuf);
}
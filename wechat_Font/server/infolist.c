/*
test a double-linked circular list
*/
#include<stdio.h>
#include<stdlib.h>
struct infolist
{
	int sockfd;
	attr *attribute;
	struct infolist *next;
	struct infolist *preview;
};
#define stu  struct infolist

int list_del(stu *delnode,stu* mylist);
int quiery(stu *mylist);

stu *init_list()
{
	stu *head=malloc(sizeof(stu));
	head->attribute=NULL;
	head->next=head;
	head->preview=head;
	head->attribute=malloc(sizeof(attr));
	return head;
}

int *picresolv(char *picname,int *picbuf)
{
	int i,x,y,k;
	//定义一个用来存放图片头文件信息的数组
	char bmp_head[54] = {0};
	//定义两个变量，用来保存从头信息中获取到的图片的宽和高
	unsigned short w=0,h=0;
	//------------1-----------------
	//打开BMP图片
	int bmp_fd = open(picname,O_RDWR);
	if(bmp_fd == -1)
	{
		return (int *)-1;
	}	
	//2读取BMP的数据
	//读取头信息，获取图片的宽和高
	read(bmp_fd,bmp_head,54);
	w = bmp_head[19] << 8 | bmp_head[18];
	h = bmp_head[23] << 8 | bmp_head[22];
	//根据得到的宽和高来设计合理的数组
	char bmp_buf[w*h*3];
	int *touch_screen_buf;
	if(picbuf==NULL)
		touch_screen_buf=malloc(w*h);
	else
		touch_screen_buf=picbuf;
	//读取BMP的像素数据
	int ret = read(bmp_fd,bmp_buf,w*h*3);
	if(ret == -1)
	{
		//_printf("read bmp fail\n");
		return (int *)-1;
	}
	//对齐像素
	for(i=0;i<w*h;i++)
		touch_screen_buf[(h-(i/w))*w+i%w] = bmp_buf[3*i+0] | bmp_buf[3*i+1]<<8 | bmp_buf[3*i+2]<<16 | 0x00<<24; 

	return touch_screen_buf;
} 

stu *tail_insert(int num,stu *mylist)
{
	stu *p = mylist;
	stu *tail = mylist->preview;
	stu *newnode = malloc(sizeof(stu));
	newnode->sockfd=num;
	newnode->attribute=NULL;
	newnode->preview=tail;
	newnode->next=p;
	p->preview=newnode;
	tail->next=newnode;
	return newnode;
}

int add_newsock(fd_set *myset,stu *mylist)
{
	stu *p = mylist;
	if(p->next==mylist)
	{
		return -1;
	}
	do{
		p=p->next;
		FD_SET(p->sockfd,myset);
	}while(p->next!=mylist);
}

int broadcast(char *string,stu *mylist)
{
	stu *p = mylist;
	int ret;
	if(p->next==mylist)
	{
		return -1;
	}
	do{
		p=p->next;
		ret=send(p->sockfd,string,strlen(string)+1,0);
		if(ret==0)
		{
			printf("Connection refused!\n");
			list_del(p,mylist);
			continue;
		}
	}while(p->next!=mylist);
}

int list_del(stu *delnode,stu* mylist)
{
	stu *p=mylist;
	delnode->next->preview=delnode->preview;
	delnode->preview->next=delnode->next;
	delnode->next=NULL;
	delnode->preview=NULL;
/* 	if(delnode->attribute!=NULL)
		free(delnode->attribute); */
	free(delnode);
}


int implement(char *msg,char *addr,int mode,int sockfd,stu *mylist)
{
	int ret;
	int ifexist=0;
	stu *p=mylist;
	/* char strbuf[255]; */
	if(mode==255)//广播(除自身之外可见)
	{
		while(p->next!=mylist)
		{
			p=p->next;
			if(strcmp(addr,p->attribute->nicname)==0)
				continue;
			/* printf("%d",pos->port); */
			ret=send(p->sockfd,msg,strlen(msg)+1,0);
			if(ret==-1){
				perror("error\n");
				return -1;
			}
			else if(ret==0){
				perror("refused!\n");
				return -1;
			}
		}
	}
	else
	{	
		while(p->next!=mylist)
		{
			p=p->next;
			if(strcmp(addr,p->attribute->nicname)==0)
			{
				ret=send(p->sockfd,msg,strlen(msg)+1,0);
				ifexist=1;
			}
			switch(ret)
			{
				case -1:
					perror("sent failed!\n");
					continue;
				case 0:
					printf("this addr is not in used!\n");
					break;
				default:break;
			}
		}
		if(ifexist==0)
		{
			send(sockfd,"This user you specified is not online!",strlen("This user you specified is not online!"),0);
		}
	}
	return 0;
}


int senduserlist(int sockfd,stu *mylist)
{
	int i=0;
	stu *p = mylist;
	int ret;
	if(p->next==mylist)
	{
		return -1;
	}
	int count=0;
	
	do{
		send(sockfd,p->attribute,sizeof(attr),MSG_WAITALL);
		printf("send: %s\n",p->attribute->nicname);
		p=p->next;
	}while(p!=mylist);
	attr sattr;
	strcpy(sattr.nicname,"#done");
	ret=send(sockfd,&sattr,sizeof(attr),0);
	if(ret==-1)
	{
		perror("infolist.c senduserlist failed!\n");
	}
	return 0;
}

int sendfile(char *sendername,char *nicname,char *filepath,stu *mylist)
{
	struct stat fileinfo;
	int ret,bufsize,rdnum;
	char filebuf[1024*1024];		//清空数组
	char filepathbuf[255];
	stu *p=mylist;
	char msgbuf[255];
	
	bzero(filepathbuf,255);
	bzero(filebuf,1024*1024);
	bzero(msgbuf,1024*1024);
	
	printf("Send file!\n");
	
	while(p->next!=mylist)
	{
		p=p->next;
		if(strcmp(nicname,p->attribute->nicname)==0)		//查找到这个用户所属的节点,方便之后发送文件
			break;
	}
	
	sprintf(filepathbuf,"./download/%s",filepath);
	ret=stat(filepathbuf,&fileinfo);			//获取文件的大小信息
	if(ret==-1)
	{
		perror("Sent failed\n");
		return -1;
	}
	sprintf(msgbuf,"#sendfile:%s^%ld",filepath,fileinfo.st_size);
	
	send(p->sockfd,msgbuf,strlen(msgbuf)+1,0);
	printf("msgbuf: %s\n",msgbuf);
	
	if(fileinfo.st_size>1024*1024)			//根据文件的大小来合理的分配每次发送的缓存大小
		bufsize=1024*1024;
	else
		bufsize=fileinfo.st_size;
	
	printf("file size: %ld\n",fileinfo.st_size);
	int fd=open(filepathbuf,O_RDONLY);			//以只读方式打开文件
	if(fd==-1){
		perror("Sent failed\n");
		return -1;
	}
	do{
		bzero(filebuf,1024*1024);
		rdnum=read(fd,filebuf,bufsize);//将缓存的文件发送给服务器,服务器根据之前的信息选择转发
		if(rdnum==0){
			printf("exit!\n");
			return 0;
		}
		ret=send(p->sockfd,filebuf,rdnum,0);
		if(ret==0)
		{
			printf("Connection refused!\n");
			return 0;
		}
	}while(1);
	usleep(100);
}

int recvfile(int sockfd,long int size,char *filename)			//接收文件
{
	printf("%ld\n",size);
	char filepathbuf[255];
	char filebuf[1024*1024];
	printf("here!\n");
	mkdir("./download/",0777);
	sprintf(filepathbuf,"./download/%s",filename);
	printf("%s\n",filepathbuf);
	int fd=open(filepathbuf,O_CREAT|O_RDWR);
	if(fd==-1){
		perror("open failed!\n");
		return 0;
	}
	chmod(filepathbuf,0777);
	printf("here!\n");
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

int issetsock(fd_set *myset,stu *mylist)
{
	char recvbuf[255];
	char msgbuf[255];
	char flagbuf[20];
	char stringbuf[255];
	char filename[255];
	long int size;
	stu *p = mylist;
	int ret;
	while(p->next!=mylist)
	{
		bzero(recvbuf,255);
		bzero(msgbuf,255);
		bzero(flagbuf,255);
		bzero(stringbuf,255);
		bzero(filename,255);
		
		p=p->next;
		if(p->sockfd==0)
			continue;
		else if(FD_ISSET(p->sockfd,myset))
		{
			
			if(p->attribute==NULL){
				goto setattr;
			}
			else{
				ret=recv(p->sockfd,recvbuf,255,0);
			}
			if(ret==0)
			{
				printf("connection refused!\n");
				list_del(p,mylist);
				return 0;
			}
			//如果该用户没有用户名和头像,那么他发送过来的这一条信息将会作为attr结构体接收,验证后存入用户信息库中(包括用户头像)
			setattr:if(p->attribute==NULL)
			{
				attr *userinfo=malloc(sizeof(attr));
				bzero(userinfo,sizeof(attr));
				ret=recv(p->sockfd,userinfo,sizeof(attr),MSG_WAITALL);
				p->attribute=userinfo;
				printf("nicname %s\n",userinfo->nicname);
			}
			else if(strncmp(recvbuf,"#",1)==0)//若接收到的字符串有#前缀,则表示它为系统指令
			{
				if(strcmp(recvbuf+1,"getuserlist")==0)
				{
					senduserlist(p->sockfd,mylist);
				}
				else if(strncmp(recvbuf+1,"sendfile",strlen("sendfile"))==0)
				{
					if(strstr(recvbuf+1,":")!=NULL && strstr(recvbuf+1,"@")!=NULL){
						strtok(recvbuf+1,":");
						strcpy(filename,strtok(NULL,"@"));		//获取文件名
						strcpy(flagbuf,strtok(NULL,"^"));		//获取发送目标用户
						size=atoi(strtok(NULL,"^"));
						printf("This file is send to%s  %s size:%ld\n",flagbuf,filename,size);
						recvfile(p->sockfd,size,filename);
						
						sendfile(p->attribute->nicname,flagbuf,filename,mylist);
						return 0;
					}
				}
			}
			else if(strstr(recvbuf,"@")==NULL)//没有检测到@符号,表示这条消息会发送给服务器,并在服务器上显示出来
			{
				printf("%s: %s\n",p->attribute->nicname,recvbuf);
				return 0;
			}
			else
			{
				strcpy(msgbuf,strtok(recvbuf,"@"));
				strcpy(flagbuf,strtok(NULL,"@"));
				strcpy(stringbuf,p->attribute->nicname);
				if(strcmp(flagbuf,"all")==0)//检测到@all后缀,表示广播信息
				{
					implement(msgbuf,stringbuf,255,0,mylist);
					return 0;
				}
				else
				{
					bzero(recvbuf,255);
					sprintf(recvbuf,"%s:%s",stringbuf,msgbuf);
					printf("stringbuf %s\n",recvbuf);
					implement(recvbuf,flagbuf,0,p->sockfd,mylist);//向指定昵称用户发送信息
					switch(ret)
					{
						case -1:
							perror("sent failed!\n");
							return 0;
						case 0:
							printf("connetion refused!\n");
							return 0;
						default:return 0;;
					}
				}
			}
			bzero(recvbuf,255);
		}
	}
}

int find_max(stu *mylist)
{
	stu *p1=mylist;
	stu *p2=mylist;
	int max=0;
	if(mylist->next==mylist)
	{
		return -1;
	}
	while(p1->next!=mylist)
	{
		p1=p1->next;
		while(p2->next!=mylist)
		{
			p2=p2->next;
			if(p1->sockfd > p2->sockfd)
			{
				max=p1->sockfd;
			}
			else if(p2->sockfd > p1->sockfd)
			{
				max=p2->sockfd;
			}
			else
			{
				max=p1->sockfd;
			}
		}
	}
	return max;
}

int quiery(stu *mylist)
{
	stu *p = mylist;
	while(p->next!=mylist){
		p=p->next;
		printf("%d",p->sockfd);
		if(p->attribute!=NULL)
			printf("%s",p->attribute->nicname);
	}
	
}

int backquiery(stu *mylist)
{
	stu *p = mylist;
	do{
		p=p->preview;
		printf("%d\n",p->sockfd);
	}while(p->preview!=mylist);
	
}
/* void main(int argc,char **argv)
{
	infohead=init_list();
	dirsol(argv[1]);
	quiery(infohead);
	//backquiery(mylist);
	free(infohead);
}
*/
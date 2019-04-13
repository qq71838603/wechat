#include "myhead.h"

int send_file(char * path,int newfd)//发文件
{
	int bmpfd,ret;
	//打开文件
	 bmpfd=open(path,O_RDWR);
	 if(bmpfd==-1)
	 {
		 perror("File opening failed!\n");
		 return -1;
	 }
	 
	 int size=lseek(bmpfd,0,SEEK_END);//求文件的大小
	 printf(" file size:%d\n",size);
	 //依照文件实际大小定义数组
	 char buf[size];
	 lseek(bmpfd,0,SEEK_SET);//将文件指针移到文件开头
	 //将文件大小发送给客户端
	 ret=send(newfd,&size,sizeof(int),0);
	 bzero(buf,sizeof(buf));
	 read(bmpfd,buf,size);
	 //将文件发送给客户端
	 ret=send(newfd,buf,size,0);
	 printf("Size of the sending fil:%d\n",ret);
	
	return 0;
}

int receive_file(char * bmppath,int fd)//接收文件
{
	int size,ret,bmpfd,ret1;
	//接收文件大小信息
	ret=recv(fd,&size,sizeof(int),0);
	if(ret==-1)
	{
		perror("Failed to receive file siz!\n");
		return -1;
	}
	//依照文件实际大小定义数组
	 char buf[size];
	//接收文件
	bzero(buf,sizeof(buf));
	ret=recv(fd,buf,size,MSG_WAITALL);
	if(ret==-1)
	{
		perror("Failed to receive file!\n");
		return -1;
	}
	printf("Receive File size:%d\n",ret);
	//创建一个文件
	bmpfd=open(bmppath,O_CREAT|O_RDWR|O_TRUNC,0777);
	if(bmpfd==-1)
	{
		perror("File creation failed!\n");
		return -1;
	}
	ret1=write(bmpfd,buf,ret);
	if(ret1==-1)
	{
		perror("Write file failed!\n");
	}
	
	close(bmpfd);
	
	return 0;
}
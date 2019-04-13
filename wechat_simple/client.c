#include "myhead.h"

int fd;
int flag=1;//标志用户是否进入了聊天界面
int flag2=2;//标志选择功能位

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

//显示所有在线用户图片
void show_user_picture(pnode head)
{
	int x=0,y=0;
	char buf[50];
	
	//先显示主界面背景图
	show_fullbmp("/IOT/mypicture/back.bmp");
	
	pnode p=head->next;
	while(p!=NULL)
	{
		bzero(buf,sizeof(buf));
		sprintf(buf,"/IOT/mypicture/%d.bmp",p->number);
		show_shapebmp(x,y,100,100,buf);
		x+=100;
		if(x==800)
		{
			y+=100;
			x=0;
		}
		
		p=p->next;
	}
}

void * routine(void * arg)
{
	
	
	int ret,temp,i;
	int count=0;
	char buffer[100];
	char buf[100];
	char * str1,* str2,* str3,* str4;
	pnode head=(pnode)arg;
	
	
	//循环接收服务器消息
	while(1)
	{
		bzero(buffer,100);
		ret=recv(fd,buffer,100,0);
		if(ret==0)//如果服务器断开连接
		{
			exit(-1);
		}
		//判断发来的是否为以下线用户的IP
		if(strncmp(buffer,"off line",8)==0)
		{
			strtok(buffer,":");
			str1=strtok(NULL,":");
			//删除该用户数据
			delete(head,str1);
			if(flag)
				//刷新用户图像
				show_user_picture(head);
		}
		//判断发来的是否为新上线用户的信息
		else if(strncmp(buffer,"go online",9)==0)
		{
			//对消息进行分割
			strtok(buffer,":");
			str1=strtok(NULL,":");
			str2=strtok(NULL,":");
			str3=strtok(NULL,":");
			str4=strtok(NULL,":");
			printf("新上线用户的图像编号为:%d\n",atoi(str4));
			//把信息存进链表
			add(head,str1,atoi(str2),atoi(str3),atoi(str4));
			//接收新上线用户图像
			bzero(buf,sizeof(buf));
			sprintf(buf,"/IOT/mypicture/%d.bmp",atoi(str4));//IOT/mypicture/%d.bmp
			receive_file(buf,fd);
			if(flag)
				//刷新用户图像
				show_user_picture(head);
		}
		//判断是否发来文件
		else if(strncmp(buffer,"@file",5)==0)
		{
			//分割得到文件名
			strtok(buffer,":");
			str1=strtok(NULL,":");
			bzero(buf,sizeof(buf));
			sprintf(buf,"/IOT/%s",str1);
			//接收文件
			receive_file(buf,fd);
			printf("接收文件成功!\n");
		}
		//发来表情
		else if(strncmp(buffer,"emjo",4)==0)
		{
			str1=strtok(buffer,":");
			str2=strtok(NULL,":");
			
			if(strcmp(str1,"emjo1")==0)//发来的是表情1
			{
				receive_file("/IOT/mypicture/emjo1.bmp",fd);
				//显示表情,只在聊天界面显示
				if(flag==0)
				{
					//刷新屏幕
					show_fullbmp("/IOT/mypicture/timg.bmp");
					show_shapebmp(350,100,100,100,"/IOT/mypicture/emjo1.bmp");
				}
			}
			if(strcmp(str1,"emjo2")==0)//发来的是表情2
			{
				receive_file("/IOT/mypicture/emjo2.bmp",fd);
				//显示表情
				if(flag==0)
				{
					//刷新屏幕
					show_fullbmp("/IOT/mypicture/timg.bmp");
					show_shapebmp(350,100,100,100,"/IOT/mypicture/emjo2.bmp");
				}
			}
			if(strcmp(str1,"emjo3")==0)//发来的是表情3
			{
				receive_file("/IOT/mypicture/gif1.bmp",fd);
				receive_file("/IOT/mypicture/gif2.bmp",fd);
				//显示gif图像
				if(flag==0)
				{
					//刷新屏幕
					show_fullbmp("/IOT/mypicture/timg.bmp");
					for(i=0;i<50;i++)
					{
						show_shapebmp(350,100,100,100,"/IOT/mypicture/gif1.bmp");
						usleep(20000);
						show_shapebmp(350,100,100,100,"/IOT/mypicture/gif2.bmp");
					}
				}
			}
			
		}
		//发来聊天信息
		else
		{
			printf("%s\n",buffer);
			
			str1=strtok(buffer,":");
			str2=strtok(NULL,":");
			
			//遍历链表找到发消息用户图像的位置
			pnode p=head->next;
			count=0;
			while(p!=NULL)
			{
				count++;
				if(p->someport==atoi(str1))//改成ip
					break;
				p=p->next;
			}
			//在聊天界面时
			if(flag==0)
			{
				bzero(buf,sizeof(buf));
				sprintf(buf,"第%d个图像用户给您发送了信息:",count);
				//刷新一下屏幕，清除历史字体
				show_fullbmp("/IOT/mypicture/timg.bmp");
				//初始化lcd
				Init_Lcd();
				//显示提示信息
				Display_characterX(160,34,30,buf,2);
				//显示聊天信息
				Display_characterX(160,115,50,str2,2);
				UnInit_Lcd();
			}
			//在主界面时
			else
			{
				bzero(buf,sizeof(buf));
				sprintf(buf,"第%d个图像用户给您发送了信息:",count);
				//刷新一下屏幕，清除历史字体
				show_user_picture(head);
				//初始化lcd
				Init_Lcd();
				Display_characterX(160,323,30,buf,2);
				Display_characterX(160,370,50,str2,2);
				UnInit_Lcd();
			}
		}
	}
}

void function(pnode p)//选择功能函数
{
	int ret;
	char buf[100];
	char buf1[100];
	
	if(flag2==1)//聊天
	{
		printf("聊天对象的IP:%s\n",p->ip_address);
		printf("请输入聊天信息:\n");
		bzero(buf,sizeof(buf));
		scanf("%s",buf);
		sprintf(buf1,"%s:%s",p->ip_address,buf);
		send(fd,buf1,sizeof(buf1),0);
	}
	if(flag2==2)//发文件
	{
		//拼接发送文件指令以及目标ip
		bzero(buf,sizeof(buf));
		sprintf(buf,"@file:%s",p->ip_address);
		send(fd,buf,sizeof(buf),0);

		printf("请输入您要发送文件的绝对路径名:\n");
		while(1)
		{
			bzero(buf1,sizeof(buf1));
			scanf("%s",buf1);
			//判断文件是否存在
			ret=open(buf1,O_RDONLY);
			if(ret==-1)
			{
				 perror("文件路径名有误,请重新输入！\n");
			}
			else
				break;
		}
		//发送文件绝对路径名给服务器
		send(fd,buf1,sizeof(buf1),0);
		//发送文件给服务器
		send_file(buf1,fd);//在开发版上改成ip
		printf("发送文件成功!\n");
	}
	if(flag2==3)//发送表情1
	{
		bzero(buf,sizeof(buf));
		sprintf(buf,"emjo1:%s",p->ip_address);
		send(fd,buf,sizeof(buf),0);
	}
	if(flag2==4)//发送表情2
	{
		bzero(buf,sizeof(buf));
		sprintf(buf,"emjo2:%s",p->ip_address);
		send(fd,buf,sizeof(buf),0);
	}
	if(flag2==5)//发送表情3
	{
		bzero(buf,sizeof(buf));
		sprintf(buf,"emjo3:%s",p->ip_address);
		send(fd,buf,sizeof(buf),0);
	}
}

void *rout(void * arg)
{
	int x,y,a,b;
	int flag1=0;
	pnode p;
	
	while(1)
	{
		a=0;
		b=0;
		p=((pnode)arg)->next;
		
		get_xy(&x,&y);//获取坐标
		flag=0;//令标志位为0，暂时停止刷新用户图像并在屏幕上显示聊天信息
		printf("x=%d,y=%d\n",x,y);
		//判断用户点击谁的头像
		while(p!=NULL)
		{
			if((x>=a&&x<=a+100)&&(y>=b&&y<=b+100))
			{
				//显示聊天界面
				show_fullbmp("/IOT/mypicture/timg.bmp");
				while(1)
				{
					get_xy(&x,&y);//获取坐标,判断用户选择了哪个功能
					if((x>=0&&x<=105)&&(y>=0&&y<=100))//返回
					{
						//刷新用户图像
						show_user_picture((pnode)arg);
						flag1=1;
						flag=1;
						break;
					}
					else if((x>=160&&x<=240)&&(y>=300&&y<=365))//聊天
					{
						//调用选择功能函数
						flag2=1;
						function(p);
					}
					else if((x>=590&&x<=650)&&(y>=304&&y<=370))//发文件
					{
						flag2=2;
						function(p);
					}
					else if((x>=160&&x<=240)&&(y>=400&&y<=480))//发表情1
					{
						flag2=3;
						function(p);
					}
					else if((x>=358&&x<=440)&&(y>=404&&y<=480))//发表情2
					{
						flag2=4;
						function(p);
					}
					else if((x>=594&&x<=666)&&(y>=405&&y<=480))//发表情3
					{
						flag2=5;
						function(p);
					}
				}
				if(flag1)
				{
					flag1=0;
					break;
				}
			}
			p=p->next;
			
			
			a+=100;
			if(a==800)
			{
				a=0;
				b+=100;
			}
		}
	}
}

//从服务器获取信息
void user_info(pnode head)
{
	int i;
	int num=1,count=0;
	int ret;
	char buf[100];
	char buf1[100];
	char * str1,* str2,* str3,* str4;
	
	bzero(buf,sizeof(buf));
	strcpy(buf,"user");
	send(fd,buf,strlen(buf),0);//给服务器发送"user"消息，请求服务器发送用户信息过来
	//接收主界面背景图
	ret=receive_file("/IOT/mypicture/back.bmp",fd);
	if(ret==-1)
	{
		exit(-1);
	}
	//接收聊天界面图片
	ret=receive_file("/IOT/mypicture/timg.bmp",fd);
	if(ret==-1)
	{
		exit(-1);
	}
	//接收服务器发来的在线用户数量
	ret=recv(fd,&count,sizeof(count),0);
	if(ret==-1)
	{
		perror("接收在线用户数量失败!\n");
		exit(-1);
	}
	
	for(i=1;i<=count;i++)
	{
		//接收其他用户信息
		bzero(buf,sizeof(buf));
		ret=recv(fd,buf,sizeof(buf),0);
		if(ret==-1)
		{
			perror("接收用户信息失败!\n");
			exit(-1);
		}
		//对消息进行分割
		str1=strtok(buf,":");
		str2=strtok(NULL,":");
		str3=strtok(NULL,":");
		str4=strtok(NULL,":");
		//存进链表
		add(head,str1,atoi(str2),atoi(str3),atoi(str4));
		//接收其他用户图像
		bzero(buf1,sizeof(buf1));
		sprintf(buf1,"/IOT/mypicture/%d.bmp",atoi(str4));
		receive_file(buf1,fd);
	}
}

int main( int argc,char *argv[])
{
	int ret;
	int value=1;
	char buffer[100];
	char buf[100];
	pthread_t id;
	pthread_t id1;
	
	struct sockaddr_in ipv4_address;//定义一个ipv4结构体变量
	bzero(&ipv4_address,sizeof(ipv4_address));
	ipv4_address.sin_family=AF_INET;//存放地址协议
	ipv4_address.sin_port=htons(atoi(argv[1]));//端口号
	ipv4_address.sin_addr.s_addr=inet_addr("INADDR_ANY");//IP地址，注意要转换字节序,宏INADDR_ANY(表示使用本地主机中任意一个ip地址)
	
	struct sockaddr_in server_address;//定义一个服务器结构体变量
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family=AF_INET;//存放地址协议
	server_address.sin_port=htons(6666);//服务器的端口号
	//服务器的IP地址，注意要转换字节序
	server_address.sin_addr.s_addr=inet_addr("192.168.11.135");
	
	
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
	printf("My port number is:%d\n",ntohs(ipv4_address.sin_port));
	//连接服务器
	ret=connect(fd,(struct sockaddr *)&server_address,sizeof(server_address));
	if(ret==-1)
	{
		perror("连接服务器失败!\n");
		return -1;
	}
   	//在开发板上创建一个目录
	ret=mkdir("/IOT/mypicture",0777);
	if(ret==-1)
	{
		if(errno!=17)
		{
			perror("创建目录失败!\n");
			return -1;
		}
	} 
	//给服务器发送用户图像
	bzero(buffer,sizeof(buffer));
	printf("请输入您上传的图像的绝对路径名,图像规格为:100*100\n");
	while(1)
	{
		scanf("%s",buffer);
		ret=send_file(buffer,fd);//发送图像
		if(ret==0)
			break;
		printf("图片不存在,请您重新输入要上传的图像的绝对路径名：\n");
	}
	
	//获取用户信息
	 pnode mylist=init_list();//初始化链表
	 user_info(mylist);
	 
	//显示所有在线用户图片
	show_user_picture(mylist);
	
	//创建线程,用来接收信息
	pthread_create(&id,NULL,routine,mylist);
	pthread_detach(id);//设置线程为分离属性
	
 	//创建一个线程用来监测屏幕
	pthread_create(&id1,NULL,rout,mylist);
	pthread_detach(id1);//设置线程为分离属性 
	
	while(1);
	
	close(fd);
	
	return 0;
}
#include "myhead.h"

void *jpeg(void *arg)
{
	//把自己分离出去
	//pthread_detach(pthread_self());
	int i= 0;
	//屏幕闪烁
	while(i<6)
	{
		show_anyjpg(266,300,"/wechat/1.jpg");
		usleep(100000);
		show_anyjpg(266,300,"/wechat/2.jpg");
		usleep(100000);
		i++;
	}
	pthread_exit(NULL);
}

int menu()
{
	int ret;

	int addrsize = sizeof(struct sockaddr_in);
	char buf[MAXSIZE];
	char *sound;
	struct input_event eventbuf;
	//访问触摸屏设备文件
	int eventfb = open("/dev/input/event0",O_RDWR);
	if(eventfb == -1)
	{
		printf("open event0 error!\n");
		return -1;
	}
	//显示主界面jpeg图片
	show_jpeg("/wechat/weixinmenu.jpg");

	//定义ipv4地址结构体变量,用于存放绑定的ip和端口号
	struct sockaddr_in bindaddr;
	struct sockaddr_in otheraddr;

	//初始化结构体变量
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = inet_addr("192.168.110.8");
	bindaddr.sin_port = htons(12345);


	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		printf("socket error!\n");
		return -1;
	}
	//设置端口重复使用
	int on = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	//绑定
	ret = bind(sock,(struct sockaddr*)&bindaddr,addrsize);
	if(ret == -1)
	{
		printf("bind error!\n");
		return -1;
	}
	//监听
	ret = listen(sock,1);
	if(ret == -1)
	{
		printf("listen error!\n");
		return -1;
	}
	//接收连接请求
	int newsock = accept(sock,(struct sockaddr*)&otheraddr,&addrsize);
	if(newsock == -1)
	{
		printf("accept error !\n");
		return -1;
	}

	//定义集合并清空
	fd_set fdset;
	FD_ZERO(&fdset);

	int maxfd = eventfb;
	if(maxfd < newsock)
		maxfd = newsock;

	int x,y;
	//定义线程
	pthread_t tid;
	//pthread_attr_t attr;

	while(1)
	{
		bzero(buf,MAXSIZE);
		//添加监测文件描述符
		FD_SET(0,&fdset);
		FD_SET(eventfb,&fdset);
		FD_SET(newsock,&fdset);




		//监测
		ret = select(maxfd+1,&fdset,NULL,NULL,NULL);
		if(ret == -1)
		{
			printf("select error !\n");
			return -1;
		}
		//如果收到了消息  则用线程实现屏幕闪烁
		if(FD_ISSET(newsock,&fdset))
		{
			//创建线程
			//pthread_attr_init(&attr);
			//pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

			ret = pthread_create(&tid,NULL,jpeg,NULL);
			if(ret == -1)
			{
				printf("pthread_create error\n");
				return -1;
			}
		}

		//阻塞读取触摸屏数据
		read(eventfb,&eventbuf,sizeof(eventbuf));
		//获取触摸到的x,y值
		if(eventbuf.type == EV_ABS && eventbuf.code == ABS_X)
		{
			x = eventbuf.value;
		}
		if(eventbuf.type == EV_ABS && eventbuf.code == ABS_Y)
		{
			y = eventbuf.value;
		}

		//获取压力值
		if(eventbuf.type == EV_KEY && eventbuf.code ==BTN_TOUCH && eventbuf.value == 0)
		{
			//发送语音
			if(x<266 && y>300)
			{
				bzero(buf,MAXSIZE);
				//监测触摸屏  如果在区域内则发送语音
				if(FD_ISSET(eventfb,&fdset))
				{
					system("arecord -d4 -c1 -r16000 -twav -fS16_LE sound.wav");
				}
				//定义文件的名字以及文件描述
				char soundbuf[SOUNDSIZE] = {0};
				FILE *stream;
				strcpy(soundbuf,"/wechat/sound.wav");
				//打开文件
				if((stream = fopen(soundbuf,"r")) == NULL)
				{
					printf("open file error!\n");
					exit(-1);
				}
				int len = 0;
				//不断的从读到的文件中发送数据出去
				while((len = fread(buf,1,1024,stream))>0)
				{
					if(send(newsock,buf,len,0) < 0)
					{
						break;
					}
					bzero(buf,MAXSIZE);
				}		
			}
			
			//接收语音
			if(FD_ISSET(eventfb,&fdset))
			{
				if(x>266 && x<532 && y>300)
				{
					bzero(buf,MAXSIZE);
					//为存放文件名的指针赋予空间
					sound= (char*)malloc(sizeof(MAXSIZE));
					//接收发送过来的语音
					recv(newsock,buf,MAXSIZE,0);
					strcpy(sound,"recvsound.wav");

					//创建recvsound.wav名字的文件并打开
					int fd = open(sound,O_CREAT | O_TRUNC | O_RDWR); 
					if(fd == -1)
					{
						printf("open error\n");
						return -1;
					}
					//把从buf中读到的数据写入fd
					write(fd,buf,MAXSIZE);
					//执行播放.wav功能
					system("aplay recvsound.wav");
				}	
			}

			//发送文件
			if(x>532 && y>300)
			{
				//定义数组存放发送的文件名
				
				char name_file[SOUNDSIZE] = {0};
				AA:
				printf("plz input file path\n");
				//输入文件的路径
				scanf("%s",name_file);

				bzero(buf,MAXSIZE);

				FILE *fp;
				//打开文件
				if((fp = fopen(name_file,"r")) == NULL)
				{
					printf("open file error! plz input again\n");
					goto AA;
				}
				int size;

				//不断的写入buf
				while((size = fread(buf,1,1111,fp))>0)
				{
					//通过buf不断的发送数据
					if(send(newsock,buf,size,0) < 0)
					{
						break;
					}
					bzero(buf,MAXSIZE);
				}	
				
			}
			//打开摄像头并拍一张照片
			if(FD_ISSET(eventfb,&fdset))
			{
				if(x>600&&y>100&&y<300)
				{
					camera();
				}				
			}

			
			//退出
			if(FD_ISSET(eventfb,&fdset))
			{
				if(x>532 && y<100)
				{
					return 0;
				}			
			}
		}	
	}
}





int main(int argc, char const *argv[])
{
	menu();	

	return 0;
}
#include"header.h"

/************函数声明**************/
int screen_init();
void *getxy();
int clearscr(int x1,int y1,int x2,int y2);
int writescr(int x1,int y1,int x2,int y2,int color);
int scr_store(int x1,int y1,int x2,int y2);
int scr_restore(int x1,int y1,int x2,int y2);
int coloradd(int x1,int y1,int x2,int y2,int value);
int colorminus(int x1,int y1,int x2,int y2,int value);
int colorset(int x1,int y1,int x2,int y2,int value);
int scr_store(int x1,int y1,int x2,int y2);
int scr_restore(int x1,int y1,int x2,int y2);
/********************************/
int screen_init()
{
	lcd_fd = open("/dev/fb0",O_RDWR);	
	if(lcd_fd == -1)
	{
		//_printf("open touch_screen fail\n");
		return -1;
	}
	ts_fd = open("/dev/input/event0",O_RDWR);
	if(ts_fd == -1)
	{
		//_printf("open touch_screen fail\n");
		return -1;
	}
	//2 映射显存，对显存进行操作就相当于对屏幕进行操作
	fbp = (int *)mmap(0,800*480*4,PROT_WRITE | PROT_READ,MAP_SHARED,lcd_fd,0);
	
}
int getmed(int *num)
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=3;j>=2;j--)
		{
			if(num[i]>num[j])
			{
				/* printf("%d num[i]:%d num[j]:%d   ",i+1,num[i],num[j]); */
				num[i]=num[i]+num[j];
				num[j]=num[i]-num[j];
				num[i]=num[i]-num[j];
				/* printf("%d num[i]:%d num[j]:%d\n",i+1,num[i],num[j]); */
			}
		}
	return num[2];
}
void *getxy()
{
	int x,y;
	char addrbuf[10];
	int count=0;
	int xbuf[4];
	int ybuf[4];
	int pressbuf;
	while(1)
	{
		x = -1;y = -1;
		while(1)
		{
			if(shutdownenable==true)
				break;
			read(ts_fd,&tchevent_buf,sizeof(tchevent_buf));
			if(tchevent_buf.type == EV_ABS)
			{
				if(tchevent_buf.code == ABS_X)
				{
					x = tchevent_buf.value;
				}
				else if(tchevent_buf.code == ABS_Y)
				{
					y = tchevent_buf.value;
				}		
				else if(tchevent_buf.code == ABS_PRESSURE)
				{
					press=tchevent_buf.value;
					//printf("PRESSURE = %d\n",tchevent_buf.value);
				}
			}
			if(x>=0 && y>=0)
			{
				xbuf[count]=x;
				ybuf[count]=y;
				count++;
				if(count==4)
				{
					/* printf("%d\n",count); */
					xpos=getmed(xbuf);
					ypos=getmed(ybuf);
					/* press=pressbuf; */
					//printf("%d %d\n",xpos,ypos);
					/* usleep(100); */
/* 					xpos=x;
					ypos=y; */
					sprintf(addrbuf,"x:%d y:%d",xpos,ypos);
					clearscr(700,0,800,16);
					Display_characterXX(700,4,strlen(addrbuf)+1,addrbuf,1,0x00ffffff);
					count=0;
					break;
				}
			}
		}
	}
	pthread_exit("screen monitor exit!\n");
}

int clearscr(int x1,int y1,int x2,int y2)
{
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
		*(fbp+i*800+j)=0x00000000;
		}
	}
	return 0;
}
int writescr(int x1,int y1,int x2,int y2,int color)
{	
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
			if(j>=0&&j<800 && i>=0&&i<480)
			*(fbp+i*800+j)=color;
		}
	}
}
int drawcircle(int x1,int y1,int width,int color)
{	
	int i,j,k;
	for(i=y1-width/2;i<y1+width/2;i++)
	{
		for(j=x1-width/2;j<x1+width/2;j++)
		{
			if((y1-i)*(y1-i)+(x1-j)*(x1-j)<=(width/2)*(width/2))
				if(i<480&&j<800)
				*(fbp+i*800+j)=color;
		}
	}
}
int clearcircle(int x1,int y1,int width)
{	
	int i,j,k;
	for(i=y1-width/2;i<y1+width/2;i++)
	{
		for(j=x1-width/2;j<x1+width/2;j++)
		{
			if((y1-i)*(y1-i)+(x1-j)*(x1-j)<=(width/2)*(width/2))
				if(i<480&&j<800)
				*(fbp+i*800+j)=0;
		}
	}
}
int coloradd(int x1,int y1,int x2,int y2,int value)
{
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
			*(fbp+800*i+j)+=value;
		}
	}
}
int colorset(int x1,int y1,int x2,int y2,int value)
{
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
			*(fbp+800*i+j)=value;
		}
	}
}
int colorminus(int x1,int y1,int x2,int y2,int value)
{
	int i,j;
	for(i=y1;i<y2;i++)
	{
		for(j=x1;j<x2;j++)
		{
			*(fbp+800*i+j)-=value;
		}
	}
}

int scr_store(int x1,int y1,int x2,int y2)
{
	bzero(scrbuffer,800*480*4);
	int i,j;
	for(i=y1;i<y2;i++)
		for(j=x1;j<x2;j++)
			*(scrbuffer+i*800+j)=*(fbp+i*800+j);
	return 0;
}
int scr_restore(int x1,int y1,int x2,int y2)
{
	int i,j;
	for(i=y1;i<y2;i++)
		for(j=x1;j<x2;j++)
				*(fbp+i*800+j)=*(scrbuffer+i*800+j);
	return 0;
}
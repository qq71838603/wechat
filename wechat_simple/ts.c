#include "myhead.h"

//获取x，y坐标的函数
void get_xy(int *x,int *y)
{
	struct input_event tsevent;
	int count=0;//标志位
	int fd;
	
	//触摸屏的初始化
	fd=open("/dev/input/event0",O_RDONLY);
	if(fd==-1)
	{
		perror("失败!\n");
		return;
	}
	
	while(1)
	{
		//读取坐标
		read(fd,&tsevent,sizeof(struct input_event));//无法读取到坐标，read会一直阻塞
		//判断事件类型是不是触摸屏事件
		if(tsevent.type==EV_ABS) //说明是触摸屏事件
		{
			//判断x，y坐标
			if(tsevent.code==ABS_X) //x坐标
			{
				*x=tsevent.value;
				count++;
			}
				
			if(tsevent.code==ABS_Y) //y坐标
			{
				*y=tsevent.value;	
				count++;
			}
			if(count==2)
				break;//读取到坐标后退出循环
		}
	}
	
	close(fd);//关闭触摸屏
}

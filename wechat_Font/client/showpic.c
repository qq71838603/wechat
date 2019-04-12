#include"header.h"
int *show_bmp(char * bmppathname,int x1,int y1,int z)
{
	int i,x,y,k;
	//定义一个用来存放图片头文件信息的数组
	char bmp_head[54] = {0};
	//定义两个变量，用来保存从头信息中获取到的图片的宽和高
	unsigned short w=0,h=0;
	//------------1-----------------
	//打开BMP图片
	int bmp_fd = open(bmppathname,O_RDWR);
	if(bmp_fd == -1)
	{
		//_printf("open bmp fail\n");
		//_printf("%s\n",strerror(errno));
		return (int *)-1;
	}	
	//2读取BMP的数据
	//读取头信息，获取图片的宽和高
	read(bmp_fd,bmp_head,54);
	w = bmp_head[19] << 8 | bmp_head[18];
	h = bmp_head[23] << 8 | bmp_head[22];
	////_printf("w:%d h:%d\n",w,h);
	//判断要刷的图片是否超过了屏幕范围
	if(w+x1>800 || h+y1>480)
	{
		//_printf("yuejie\n");
		return (int *)-1;
	}
	//根据得到的宽和高来设计合理的数组
	char bmp_buf[w*h*3];
	int touch_screen_buf[w*h];
	//读取BMP的像素数据
	int ret = read(bmp_fd,bmp_buf,w*h*3);
	if(ret == -1)
	{
		//_printf("read bmp fail\n");
		return (int *)-1;
	}	
	//-------------3--------------
	//对齐像素
	for(i=0;i<w*h;i++)
		touch_screen_buf[i] = bmp_buf[3*i+0] | bmp_buf[3*i+1]<<8 | bmp_buf[3*i+2]<<16 | 0x00<<24; 
	//在显存中写入数据，相当于显示在touch_screen屏幕上
	switch(z)
	{
		case 0:
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{		
				*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];//把图片像素填充到显存中去，让它在touch_screen屏幕上显示出来
			}
		}
		break;
		case 1:
		//从上到下的显示效果
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{		
				*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];//把图片像素填充到显存中去，让它在touch_screen屏幕上显示出来
			}
			usleep(1000);//停顿5毫秒，让我们能够观察到图片是如何在touch_screen上面显示出来的
		}
		break;
		case 2:
		//从左往右的显示效果
		for(x=0;x<w;x++)
		{
			for(y=0;y<h;y++)
			{		
				*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];//把图片像素填充到显存中去，让它在touch_screen屏幕上显示出来

			}
			usleep(1000);//停顿5毫秒，让我们能够观察到图片是如何在touch_screen上面显示出来的
		}
		break;
		case 3:
		//从下到上：
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{		
				*(fbp + (h-y-1+y1)*800 + x1 + x) = touch_screen_buf[y*w+x];//把图片像素填充到显存中去，让它在touch_screen屏幕上显示出来

			}
			usleep(1000);//停顿5毫秒，让我们能够观察到图片是如何在touch_screen上面显示出来的
		}
		break;
		case 4:
		//从右到左：
		for(x=0; x<w; x++)
		{
			for(y=0; y<h; y++)
			{
				*(fbp + (y+y1)*800 + x1 + w - x - 1) = touch_screen_buf[(h-1-y)*w+w - x - 1];			
			}							
			usleep(1000);//时间停顿，显示刷的效果				
		}
		break;
		//横百叶窗
		case 5:
		//int k;
		for(x=0;x<(w/8);x++)
		{
			for(y=0;y<h;y++)
			{
				for(k=0;k<8;k++)
				{
					*(fbp+800*(y+y1)+x+x1+k*w/8)= touch_screen_buf[(h-1-y)*w+x+k*w/8];
				}
			}
			usleep(2000);			
		}
	 
		break;
		//竖百叶窗（图片的高需要是8的倍数）
		case 6:
		//int k;
		for(y=0;y<(h/8);y++)
		{
			for(x=0;x<w;x++)
			{
				for(k=0;k<8;k++)
				{
					*(fbp+800*(y+y1+k*h/8)+x+x1)= touch_screen_buf[(h-1-y-k*h/8)*w+x];
				}
			}
			usleep(4000);			
		}
	
		break;
		//圆形收缩
		case 7:
		//int k;
		for(k=h;k>=0;k-=10)
		{
			for(y=0;y<h;y++)
			{
				for(x=0;x<w;x++)
				{
					if((x-400)*(x-400)+(y-240)*(y-240)>=k*k)
					{
						*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];
					}
				}
			}
			usleep(1000);
		}
		
		break;
		//圆形扩散
		case 8:
		//int k;
		for(k=0;k<h;k+=10)
		{
			for(y=0;y<h;y++)
			{
				for(x=0;x<w;x++)
				{
					if((x-400)*(x-400)+(y-240)*(y-240)<=k*k)
					{
						*(fbp+800*(y+y1)+x+x1)= touch_screen_buf[(h-1-y)*w+x];
					}
				}
			}
			usleep(1000);
		}
		break;
		//随机填充,拥有雾化的显示效果.但是有bug,只能用于480*800的图片.
		case 9:
		{
			int m=800*480;
			srand((unsigned )time(NULL));
			int n=800*480;
			int t,w;
			int a[480*800];
			for (i=0;i<n;i++)
			a[i]=i;
			for (i=0;i<m;i++)
			{
				w=rand()%(n-i)+i;
				t=a[i];
				a[i]=a[w];
				a[w]=t;
				}
			for(i=0;i<480*800;i++)
			{
				*(fbp+(479-a[i]/800)*800+a[i]%800)=touch_screen_buf[a[i]];
				if(i%800==0)
				{
				usleep(1);
				}
			}
		break;
		/* randomize(); */
		}

	}
	

	close(bmp_fd);

	return  (int *)0;
	
}

int show_head_portrait(int x1,int y1,int *picbuffer)
{
	int x,y;
	int width=32;
	for(y=0;y<64;y++)
		for(x=0;x<64;x++){
			if((x-32)*(x-32)+(y-32)*(y-32)<=width*width)
				*(fbp+(y+y1)*800+x+x1)=picbuffer[y*64+x];
		}
}

int show_exit_button(int x1,int y1,int width,int *picbuffer)
{
	int x,y;
	for(y=0;y<64;y++)
		for(x=0;x<64;x++){
			if((x-width/2)*(x-width/2)+(y-width/2)*(y-width/2)<=(width/2)*(width/2))
				*(fbp+(y+y1)*800+x+x1)=picbuffer[y*64+x];
		}
}

void show_part_image(int x1,int y1,int x2,int y2,int *image)//width:800
{
	int i,j;
	for(j=y1;j<y2;j++)
		for(i=x1;i<x2;i++)
			if(j<480 && i<800)
			*(fbp+j*800+i)=image[j*800+i];
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
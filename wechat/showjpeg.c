#include "myhead.h"
/*
	先从最简单的800*480大小的jpg入手
*/
void show_jpeg(char *argv)
{
	int i=0;
	int lcdfd;
	//打开lcd
	lcdfd=open("/dev/fb0",O_RDWR);
	struct jpeg_decompress_struct myinfo;//定义用于存放jpeg解压参数的结构体
	struct jpeg_error_mgr myerr;//定义存放错误信息的结构体
	//对结构体初始化
	jpeg_create_decompress(&myinfo);
	//注册错误信息
	myinfo.err=jpeg_std_error(&myerr);
	//获取jpeg数据源
	FILE *myjpeg=fopen(argv,"r+");
	jpeg_stdio_src(&myinfo,myjpeg);
	//读取jpeg的头信息
	jpeg_read_header(&myinfo,true);
	//开始解压缩
	jpeg_start_decompress(&myinfo);
	//定义一个数组用于存放一行RGB数值
	char *buf=malloc(myinfo.output_width*3);
	/*
		&buf --->char **
		char buf[10]  &buf--> char(*p)[10]
	*/
	
	int newbuf[myinfo.output_width];
	while(myinfo.output_scanline < myinfo.output_height)
	{
		jpeg_read_scanlines(&myinfo,(JSAMPARRAY)&buf,1);//每次循环读取一行RGB
		//将读取到的RGB转换之后填充LCD
		for(i=0;i<myinfo.output_width;i++)
		{
			newbuf[i]=buf[3*i+2]|buf[3*i+1]<<8|buf[3*i]<<16|0x00<<24;
		}
		write(lcdfd,newbuf,myinfo.output_width*4);
	}
	//收尾
	jpeg_finish_decompress(&myinfo);
	jpeg_destroy_decompress(&myinfo);
	close(lcdfd);
	fclose(myjpeg);
}


int show_anyjpg(int x,int y,char *jpgpath)
{
	int i=0;
	int lcdfd;
	//打开lcd
	lcdfd=open("/dev/fb0",O_RDWR);
	//映射LCD
	int *lcdmem=mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcdfd,0);
	struct jpeg_decompress_struct myinfo;//定义用于存放jpeg解压参数的结构体
	struct jpeg_error_mgr myerr;//定义存放错误信息的结构体
	//对结构体初始化
	jpeg_create_decompress(&myinfo);
	//注册错误信息
	myinfo.err=jpeg_std_error(&myerr);
	//获取jpeg数据源
	FILE *myjpeg=fopen(jpgpath,"r+");
	jpeg_stdio_src(&myinfo,myjpeg);
	//读取jpeg的头信息
	jpeg_read_header(&myinfo,true);
	//开始解压缩
	jpeg_start_decompress(&myinfo);
	//定义一个数组用于存放一行RGB数值
	char *buf=malloc(myinfo.output_width*3);
	/*
		&buf --->char **
		char buf[10]  &buf--> char(*p)[10]
	*/
	
	int newbuf[myinfo.output_width];
	while(myinfo.output_scanline < myinfo.output_height) //  450*200
	{
		jpeg_read_scanlines(&myinfo,(JSAMPARRAY)&buf,1);//每次循环读取一行RGB
		//将读取到的RGB转换之后填充LCD
		for(i=0;i<myinfo.output_width;i++)
		{
			newbuf[i]=buf[3*i+2]|buf[3*i+1]<<8|buf[3*i]<<16|0x00<<24;
		}
		memcpy(lcdmem+(y+myinfo.output_scanline-1)*800+x,newbuf,myinfo.output_width*4);
		/*第一次循环  首地址  lcdmem+y*800+x
		  第二次循环          lcdmem+(y+1)*800+x    
		*/
	}
	//收尾
	jpeg_finish_decompress(&myinfo);
	jpeg_destroy_decompress(&myinfo);
	close(lcdfd);
	fclose(myjpeg);
	munmap(lcdmem,800*480*4);
	return 0;
}
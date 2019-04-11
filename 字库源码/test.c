#include <stdio.h>
#include <string.h>
/*
	开发板上字符编码设置成默认default
*/
int main()
{
	//初始化lcd
	Init_Lcd();
	//给汉字设置背景颜色
	//int  Clean_Area(int X,int Y,int width,int height)
	Clean_Area(0,0,800,100); //默认是白色背景
	//显示字符串
	//int  Display_characterX(unsigned int x,unsigned int y,unsigned int len,unsigned char *string,int size)
	Display_characterX(0,0,strlen("你好粤嵌"),"你好粤嵌",4); //注意汉字编码必须修改成GB2312
	UnInit_Lcd();
}
#include <stdio.h>
#include <string.h>
/*
	开发板上字符编码设置成默认default
*/
int main()
{
	Init_Lcd();
	Clean_Area(0,0,800,100); //默认是白色背景
	Display_characterX(0,0,strlen("你好粤嵌"),"你好粤嵌",4); //注意汉字编码必须修改成GB2312
	UnInit_Lcd();
}
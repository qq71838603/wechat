#include "header.h"
/********************/
int showtext(char *text);
char *getinput();
/********************/
int showtext(char *text)
{
	Clean_Area(0,0,800,172); //默认是白色背景
	Display_characterXX(100,140,strlen(text)+1,text,2,0x23333333);
}
showcursor(char *text)
{
	Clean_Area(0,0,800,172); //默认是白色背景
	Display_characterXX(100,140,strlen(text)+1,text,2,0x23333333);
}
char *getinput()
{
	char *stringbuf=malloc(255);
	short xbuf,ybuf,lybuf,sybuf;
	bool ifshift=false;
	bzero(stringbuf,255);
	clock_t start, finish;
	double duration;  
	keyboard:while(1)
	{
		bzero(scrbuffer,480*800*4);
		show_bmp("./source/keyboard.bmp",0,170,0);
		scr_store(0,0,800,480);
		usleep(500000);
		while(1)
		{
			while(press==0);
			usleep(100);
			xbuf=xpos;ybuf=ypos;
			if(strlen(stringbuf)<254){
				showtext(stringbuf);
			}
			else if(strlen(stringbuf)==255){
				showtext("字符串过长,继续输入将会清空缓冲区!");
			}
			else{
				showtext("缓冲区已清空");
			}
			if(ybuf>=208 && ybuf<=270)
			{					//第一行的按键
				if(xbuf>=0 && xbuf<=70)
				{							//Q
					colorminus(0,208,70,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"q");
					showtext(stringbuf);
					scr_restore(0,208,70,270);
				}
				else if(xbuf>80 && xbuf<=140)
				{							//W
					colorminus(80,208,140,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"w");
					showtext(stringbuf);
					scr_restore(80,208,140,270);
				}
				else if(xbuf>=150 && xbuf<=210)
				{							//E
					colorminus(150,208,210,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"e");
					showtext(stringbuf);
					scr_restore(150,208,210,270);
				}
				else if(xbuf>=220 && xbuf<=280)
				{							//R
					colorminus(220,208,290,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"r");
					showtext(stringbuf);
					scr_restore(220,208,290,270);
				}
				else if(xbuf>=290 && xbuf<=360)
				{							//T
					colorminus(290,208,360,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"t");
					showtext(stringbuf);
					scr_restore(290,208,360,270);
				}
				else if(xbuf>=366 && xbuf<=433)
				{							//Y
					colorminus(366,208,433,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"y");
					showtext(stringbuf);
					scr_restore(366,208,433,270);
				}
				else if(xbuf>=438 && xbuf<=504)
				{							//U
					colorminus(438,208,504,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"u");
					showtext(stringbuf);
					scr_restore(438,208,504,270);
				}
				else if(xbuf>=514 && xbuf<=576)
				{							//I
					colorminus(514,208,576,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"i");
					showtext(stringbuf);
					scr_restore(514,208,576,270);
				}
				else if(xbuf>=585 && xbuf<=650)
				{							//O
					colorminus(585,208,650,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"o");
					showtext(stringbuf);
					scr_restore(585,208,650,270);
				}
				else if(xbuf>=656 && xbuf<=720)
				{							//P
					colorminus(656,208,720,270,0x23333333);
					while(press!=0);
					strcat(stringbuf,"p");
					showtext(stringbuf);
					scr_restore(656,208,720,270);
				}
				else if(xbuf>=730 && xbuf<=795)
				{							//BACKSPACE
					colorminus(730,208,795,270,0x23333333);
					while(press!=0){
						stringbuf[strlen(stringbuf)-1]=0;
						usleep(100000);
						showtext(stringbuf);
					}
					scr_restore(730,208,795,270);
				}
			}
			else if(ybuf>=276 && ybuf<=339)
			{								//第二行的按键
				sybuf=276;lybuf=343;
				if(xbuf>=30 && xbuf<=96)
				{							//A
					colorminus(30,sybuf,96,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"a");
					showtext(stringbuf);
					scr_restore(30,sybuf,96,lybuf);
				}
				else if(xbuf>106 && xbuf<=170)
				{							//S
					colorminus(106,sybuf,170,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"s");
					showtext(stringbuf);
					scr_restore(106,sybuf,170,lybuf);
				}
				else if(xbuf>=180 && xbuf<=240)
				{							//D
					colorminus(180,sybuf,240,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"d");
					showtext(stringbuf);
					scr_restore(180,sybuf,240,lybuf);
				}
				else if(xbuf>=247 && xbuf<=311)
				{							//F
					colorminus(247,sybuf,311,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"f");
					showtext(stringbuf);
					scr_restore(247,sybuf,311,lybuf);
				}
				else if(xbuf>=321 && xbuf<=382)
				{							//G
					colorminus(321,sybuf,382,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"g");
					showtext(stringbuf);
					scr_restore(321,sybuf,382,lybuf);
				}
				else if(xbuf>=392 && xbuf<=453)
				{							//H
					colorminus(392,sybuf,453,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"h");
					showtext(stringbuf);
					scr_restore(392,sybuf,453,lybuf);
				}
				else if(xbuf>=465 && xbuf<=530)
				{							//J
					colorminus(465,sybuf,530,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"j");
					showtext(stringbuf);
					scr_restore(465,sybuf,530,lybuf);
				}
				else if(xbuf>=536 && xbuf<=600)
				{							//K
					colorminus(536,sybuf,600,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"k");
					showtext(stringbuf);
					scr_restore(536,sybuf,600,lybuf);
				}
				else if(xbuf>=610 && xbuf<=672)
				{							//L
					colorminus(610,sybuf,672,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"l");
					showtext(stringbuf);
					scr_restore(610,sybuf,672,lybuf);
				}
				else if(xbuf>=680 && xbuf<=800)
				{							//搜索
					colorminus(680,sybuf,800,lybuf,0x23333333);
					start=clock();
					while(press!=0);
					finish=clock();
					if(finish-start>500000)
						strcat(stringbuf,"`");
					else
						strcat(stringbuf,"$");
					showtext(stringbuf);
					scr_restore(680,sybuf,800,lybuf);
				}
			}
			
			else if(ybuf>=343 && ybuf<=404)
			{								//第三行的按键
				sybuf=343;lybuf=410;
				if(xbuf>=0 && xbuf<=64)
				{							//shift
					if(ifshift==true){
						colorminus(0,sybuf,64,lybuf,0x23333333);
						while(press!=0);
					}
					else{
						scr_restore(0,sybuf,64,lybuf);
						while(press!=0);
					}
				}
				else if(xbuf>75 && xbuf<=135)
				{							//Z
					colorminus(75,sybuf,135,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"z");
					showtext(stringbuf);
					scr_restore(75,sybuf,135,lybuf);
				}
				else if(xbuf>=147 && xbuf<=207)
				{							//X
					colorminus(147,sybuf,207,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"x");
					showtext(stringbuf);
					scr_restore(147,sybuf,207,lybuf);
				}
				else if(xbuf>=214 && xbuf<=278)
				{							//C
					colorminus(214,sybuf,278,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"c");
					showtext(stringbuf);
					scr_restore(214,sybuf,278,lybuf);
				}
				else if(xbuf>=287 && xbuf<=350)
				{							//V
					colorminus(287,sybuf,350,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"v");
					showtext(stringbuf);
					scr_restore(287,sybuf,350,lybuf);
				}
				else if(xbuf>=359 && xbuf<=418)
				{							//B
					colorminus(359,sybuf,418,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"b");
					showtext(stringbuf);
					scr_restore(359,sybuf,418,lybuf);
				}
				else if(xbuf>=428 && xbuf<=492)
				{							//N
					colorminus(428,sybuf,492,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"n");
					showtext(stringbuf);
					scr_restore(428,sybuf,492,lybuf);
				}
				else if(xbuf>=499 && xbuf<=563)
				{							//M
					colorminus(499,sybuf,563,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"m");
					showtext(stringbuf);
					scr_restore(499,sybuf,563,lybuf);
				}
				else if(xbuf>=568 && xbuf<=630)
				{							//!,
					colorminus(568,sybuf,630,lybuf,0x23333333);
					start=clock();
					while(press!=0);
					finish=clock();
					if(finish-start>500000)
						strcat(stringbuf,"!");
					else
						strcat(stringbuf,",");
					showtext(stringbuf);
					scr_restore(568,sybuf,630,lybuf);
				}
				else if(xbuf>=638 && xbuf<=701)
				{							//.?
					colorminus(638,sybuf,701,lybuf,0x23333333);
					start=clock();
					while(press!=0);
					finish=clock();
					if(finish-start>500000)
						strcat(stringbuf,"?");
					else
						strcat(stringbuf,".");
					showtext(stringbuf);
					scr_restore(638,sybuf,701,lybuf);
				}
				else if(xbuf>=710 && xbuf<=774)
				{							//' *
					colorminus(710,sybuf,774,lybuf,0x23333333);
					start=clock();
					while(press!=0);
					finish=clock();
					if(finish-start>500000)
						strcat(stringbuf,"'");
					else
						strcat(stringbuf,"*");
					showtext(stringbuf);
					scr_restore(710,sybuf,774,lybuf);
				}
			}
			else if(ybuf>=413 && ybuf<=473)
			{								//第四行的按键
				sybuf=413;lybuf=473;
				if(xbuf>=0 && xbuf<=65)
				{							//?123
					colorminus(0,sybuf,64,lybuf,0x23333333);
					while(press!=0);
					scr_restore(0,sybuf,64,lybuf);
					goto numboard;
				}
				else if(xbuf>75 && xbuf<=135)
				{							/* internet / \  */
					colorminus(75,sybuf,135,lybuf,0x23333333);
					start=clock();
					while(press!=0);
					finish=clock();
					if(finish-start>500000)
						strcat(stringbuf,"/");
					else
						strcat(stringbuf,"\\");
					showtext(stringbuf);
					scr_restore(75,sybuf,135,lybuf);
				}
				else if(xbuf>=145 && xbuf<=206)
				{							/* @ # */
					colorminus(145,sybuf,206,lybuf,0x23333333);
					start=clock();
					while(press!=0);
					finish=clock();
					if(finish-start>500000)
						strcat(stringbuf,"@");
					else
						strcat(stringbuf,"#");
					showtext(stringbuf);
					scr_restore(145,sybuf,206,lybuf);
				}
				else if(xbuf>=214 && xbuf<=632)
				{							//SPACE
					colorminus(214,sybuf,632,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf," ");
					showtext(stringbuf);
					scr_restore(214,sybuf,632,lybuf);
				}
				else if(xbuf>=640 && xbuf<=723)
				{							//?123
					colorminus(640,sybuf,723,lybuf,0x23333333);
					while(press!=0);
					scr_restore(640,sybuf,723,lybuf);
					goto numboard;
				}
				else if(xbuf>=731 && xbuf<=795)
				{							//EXIT
					colorminus(731,sybuf,795,lybuf,0x23333333);
					while(press!=0);
					scr_restore(731,sybuf,795,lybuf);
					return stringbuf;
				}
			}
		}
	}
	numboard:while(1)
	{
		clearscr(0,0,800,480);
		bzero(scrbuffer,480*800*4);
		show_bmp("./source/numboard.bmp",200,240,0);
		scr_store(0,0,800,480);
		while(1)
		{
			while(press==0);
			usleep(100);
			xbuf=xpos;ybuf=ypos;
			if(strlen(stringbuf)<254){
				showtext(stringbuf);
			}
			else if(strlen(stringbuf)==255){
				showtext("字符串过长,继续输入将会清空缓冲区!");
			}
			else{
				showtext("缓冲区已清空");
			}
			if(ybuf>=240 && ybuf<300)
			{								//第一行的按键
				sybuf=240;lybuf=300;
				if(xbuf>200 && xbuf<=333)
				{							//1
					colorminus(200,sybuf,333,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"1");
					showtext(stringbuf);
					scr_restore(200,sybuf,333,lybuf);
				}
				else if(xbuf>333 && xbuf<=466)
				{							//2
					colorminus(333,sybuf,466,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"2");
					showtext(stringbuf);
					scr_restore(333,sybuf,466,lybuf);
				}
				else if(xbuf>466 && xbuf<=600)
				{							//3
					colorminus(466,sybuf,600,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"3");
					showtext(stringbuf);
					scr_restore(466,sybuf,600,lybuf);
				}
			}
			else if(ybuf>=300 && ybuf<360)
			{								//第二行的按键
				sybuf=300;lybuf=360;
				if(xbuf>=200 && xbuf<=333)
				{							//4
					colorminus(200,sybuf,333,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"4");
					showtext(stringbuf);
					scr_restore(200,sybuf,333,lybuf);
				}
				else if(xbuf>333 && xbuf<=466)
				{							//5
					colorminus(333,sybuf,466,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"5");
					showtext(stringbuf);
					scr_restore(333,sybuf,466,lybuf);
				}
				else if(xbuf>466 && xbuf<=600)
				{							//6
					colorminus(466,sybuf,600,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"6");
					showtext(stringbuf);
					scr_restore(466,sybuf,600,lybuf);
				}
			}
			else if(ybuf>=360 && ybuf<420)
			{								//第三行的按键
				sybuf=360;lybuf=420;
				if(xbuf>=200 && xbuf<=333)
				{							//7
					colorminus(200,sybuf,333,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"7");
					showtext(stringbuf);
					scr_restore(200,sybuf,333,lybuf);
				}
				else if(xbuf>333 && xbuf<=466)
				{							//8
					colorminus(333,sybuf,466,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"8");
					showtext(stringbuf);
					scr_restore(333,sybuf,466,lybuf);
				}
				else if(xbuf>466 && xbuf<=600)
				{							//9
					colorminus(466,sybuf,600,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"9");
					showtext(stringbuf);
					scr_restore(466,sybuf,600,lybuf);
				}
			}
			else if(ybuf>=420 && ybuf<480)
			{								//第四行的按键
				sybuf=420;lybuf=480;
				if(xbuf>=200 && xbuf<=333)
				{							//.exit
					colorminus(200,sybuf,333,lybuf,0x23333333);
					while(press!=0);
					scr_restore(200,sybuf,333,lybuf);
					goto keyboard;
				}
				else if(xbuf>333 && xbuf<=466)
				{							//0
					colorminus(333,sybuf,466,lybuf,0x23333333);
					while(press!=0);
					strcat(stringbuf,"0");
					showtext(stringbuf);
					scr_restore(333,sybuf,466,lybuf);
				}
				else if(xbuf>466 && xbuf<=600)
				{							//backspace
					colorminus(466,sybuf,600,lybuf,0x23333333);
					while(press!=0){
						stringbuf[strlen(stringbuf)-1]=0;
						usleep(100000);
						showtext(stringbuf);
					}
					scr_restore(466,sybuf,600,lybuf);
				}
			}
		}
	}
}
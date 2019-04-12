#ifndef _COMMONHEADER_H_
#define _COMMONHEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <wait.h>
#include <sys/timeb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <linux/input.h>

#define KEYBOARD 5

int ifexit;

typedef struct attributions{
	char nicname[20];
	int head_portrait[64*64];
}attr;

attr attribute;
bool shutdownenable=false;
bool waitevent=false;
short ifshowacrossbar;
int *scrbuffer;

int ts_fd,lcd_fd,*fbp;
struct input_event tchevent_buf;
short xpos,ypos,press;
short exitevent=0;
short ifshowacrossbar=0;
char image[255]="/mydesk/source/mainpic.bmp";
char sys_strbuf[255];
int system_state;
int sockfd;
int *currentimg;
char sys_msgbuf[255];
int ifrefresh;
#endif

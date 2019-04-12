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

bool shutdownenable;

typedef struct attributions{
	char nicname[20];
	int head_portrait[64*64];
}attr;
attr *serverattr;
#endif

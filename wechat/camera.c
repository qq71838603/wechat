#include "myhead.h"
//自定义一个结构体，用于存放映射之后得到的缓冲块的首地址和大小

int camera()
{
	int camerafd;
	int ret;
	int i;
	//打开usb摄像头的驱动
	camerafd=open("/dev/video7",O_RDWR);//插上摄像头之后，实事求是
	if(camerafd==-1)
	{
		perror("打开摄像头驱动失败!\n");
		return -1;
	}
	//获取摄像头功能参数 
	struct v4l2_capability mycap; //此结构体用于存放功能参数
	bzero(&mycap,sizeof(mycap));
	ret=ioctl(camerafd,VIDIOC_QUERYCAP,&mycap);
	if(ret==-1)
	{
		perror("获取功能参数失败!\n");
		return -1;
	}
	printf("内核版本号是：%ud\n",mycap.version);
	//设置摄像头通道   
	int index=0;
    ret=ioctl(camerafd,VIDIOC_S_INPUT,&index);
	if(ret==-1)
	{
		perror("设置通道失败!\n");
		return -1;
	}
	//获取摄像头当前的采集格式
	struct v4l2_format myfmt;
	bzero(&myfmt,sizeof(myfmt));
    myfmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret=ioctl(camerafd, VIDIOC_G_FMT,&myfmt);
	if(ret==-1)
	{
		perror("获取采集格式失败!\n");
		return -1;
	}
	printf("目前摄像头采集的图片长%d 宽%d 大小%d\n",myfmt.fmt.pix.height,myfmt.fmt.pix.width,myfmt.fmt.pix.sizeimage);
	//申请缓冲区(摄像头内部的缓冲区，为了存放画面)
	struct v4l2_requestbuffers myresbuf;
	bzero(&myresbuf,sizeof(myresbuf));
	myresbuf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	myresbuf.memory=V4L2_MEMORY_MMAP;
	myresbuf.count=4; //申请4个缓冲块
	ret=ioctl(camerafd,VIDIOC_REQBUFS,&myresbuf);
	if(ret==-1)
	{
		perror("申请缓冲块失败!\n");
		return -1;
	}
	//分配缓冲块
	struct v4l2_buffer mybuf;
	bzero(&mybuf,sizeof(mybuf));
	mybuf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	mybuf.memory=V4L2_MEMORY_MMAP;
	//定义一个用户空间结构体数组
	//struct usrbuf  mapbuf[4];
	struct usrbuf *mapbuf=calloc(4,sizeof(struct usrbuf));
	for(i=0; i<4; i++)//因为前面我申请了4块缓冲区
	{
		mybuf.index=i;
		ret=ioctl(camerafd,VIDIOC_QUERYBUF,&mybuf);
		if(ret==-1)
		{
			perror("分配缓冲块失败!\n");
			return -1;
		}
		mapbuf[i].somelength=mybuf.length;
		mapbuf[i].addr=mmap(NULL,mybuf.length,PROT_READ|PROT_WRITE,MAP_SHARED,camerafd,mybuf.m.offset);
		//入队
		ret=ioctl(camerafd,VIDIOC_QBUF,&mybuf);
		if(ret==-1)
		{
			perror("入队失败!\n");
			return -1;
		}
	}	
	//开始摄像头数据采集 
	enum v4l2_buf_type startbuf=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret=ioctl(camerafd,VIDIOC_STREAMON,&startbuf);
	if(ret==-1)
	{
		perror("开始采集失败!\n");
		return -1;
	}
	//出队
	for(i=0; i<4; i++)
	{
		mybuf.index=i;
		ret=ioctl(camerafd, VIDIOC_DQBUF,&mybuf);
		if(ret==-1)
		{
			perror("出队失败!\n");
			return -1;
		}
	}
	
	//将出队获取的画面数据保存到一张空白的jpg图片中
	//新建一个空白的jpg文件
	int jpgfd=open("newcamera.jpg",O_CREAT|O_RDWR|O_TRUNC);
	if(jpgfd==-1)
	{
		perror("新建jpg失败!\n");
		return -1;
	}
	//将出队得到的画面数据写入到空白的jpg
	write(jpgfd,mapbuf[mybuf.index].addr,mapbuf[mybuf.index].somelength);
	close(jpgfd);
	//关闭摄像头数据采集
	ret=ioctl(camerafd,VIDIOC_STREAMOFF,&startbuf);
	if(ret==-1)
	{
		perror("关闭采集失败!\n");
		return -1;
	}
	close(camerafd);
	return 0;
}
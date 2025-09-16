/*
硬件 / 外设控制功能（网关的 "四肢"）

控制摄像头设备（/tmp/webcom），根据指令拍摄照片
*/

#include "data_global.h"

extern char dev_camera_mask;
extern int dev_camera_fd;

extern pthread_mutex_t mutex_camera;

extern pthread_cond_t cond_camera;

/*
根据全局变量dev_camera_mask决定拍摄几张照片

实现需要通过驱动文件，文档没有驱动文件需要进行修改。
*/
void *pthread_camera (void *arg)
{
	unsigned char picture = 0;

	//以读写，不将该设备作为进程的控制终端，非阻塞模式打开摄像头设备文件，路径DEV_CAMERA "/tmp/webcom"
#if 1
	if ((dev_camera_fd = open (DEV_CAMERA, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
	{
		printf ("Cann't open file /tmp/webcom\n");
		exit (-1);
	}
	printf ("pthread_camera is ok\n");
#endif

	while (1)	
	{
		pthread_mutex_lock (&mutex_camera);
		pthread_cond_wait (&cond_camera, &mutex_camera);
		picture = dev_camera_mask;
		pthread_mutex_unlock (&mutex_camera);
#if 1
		for (; picture > 0; picture--)
		{
			write (dev_camera_fd, "one", 3);		
			usleep (600000)	;
		}
#endif
	}
}

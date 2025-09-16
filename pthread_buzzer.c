/*
硬件 / 外设控制功能（网关的 "四肢"）

控制 LED 灯和蜂鸣器（如告警时触发声光提示）
*/

#include "data_global.h"

extern int dev_buzzer_fd;
extern char dev_buzzer_mask;

extern pthread_mutex_t mutex_buzzer;

extern pthread_cond_t cond_buzzer;

/*
外部调用线程后会不断循环执行等待控制信号，根据全局变量dev_buzzer_mask判断并执行蜂鸣器开关，1开0关。

实现需要通过驱动文件，文档没有驱动文件需要进行修改。
*/
void *pthread_buzzer (void *arg)
{
	unsigned char set_buzzer;

	//以读写，非阻塞模式打开蜂鸣器设备文件，文件路径DEV_BUZZER "/dev/dota_beep"
	if ((dev_buzzer_fd = open (DEV_BUZZER, O_RDWR | O_NONBLOCK)) < 0)
	{
		printf ("Cann't open file /dev/beep\n");
		exit (-1);
	}
	printf ("pthread_buzzer is ok\n");

	while (1)
	{
		pthread_mutex_lock (&mutex_buzzer);
		pthread_cond_wait (&cond_buzzer, &mutex_buzzer);
		set_buzzer = dev_buzzer_mask;
		printf ("pthread_buzzer is wake up\n");
		if (0 == set_buzzer)
		{
			ioctl (dev_buzzer_fd, BUZZER_OFF, 0);
			printf("beep off\n");
		}
		else
		{
			printf("beep on\n");
			ioctl (dev_buzzer_fd, BUZZER_ON, 0);
		}
		pthread_mutex_unlock (&mutex_buzzer);
	}

	return 0;
}

/*
硬件 / 外设控制功能（网关的 "四肢"）

控制 LED 灯和蜂鸣器（如告警时触发声光提示）
*/

#include "data_global.h"

extern unsigned char dev_led_mask;
extern int dev_led_fd;//

extern pthread_mutex_t mutex_led;

extern pthread_cond_t cond_led;

/*
外部调用线程后会不断循环执行等待控制信号，根据全局变量dev_led_mask的值来判断并执行不同编号的LED的亮灭
高八位决定编号0123，低八位决定亮灭1亮0灭

ioctl (dev_led_fd, LED_ON, led_no);
ioctl 系统调用 “请求” 内核驱动执行点亮操作。
实现需要通过驱动文件，文档没有驱动文件需要进行修改。
*/ 
void *pthread_led (void *arg)
{
	int led_no;
	unsigned char led_set;
	//以读写，非阻塞方式打开led文件获取文件描述符，文件路径为DEV_LED "/dev/dota_led"
	if ((dev_led_fd = open (DEV_LED, O_RDWR | O_NONBLOCK)) < 0 )
	{
		printf ("Cann't open file /dev/led\n");
		exit (-1);
	}
	printf ("pthread_led is ok\n");
	while (1)
	{
		pthread_mutex_lock (&mutex_led);
		pthread_cond_wait (&cond_led, &mutex_led);//调用后会自动释放互斥锁，让其他线程可以修改共享变量（如dev_led_mask）
		led_set = dev_led_mask;//将共享变量dev_led_mask的值复制到局部变量led_set，后续操作基于led_set，减少对共享变量的占用时间
		pthread_mutex_unlock (&mutex_led);
//		printf ("pthread_led is wake up\n");

		//一次只能控制一个LED亮灭
		switch (led_set & 0xf0)//解析led_set的高 4 位（&0xf0表示只保留高 4 位，低 4 位置 0），用于确定要控制的 LED 编号
		{
			case 0x10:
				{
					led_no = 0;//若高 4 位为0x10（二进制0001 0000），则当前要控制的 LED 编号为 0。
					break;
				}
			case 0x20:
				{
					led_no = 1;
					break;
				}
			case 0x40:
				{
					led_no = 2;
					break;
				}
			case 0x80:
				{
					led_no = 3;
					break;
				}
			default :break;
		}
		led_set &= 0x0f;//保留led_set的低 4 位（&0x0f表示高 4 位置 0，只保留低 4 位），低 4 位通常用于表示 LED 的开关状态。
		if (led_set & (0x1 << led_no))//判断低 4 位中对应led_no的位是否为 1：
		{
			ioctl (dev_led_fd, LED_ON, led_no);//通过ioctl系统调用控制 LED 点亮
		}
		else
		{
			ioctl (dev_led_fd, LED_OFF, led_no);
		}
	}
}

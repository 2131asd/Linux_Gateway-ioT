/*
数据处理功能（网关的 "大脑"）

进行协议转换（将不同设备的协议统一为网关内部格式），衔接数据接收与存储。
*/

#include "link_list.h"
#include "data_global.h"

#define LEN_ENV 12
#define LEN_RFID 4 

extern int dev_uart_fd;

extern linklist linkHead;

extern pthread_cond_t cond_client_send;

extern pthread_mutex_t mutex_linklist;

void serial_init(int fd)
{
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= ( CLOCAL | CREAD );
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB; 
	options.c_iflag |= IGNPAR;
	options.c_iflag &= ~(ICRNL | IXON);
	options.c_oflag = 0;
	options.c_lflag = 0;

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd,TCSANOW,&options);
}

void *pthread_transfer (void *arg)
{

	char flag = 0;
	char uart_buf[100];
	unsigned int n, i,len;

	link_datatype buf;

	linkHead = CreateEmptyLinklist ();
#if 1
	if ((dev_uart_fd = open (DEV_ZIGBEE, O_RDWR)) < 0)
	{
		perror ("open arm-gate_com3  is error");
		exit (-1);
	}
	serial_init (dev_uart_fd);  //init  com3
	printf ("pthread_transfer is ok\n");
#endif
	while (1)
	{
		printf ("waiting for data ***\n");
		memset (&buf, 0, sizeof (link_datatype));
		memset(uart_buf,0,100);
		n=0;
		len=0;

		n=read(dev_uart_fd,uart_buf,1);
		if((n==1)||(uart_buf[0]==0xff))
			n=read(dev_uart_fd,&uart_buf[1],1);		
		else continue;
		
               if((n==1)||(uart_buf[1]==0xff))
		{
			n=read(dev_uart_fd,&uart_buf[2],1);
			len=uart_buf[2];
                     
		}
		else continue;

		if((n==1)||(len<3))
		    continue;
                
                n=read(dev_uart_fd,&uart_buf[3],1);

               if((n==1)||(uart_buf[3]==0x10))
                   {


                  if ((len = read (dev_uart_fd, buf.text, LEN_ENV)) != LEN_ENV)
		    {
			for (i = len; i < LEN_ENV; i++)
			{
			     read (dev_uart_fd, buf.text+i, 1);
			}
		     }
		printf("\nReceive from uart success, date_len = %d\n ", len);
		flag=1;
                 }

		if (1 == flag)
		{
			pthread_mutex_lock (&mutex_linklist);
			if ((InsertLinknode (buf)) == -1)
			{
				pthread_mutex_unlock (&mutex_linklist);
				printf ("NONMEM\n");
			}
			pthread_mutex_unlock (&mutex_linklist);
			flag = 0;
			pthread_cond_signal (&cond_client_send);
		}
		
	}
	return 0;
}

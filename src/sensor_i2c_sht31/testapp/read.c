/*

此测试程序用于验证驱动是否正常工作

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

int process_flag=1;

#define I2C_DEVICE_ADDR	0x88

int main()
{
    int fd=0;
    char buf[50];
    unsigned char tmp[6];
    unsigned short tempori,humiori;//温湿度原始数据
    float temp,humi;
	
    fd = open("/dev/i2c-2",O_RDWR);
    if(fd<0){
        printf("App:Open dev failed.\n");
        goto END;
    }
    
	//设置sht31外设地址
	ret = ioctl(fd, I2C_SLAVE_FORCE, I2C_DEVICE_ADDR);
	if(ret<0){
		printf("App:Set device address fail.\n");
        goto END;
	}
	
	//设置sht31寄存器位宽。其中ioctl第三个参数0表示8bit位宽，1表示16bit位宽
	ret = ioctl(fd, I2C_16BIT_REG, 1);
	if(ret<0){
		printf("App:Set sht31 register bit size fail.\n");
        goto END;
	}
	
	//设置sht31数据位宽。其中ioctl第三个参数0表示8bit位宽，1表示16bit位宽
	ret = ioctl(fd, I2C_16BIT_DATA, 1);
	if(ret<0){
		printf("App:Set sht31 data bit size fail.\n");
        goto END;
	}
    
    while(process_flag){
		
        if(read(STDIN_FILENO,buf,2)>0)
        {
            if(read(fd,tmp,5)>0)
            {
                // for(int i=0;i<4;i++)
                // {
                    // printf("%.2x ",tmp[i]);//打印收到的数据
                // }
                // printf("\n");
                
                tempori=(tmp[0]<<8|tmp[1]);//tmp[2]位为CRC位
                humiori=(tmp[3]<<8|tmp[4]);//tmp[5]位位CRC位
                
                temp=-46.85+175.72/65536*(float)tempori;
                humi=-6.0  +125.0 /65536*(float)humiori;
                
                printf("SHT31-T:%f RH:%f%%\n",temp,humi);
            }
        }
    }

    close(fd);
END:
    exit(0);
}

void L_HandleSig(int signo)
{
	if(SIGINT==signo||SIGTERM==signo)
	{
		process_flag=0;
	}
}
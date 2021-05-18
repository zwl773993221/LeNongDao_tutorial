/*************************************************************************
        > File Name: cli.c
        > Author: zhoulin
        > Mail: 715169549@163.com 
        > Created Time: Sat 02 Jan 2016 06:37:32 PM EST
 ************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <sys/time.h>
#include <errno.h>

#include "sht3x.h"

#define MAXLINE 1024
#define port 10004
#define sleep_time 1

char send_data[10][MAXLINE];

extern void sensirion_i2c_init(void);
extern int16_t sht3x_probe(sht3x_i2c_addr_t addr);
extern int16_t sht3x_measure_blocking_read(sht3x_i2c_addr_t addr, int32_t* temperature,
                                    int32_t* humidity);

int main(int argc,char **argv)
{
	int exec_flag=0;
	char *servInetAddr = "127.0.0.1";
	int socketfd;
	struct sockaddr_in sockaddr;
	char  recvline[MAXLINE], sendline[MAXLINE];
	int n;
    char recv_buf[MAXLINE];
    char send_buf[MAXLINE];
    char *string_send = "A02,01,0324,Nyzdh,123456,WS1802,351-121-01,CA6028A3D3133032,2021- 5-18 18:00,114.101,22.477,23,55,5.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,R:25.5,R:0.42,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,R:600,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,";
	
	int32_t temperature, humidity;

    
	if(argc != 2)
	{
		printf("client <ipaddress> \n");
		exit(0);
	}

	socketfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&sockaddr,0,sizeof(sockaddr));
	
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(19003);
    //sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	inet_pton(AF_INET, servInetAddr, &sockaddr.sin_addr);
    
	if((connect(socketfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))) < 0)
	{
		printf("connect error %s errno: %d\n",strerror(errno),errno);
		exit(0);
	}

	printf("send message to server\n");

	//fgets(sendline,1024,stdin);
	
	struct timeval timeout={2, 0};//三分钟超时设置，3*60秒
    setsockopt(socketfd, SOL_SOCKET,SO_SNDTIMEO, &timeout, sizeof(struct timeval));
    setsockopt(socketfd, SOL_SOCKET,SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    struct tm *t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);
    printf("%4d-%02d-%02d %02d:%02d:%02d\n",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
    strncpy(send_buf,string_send,strlen(string_send));
	printf("string_send:%s\n", string_send);
    printf("strlen(string_send):%ld\n", strlen(string_send));

	//get temperature and humidity
    sensirion_i2c_init();
    while (sht3x_probe(SHT3X_I2C_ADDR_DFLT) != STATUS_OK) {
        printf("SHT sensor probing failed\n");
    }
    printf("SHT sensor probing successful\n");
    sht3x_measure_blocking_read(SHT3X_I2C_ADDR_DFLT, &temperature, &humidity);


	int8_t ret;
	if (ret == STATUS_OK) {
			printf("measured temperature: %0.2f degreeCelsius, measured humidity: %0.2f percentRH\n",
							temperature / 1000.0f, humidity / 1000.0f);
	} else {
			printf("error reading measurement\n");
	}

	if((send(socketfd,send_buf,strlen(string_send),0)) < 0)
	{
		printf("send mes error: %s errno : %d",strerror(errno),errno);
		exit(0);
	}
	
	
    size_t len = recv(socketfd,recv_buf,sizeof(recv_buf), 0);
    if(len > 0) {
        if(strncmp(recv_buf,"NERCITACLOSE",12) == 0) 
        {
            strncpy(recv_buf,"quit",4);
            memset(send_buf,'\0',MAXLINE);
            strncpy(send_buf,"quit",4);
            send(socketfd,send_buf,MAXLINE,0);
            exec_flag=-1;
            return exec_flag;
        }
        else if(strncmp(recv_buf,"NERCITAGETPAR",12) == 0) 
        {
			//需要知道服务器要获取设备的具体参数
            exec_flag=-1;
            return exec_flag;
        }
        else if(strncmp(recv_buf,"NERCITASETPAR",12) == 0) 
        {
			//将服务器需要获取的设置参数，打包发送给服务器
            strncpy(recv_buf,"quit",4);
            memset(send_buf,'\0',MAXLINE);
            strncpy(send_buf,"quit",4);
            send(socketfd,send_buf,MAXLINE,0);
            exec_flag=-1;
            return exec_flag;
        }
        else if(strncmp(recv_buf,"NERCITAUPDATA",12) == 0) 
        {
            char *ptime;
            char pSubdata[128];
            ptime=strchr(&recv_buf[12], ',');
            if(ptime != NULL)
            {
              strncpy(pSubdata, ptime+1, 2);
              pSubdata[2] = '\0';
              *ptime = (unsigned short) atoi(pSubdata);
            }
            ptime=strchr(&recv_buf[14], ',');
            if(ptime != NULL)
            {
              strncpy(pSubdata, ptime+1, 2);
              pSubdata[2] = '\0';
              *ptime = (unsigned short) atoi(pSubdata);
            }
            exec_flag=-1;
            return exec_flag;
        }
        else
        {
            exec_flag=-1;
            return exec_flag;
        }
        recv_buf[len] = '\0';
        exec_flag = 0;
    }

	close(socketfd);
    printf("exit\n");
	exit(0);
   
}



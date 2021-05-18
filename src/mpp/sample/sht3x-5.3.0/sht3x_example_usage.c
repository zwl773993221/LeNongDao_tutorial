/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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

static int32_t temperature, humidity;

int strReplaceAll(char* str, char* sub, char* replace);
char* F2S(float d, char* str);

int main(int argc,char **argv) {
	int exec_flag=0;
	char *servInetAddr = "127.0.0.1";
	int socketfd;
	struct sockaddr_in sockaddr;
	char  recvline[MAXLINE], sendline[MAXLINE];
	int n;
    char recv_buf[MAXLINE];
    char send_buf[MAXLINE];
    char string_send[MAXLINE] = "A02,01,0324,Nyzdh,123456,WS1802,351-121-01,CA6028A3D3133032,2021- 5-18 18:00,114.101,22.477,23,55,5.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,R:25.55,R:0.42  ,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,R:600,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,";
	
/*
    sensirion_i2c_init();

    while (sht3x_probe(SHT3X_I2C_ADDR_DFLT) != STATUS_OK) {
        printf("SHT sensor probing failed\n");
    }
    printf("SHT sensor probing successful\n");

	int8_t ret = sht3x_measure_blocking_read(SHT3X_I2C_ADDR_DFLT,
		                                     &temperature, &humidity);
	if (ret == STATUS_OK) {
		printf("measured temperature: %0.2f degreeCelsius, "
		       "measured humidity: %0.2f percentRH\n",
		       temperature / 1000.0f, humidity / 1000.0f);
	} else {
		printf("error reading measurement\n");
	}

	sensirion_sleep_usec(1000000);

	int32_t tempInt = (int32_t)temperature*100;
	int32_t humidInt = (int32_t)humidity*100;
*/
	//float tempf = tempInt/100;
	//float humidf = humidInt/100;
	float tempf = 27.77;
	float humidf = 0.77;

	char tempv[10];
	char humidv[10];

	//get two byte after point
	char tempstr[10] = "25.55";
	char humidstr[10] = "0.42";

	//float convert to char
	char* ptemp = gcvt(tempf, 5, tempv);
	char* phumid = gcvt(humidf, 5, humidv);
	printf("ptemp:%s, phumid:%s\n", ptemp, phumid);

	if(argc != 2)
	{
		printf("client <ipaddress> \n");
		exit(0);
	}

	socketfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&sockaddr,0,sizeof(sockaddr));
	
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(10004);
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

	//convert real temp and humid value
	strReplaceAll(string_send, tempstr, ptemp);
	strReplaceAll(string_send, humidstr, phumid);

    strncpy(send_buf,string_send,strlen(string_send));

	printf("string_send:%s\n", string_send);
    printf("strlen(string_send):%ld\n", strlen(string_send));

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


char* F2S(float d, char* str)
{
	char str1[40];
	int j = 0, k, i;
	i = (int)d;//浮点数的整数部分
	while (i > 0)
	{
		str1[j++] = i % 10 + '0';
		i = i / 10;
	}
	for (k = 0; k < j; k++)
	{
		str[k] = str1[j - 1 - k];//被提取的整数部分正序存放到另一个数组
	}


	str[j++] = '.';

	d = d - (int)d;//小数部分提取
	for (i = 0; i < 10; i++)
	{
		d = d * 10;
		str[j++] = (int)d + '0';
		d = d - (int)d;
	}
	while (str[--j] == '0');
	str[++j] = '\0';
	return str;
}

int strReplaceAll(char* str, char* sub, char* replace) 
{

	if (NULL == str || NULL == sub || NULL == replace) {
		printf("mystrcat param error\n");
		//return PARAM_ERR;
	}

	char* p = NULL;
	char* t = NULL;
	char* q = NULL;
	char* dst = NULL;
	char* src = NULL;


	int len = strlen(str);
	int len1 = strlen(sub);
	int len2 = strlen(replace);

	p = str;
	while ('\0' != *p) {
		t = str + len;
		q = strstr(str, sub);
		if (NULL == q) /*没有子串了，那么直接返回吧*/ {
			break;
		}

		src = q + len1; /*源头, 原有sub后的一个字符*/
		dst = q + len2; /*目的，放完replace后的一个字符*/
		memcpy(dst, src, t - src); /*原有字符串后移，放出空间*/
		memcpy(q, replace, len2); /*将replace字符拷贝进来*/
		len = len + len2 - len1;

		p = q + len2; /* p 下一轮replace后的一个字符 */
	}

	str[len] = '\0'; /*通过'\0'表示结尾*/
	return 0;
}


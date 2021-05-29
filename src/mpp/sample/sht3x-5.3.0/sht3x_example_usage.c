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
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <string.h>

#include "sht3x.h"
#include "hi_i2c.h"

#define MAXLINE 1024
#define portnumber 19003
#define sleep_time 1

static int32_t temperature, humidity, luminance;
//static int32_t accqTimeInterval = 10;//default 10s update sensor data

int strReplaceAll(char* str, char* sub, char* replace);
char* F2S(float d, char* str);
//int getsensorvalue(void);
int sht3x(int32_t* temperature, int32_t* humidity);
char *my_itoa(int num, char *str);



int main(int argc,char **argv) {
	int exec_flag = 0;
	char* servInetAddr = "192.168.8.222";	//init address
	int socketfd;
	struct sockaddr_in sockaddr;
    char recv_buf[MAXLINE];
    char send_buf[MAXLINE];
    char string_send[MAXLINE] = "A02,01,0324,Nyzdh,123456,WS1802,351-121-01,CA6028A3D3133032,2021-05-18 18:00:58,114.101,22.477,23,55,5.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,R:25.55,R:0.42,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,R:600,S:00.0,S:00.0,S:00.0,S:00.0,S:00.0,";

	//char receInteravtiveProtocol[] = "NERCITASETPAR210.12.220.75:19003:10010:01003,01,12,2,1";
	char sendInteractiveProtocol[MAXLINE] = "CA6028A3D3133032,NERCITA,210.12.220.75:19003:10010:01003,01,12,2,2021-05-23 17:30";
	
	char sendIntervalStr[3];
	char stoIntervalStr[3];
	char picIntervalStr[3];
	char sendIntervalVal = 0;
	char storgeIntervalVal = 0;
	char picUploadNumberVal = 0;
	int32_t paraCounter = 0;
	sendIntervalVal = 20;

//	int32_t temperature = 0;
	//int32_t humidity = 0;
	//int32_t luminance = 0;

	int32_t tempInt = 0;
	int32_t humidInt = 0;

	float tempf = 0;				//not same use line 96 and 97
	float humidf = 0;				//not same use line 96 and 97

	//96-109 deal sensor data convert to string, 139-146 add this string to send_buf
/*	float tempf = 27.77;			//not same use line 92 and 93
	float humidf = 0.77;			//not same use line 92 and 93
	int luminance = 888;
*/
	char tempv[20]={0};
	char humidv[20]={0};
	char luminanv[20]={0};

	//get two byte after point
	char tempstr[10] = "25.55";
	char humidstr[10] = "0.42";
	char luminanstr[10] = "600";	


	//input argv number
	//ipaddress and portnumber not use
	if(argc != 3)
	{
		//judge the argc number, and tip the style, 
		//argv[0] = ./sht3x_example_usage
		//argv[1] = ipaddress, for example:192.168.8.222
		//example:./sht3x_example_usage 210.12.220.75
		printf("client <ipaddress> <portnumber>\n");
		exit(0);
	}

	//socket init setting
	//line 86-line 112
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&sockaddr,0,sizeof(sockaddr));	

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(portnumber);
    //sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//inet_pton(AF_INET, servInetAddr, &sockaddr.sin_addr);
	inet_pton(AF_INET, argv[1], &sockaddr.sin_addr);
    
	if((connect(socketfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))) < 0)
	{
		printf("connect error %s errno: %d\n",strerror(errno),errno);
		exit(0);
	}else{
		printf("connect server success.\n");
	}

	printf("send message to server\n");

	//fgets(sendline,1024,stdin);
	
	struct timeval timeout={2, 0};//三分钟超时设置，3*60秒
    setsockopt(socketfd, SOL_SOCKET,SO_SNDTIMEO, &timeout, sizeof(struct timeval));
    setsockopt(socketfd, SOL_SOCKET,SO_RCVTIMEO, &timeout, sizeof(struct timeval));

	//get local time
    struct tm *t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);
    printf("%4d-%02d-%02d %02d:%02d:%02d\n",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
/*
	//first get sensor value
	//getsensorvalue();

	//get sensor data and deal
	//include isl29013 and sht3x
	//use the sensor data to assenble the send_buf


	//isl29013 i2c-1 light sensor
	// Create I2C bus i2c-1
	int file;
	char *bus = "/dev/i2c-1";
	if ((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, ISL29003 I2C address is 0x44(68)
	ioctl(file, I2C_SLAVE, 0x88);

	// Select command register(0x00)
	// Normal operation, 16-bit resolution(0x80)
	char config[2] = {0};
	config[0] = 0x00;
	config[1] = 0x80;
	write(file, config, 2);
	// Select control register(0x01)
	// Gain = 64000 lux(0x0C)
	config[0] = 0x01;
	config[1] = 0x0C;
	write(file, config, 2);
	sleep(1);

	// Read 2 bytes of data from register (0x04)
	// luminance lsb, luminance msb
	char reg[1] = {0x04};
	write(file, reg, 1);
	char data[2] = {0};
	if(read(file, data, 2) != 2)
	{
		printf("Error : Input/Output error \n");
	}
	else
	{
		// Convert the data
		luminance = (data[1] * 256 + data[0]);

		// Output data to screen
		printf("Ambient Light Luminance : %d lux \n", luminance);
		fflush(stdout);
	}
	sleep(2);
	close(file);

	// Select command register(0x00)
	// Normal operation, 16-bit resolution(0x00)
	//clear register, and software reset, retart get new value
	//config[0] = 0x00;
	//config[1] = 0x00;
	//write(file, config, 2);
	//sleep(2);


	//get sht3x value, in i2c-2
	//i2c-2 temperature sensor
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

	//sensirion_i2c_release();
	sleep(2);

*/

	tempInt = (int32_t)temperature;
	humidInt = (int32_t)humidity;
	printf("tempInt:%d, humidInt:%d, luminance:%d\n", tempInt, humidInt, luminance);

	tempf = (float)tempInt/1000;		//not same use line 96 and 97
	humidf = (float)humidInt/1000;	//not same use line 96 and 97

	printf("tempf:%.2f, humidf:%.2f, luminance:%d\n", tempf, humidf, luminance);

	//float convert to char
	char* ptemp = gcvt(tempf, 4, tempv);
	char* phumid = gcvt(humidf, 4, humidv);
	//int convert to char
	char* plumi = my_itoa(luminance, luminanv);

	printf("ptemp:%s, phumid:%s, plumi:%s\n", ptemp, phumid, plumi);

	//replace real temp/humid/luminance value
	strReplaceAll(string_send, tempstr, ptemp);
	strReplaceAll(string_send, humidstr, phumid);
	strReplaceAll(string_send, luminanstr, plumi);

    strncpy(send_buf, string_send, strlen(string_send));

	printf("string_send:%s\n", string_send);
    printf("strlen(string_send):%d\n", strlen(string_send));

	//send client data to server
	//the data is assembled
	if((send(socketfd,send_buf,strlen(string_send),0)) < 0)
	{
		printf("send mes error: %s errno : %d",strerror(errno),errno);
		exit(0);
	}else{
		printf("send_buf success.\n");
	}
	
	//receive data from server
	//Parse the data received from the server
	printf("Wait for the server information.\n");
	while(1)
	{
		int len = recv(socketfd,recv_buf,sizeof(recv_buf), 0);
		printf("1.receive data:%s, and data length:%d\n", recv_buf, len);

		if(len > 0){
			if(strncmp(recv_buf,"NERCITACLOSE",12) == 0) 
			{
				printf("1.NERCITACLOSE\n");
				printf("server receive data success. and now close the connect.\n");
				close(socketfd);
				printf("exit\n");
				exit(0);
			}
			else if(strncmp(recv_buf,"NERCITAGETPAR",13) == 0) 
			{
				printf("2.NERCITAGETPAR\n");
				//需要知道服务器要获取设备的具体参数,采集数据的时间间隔
				//send client data to server
				//the data is assembled
				if((send(socketfd, sendInteractiveProtocol, strlen(sendInteractiveProtocol),0)) < 0)
				{
					printf("send interactive protocol message error: %s errno : %d",strerror(errno),errno);
					exit(0);
				}else{
					printf("sendInteractiveProtocol success.\n");
				}

			}
			else if(strncmp(recv_buf,"NERCITASETPAR",13) == 0) 
			{
				printf("3.NERCITASETPAR\n");
				//get the parameter from server
				strncpy(sendIntervalStr, recv_buf+36, 2);	//get interval data
				sendIntervalStr[2] = '\0';
				sendIntervalVal = atoi(sendIntervalStr);
				printf("2.reveive data. sendIntervalStr:%s, sendIntervalVal:%d\n", sendIntervalStr, sendIntervalVal);

				strncpy(stoIntervalStr, recv_buf+39, 2);	//get interval data
				stoIntervalStr[2] = '\0';
				storgeIntervalVal = atoi(stoIntervalStr);
				printf("2.reveive data. stoIntervalStr:%s, storgeIntervalVal:%d\n", stoIntervalStr, storgeIntervalVal);

				strncpy(picIntervalStr, recv_buf+42, 2);	//get interval data
				picIntervalStr[2] = '\0';
				picUploadNumberVal = atoi(picIntervalStr);
				printf("2.reveive data. picIntervalStr:%s, picUploadNumberVal:%d\n", picIntervalStr, picUploadNumberVal);

				
			}
			else if(strncmp(recv_buf,"NERCITAUPDATA",13) == 0) 
			{
				printf("4.NERCITAUPDATA\n");
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
				//printf other receive message.
				printf("3.receive data:%s, and data length:%d\n", recv_buf, (int)len);
			}

			//recv_buf[len] = '\0';
			//exec_flag = 0;
		}else{
			if((len<0) &&(errno == EAGAIN||errno == EWOULDBLOCK||errno == EINTR))
			{
				printf("continue...\n");
				sleep(1);
				paraCounter++;
				printf("paraCounter:%d.\n", paraCounter);

				if((paraCounter == sendIntervalVal) && (sendIntervalVal > 1)){
					//sendInteractiveProtocol data to server
					//the data is assembled
					if((send(socketfd,sendInteractiveProtocol,strlen(sendInteractiveProtocol),0)) < 0)
					{
						printf("send mes error: %s errno : %d",strerror(errno),errno);
						exit(0);
					}else{
						printf("send_buf success.\n");
					}

					//send sensor data to server
					//sensor value
					
					//get sensor data and deal
					//include isl29013 and sht3x
					//use the sensor data to assenble the send_buf


					//isl29013 i2c-1 light sensor
					// Create I2C bus i2c-1
					int file;
					char *bus = "/dev/i2c-1";
					if ((file = open(bus, O_RDWR)) < 0) 
					{
						printf("Failed to open the bus. \n");
						exit(1);
					}
					// Get I2C device, ISL29003 I2C address is 0x44(68)
					ioctl(file, I2C_SLAVE, 0x88);

					// Select command register(0x00)
					// Normal operation, 16-bit resolution(0x80)
					char config[2] = {0};
					config[0] = 0x00;
					config[1] = 0x80;
					write(file, config, 2);
					// Select control register(0x01)
					// Gain = 64000 lux(0x0C)
					config[0] = 0x01;
					config[1] = 0x0C;
					write(file, config, 2);
					sleep(1);

					// Read 2 bytes of data from register (0x04)
					// luminance lsb, luminance msb
					char reg[1] = {0x04};
					write(file, reg, 1);
					char data[2] = {0};
					if(read(file, data, 2) != 2)
					{
						printf("Error : Input/Output error \n");
					}
					else
					{
						// Convert the data
						luminance = (data[1] * 256 + data[0]);

						// Output data to screen
						printf("Ambient Light Luminance : %d lux \n", luminance);
						fflush(stdout);
					}
					sleep(2);
					close(file);

					// Select command register(0x00)
					// Normal operation, 16-bit resolution(0x00)
					//clear register, and software reset, retart get new value
					//config[0] = 0x00;
					//config[1] = 0x00;
					//write(file, config, 2);
					//sleep(2);


					//get sht3x value, in i2c-2
					//i2c-2 temperature sensor
/*					
					sensirion_i2c_init();

					while (sht3x_probe(SHT3X_I2C_ADDR_DFLT) != STATUS_OK) {
						printf("SHT sensor probing failed\n");
					}
					printf("SHT sensor probing successful\n");

*/
					int8_t ret = sht3x_measure_blocking_read(SHT3X_I2C_ADDR_DFLT,
												             &temperature, &humidity);
					if (ret == STATUS_OK) {
						printf("measured temperature: %0.2f degreeCelsius, "
							   "measured humidity: %0.2f percentRH\n",
							   temperature / 1000.0f, humidity / 1000.0f);
					} else {
						printf("error reading measurement\n");
					}

					//sensirion_i2c_release();
					sleep(2);

					tempInt = (int32_t)temperature;
					humidInt = (int32_t)humidity;
					printf("tempInt:%d, humidInt:%d, luminance:%d\n", tempInt, humidInt, luminance);

					tempf = (float)tempInt/1000;		//not same use line 96 and 97
					humidf = (float)humidInt/1000;	//not same use line 96 and 97

					printf("tempf:%.2f, humidf:%.2f, luminance:%d\n", tempf, humidf, luminance);

					//float convert to char
					char* pptemp = gcvt(tempf, 4, tempv);
					char* pphumid = gcvt(humidf, 4, humidv);
					//int convert to char
					char* pplumi = my_itoa(luminance, luminanv);

					printf("pptemp:%s, pphumid:%s, pplumi:%s\n", pptemp, pphumid, pplumi);

					//replace real temp/humid/luminance value
					strReplaceAll(string_send, tempstr, pptemp);
					strReplaceAll(string_send, humidstr, pphumid);
					strReplaceAll(string_send, luminanstr, pplumi);

					strncpy(send_buf,string_send,strlen(string_send));

					printf("string_send:%s\n", string_send);
					printf("strlen(string_send):%d\n", strlen(string_send));

					//send client data to server
					//the data is assembled
					if((send(socketfd,send_buf,strlen(string_send),0)) < 0)
					{
						printf("send mes error: %s errno : %d",strerror(errno),errno);
						exit(0);
					}else{
						printf("send_buf success.\n");
					}


					paraCounter = 0;
				}


				continue;//继续接收数据
			}
			printf("break.....\n");
			//break;//跳出接收循环
		}


	}
	close(socketfd);
    printf("exit\n");
	exit(0);


}

int getsensorvalue(void)
{

	//get sensor data and deal
	//include isl29013 and sht3x
	//use the sensor data to assenble the send_buf


	//isl29013 i2c-1 light sensor
	// Create I2C bus i2c-1
	int file;
	char *bus = "/dev/i2c-1";
	if ((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, ISL29003 I2C address is 0x44(68)
	ioctl(file, I2C_SLAVE, 0x88);

	// Select command register(0x00)
	// Normal operation, 16-bit resolution(0x80)
	char config[2] = {0};
	config[0] = 0x00;
	config[1] = 0x80;
	write(file, config, 2);
	// Select control register(0x01)
	// Gain = 64000 lux(0x0C)
	config[0] = 0x01;
	config[1] = 0x0C;
	write(file, config, 2);
	sleep(1);

	// Read 2 bytes of data from register (0x04)
	// luminance lsb, luminance msb
	char reg[1] = {0x04};
	write(file, reg, 1);
	char data[2] = {0};
	if(read(file, data, 2) != 2)
	{
		printf("Error : Input/Output error \n");
	}
	else
	{
		// Convert the data
		luminance = (data[1] * 256 + data[0]);

		// Output data to screen
		printf("Ambient Light Luminance : %d lux \n", luminance);
		fflush(stdout);
	}
	sleep(2);
	close(file);

	// Select command register(0x00)
	// Normal operation, 16-bit resolution(0x00)
	//clear register, and software reset, retart get new value
	//config[0] = 0x00;
	//config[1] = 0x00;
	//write(file, config, 2);
	//sleep(2);


	//get sht3x value, in i2c-2
	//i2c-2 temperature sensor
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

	sensirion_i2c_release();
	sleep(2);

	//Software reset, retart get new value
	//sht3x_clear_status(SHT3X_I2C_ADDR_DFLT);
	//sleep(2);

	
	return 1;
}


char *my_itoa(int num, char *str)
{
	if(str == NULL)
	{
		return NULL;
	}
	sprintf(str, "%d", num);

	return str;
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


	size_t len = strlen(str);
	size_t len1 = strlen(sub);
	size_t len2 = strlen(replace);

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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "sample_comm.h"
#include "zimo.h"
#include "osd.h"
#include "hisi_osd.h"
//////////////////////////////////
#define DVR_ALL_CH_NUM 1
#define DOORDVR_CHANNEL_NUM 4
typedef struct tag_OsdParam
{
    BITMAP_S pstBitmap;
} OSDPARAM_T;

typedef struct __MANAGE_TIME_CHSTRING_T
{
    pthread_mutex_t osd_mutex_lock[DVR_ALL_CH_NUM];
    OSDPARAM_T osdChStringParam[DVR_ALL_CH_NUM];
    OSDPARAM_T osdTimeParam[DVR_ALL_CH_NUM];
    char   *zimo_buffer;//字模
    unsigned char   old_time[DVR_ALL_CH_NUM][20];//时间字符串
    int osd_TimeStates[DVR_ALL_CH_NUM];//使能时间OSD
    int  isCreateStringRegion[DVR_ALL_CH_NUM];
    int  isCreateTimeRegion[DVR_ALL_CH_NUM];
} MANAGE_TIME_CHSTRING_T;



#define TIMESTRING  "2011/03/05 00:00:00"

static MANAGE_TIME_CHSTRING_T manageTimeChStingOsd;
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
char Osd_Init(void)
{
    unsigned char i;
    bzero(&manageTimeChStingOsd,sizeof(MANAGE_TIME_CHSTRING_T));
    for(i=0; i<DVR_ALL_CH_NUM; i++)
        pthread_mutex_init(&manageTimeChStingOsd.osd_mutex_lock[i], NULL);
    manageTimeChStingOsd.zimo_buffer = malloc(34 * 16);/*分配字模空间*/
    if(manageTimeChStingOsd.zimo_buffer ==NULL)
    {
        printf("malloc zimo space fail\n");
        exit(-1);
    }
    return 0;
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void Osd_UnInit(void)
{
    unsigned char i;

    for(i=0; i<DVR_ALL_CH_NUM; i++)
        pthread_mutex_destroy(&manageTimeChStingOsd.osd_mutex_lock[i]);
    if(manageTimeChStingOsd.zimo_buffer)
    {
        free(manageTimeChStingOsd.zimo_buffer);
        manageTimeChStingOsd.zimo_buffer=NULL;
    }
    bzero(&manageTimeChStingOsd,sizeof(MANAGE_TIME_CHSTRING_T));
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
**说    明:
***********************************************************************************************************/
void Osd_CreateForChString(unsigned char ch)
{
    char ch_name[64];
    unsigned int  osdString_length;
    RECT_S rect;
    BITMAP_S *Bitmap;
    unsigned int RgnHandle=ch;
    int VencGrp=ch;
	
	sprintf(ch_name,"网络camera%02d",ch);
    pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);

    osdString_length = strlen(ch_name)*8;
	if(osdString_length<4)
		osdString_length=4;
	
    rect.s32X=10;
    rect.s32Y=10;
    rect.u32Width=osdString_length;
    rect.u32Height=18;
    if(HI_FAILURE==Hqt_Osd_CreateRegions(RgnHandle,VencGrp,rect,0))/*创建区域*/
    {
		pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
		return ;
	}
    Bitmap=&manageTimeChStingOsd.osdChStringParam[VencGrp].pstBitmap;
    Bitmap->u32Width=rect.u32Width;
    Bitmap->u32Height=rect.u32Height;
    Hqt_Osd_RgnLoadBmp(Bitmap);/*载入位图*/
    osd_update(Bitmap->pData, \
               ch_name, \
               NULL, \
               rect.u32Width, \
               0, \
               0, \
               manageTimeChStingOsd.zimo_buffer);
    Hqt_Osd_RgnSetBitMap(RgnHandle,Bitmap);/*设置位图到OSD中*/
	Hqt_Osd_RgnShowOrHide(RgnHandle,VencGrp,HI_TRUE);
    manageTimeChStingOsd.isCreateStringRegion[VencGrp]=HI_TRUE;
    pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
	
}

void Osd_HideChString(unsigned char ch)
{
    unsigned int RgnHandle=ch;
     int VencGrp=ch;
    pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
    if(manageTimeChStingOsd.isCreateStringRegion[VencGrp])
        Hqt_Osd_RgnShowOrHide(RgnHandle,VencGrp,HI_FALSE);
    pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
}

void Osd_ShowChString(unsigned char ch)
{
    unsigned int RgnHandle=ch;
     int VencGrp=ch;
    pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
    if(manageTimeChStingOsd.isCreateStringRegion[VencGrp])
        Hqt_Osd_RgnShowOrHide(RgnHandle,VencGrp,HI_TRUE);
    pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
}

void Osd_DestroyChString(unsigned char ch)
{
    unsigned int RgnHandle=ch;
     int VencGrp=ch;
    pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
    Hqt_Osd_RgnDetachFrmChn(RgnHandle,VencGrp);
    Hqt_Osd_RgnDestroy(RgnHandle);
    manageTimeChStingOsd.isCreateStringRegion[VencGrp]=HI_FALSE;
    pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
}

void Osd_ChangeChStringPos(unsigned char ch, int x_pos, int y_pos)
{
    unsigned int RgnHandle=ch;
     int VencGrp=ch;
	POINT_S  m_point;
	m_point.s32X=x_pos;
	m_point.s32Y=y_pos;

	Hqt_Osd_RgnChgPosition(RgnHandle,VencGrp,&m_point);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:DVR_ALL_CH_NUM 开始为时间的区域句柄
***********************************************************************************************************/
void Osd_CreateForChTime(unsigned char ch)
{
    unsigned int  osdTime_length;
    RECT_S rect;
    BITMAP_S *Bitmap;
    unsigned int RgnHandle=ch+DVR_ALL_CH_NUM;
     int VencGrp=ch;

    pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
    osdTime_length = strlen(TIMESTRING)*8;
    rect.s32X=10;
    rect.s32Y=10+32;
    rect.u32Width=osdTime_length;
    rect.u32Height=16;
	if(ch>=DOORDVR_CHANNEL_NUM)
	{
		rect.s32X=80;
	}

    if(HI_FAILURE==Hqt_Osd_CreateRegions(RgnHandle,VencGrp,rect,1))/*创建区域*/
    {
		pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
		return ;
	}
    Bitmap=&manageTimeChStingOsd.osdTimeParam[VencGrp].pstBitmap;
    Bitmap->u32Width=rect.u32Width;
    Bitmap->u32Height=rect.u32Height;
    Hqt_Osd_RgnLoadBmp(Bitmap);/*载入位图*/
    memset(manageTimeChStingOsd.old_time[ch], '\0', strlen(TIMESTRING));
	manageTimeChStingOsd.osd_TimeStates[VencGrp]=1;
	Hqt_Osd_RgnShowOrHide(RgnHandle,VencGrp,HI_TRUE);

    manageTimeChStingOsd.isCreateTimeRegion[VencGrp]=HI_TRUE;
    pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void Osd_HideChTime(unsigned char ch)
{
    unsigned int RgnHandle=ch+DVR_ALL_CH_NUM;
     int VencGrp=ch;
    pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
    if(manageTimeChStingOsd.isCreateTimeRegion[VencGrp])
        Hqt_Osd_RgnShowOrHide(RgnHandle,VencGrp,HI_FALSE);
    manageTimeChStingOsd.osd_TimeStates[VencGrp]=0;
    pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/

void Osd_ShowChTime(unsigned char ch)
{
    unsigned int RgnHandle=ch+DVR_ALL_CH_NUM;
     int VencGrp=ch;
    pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
    if(manageTimeChStingOsd.isCreateTimeRegion[VencGrp])
        Hqt_Osd_RgnShowOrHide(RgnHandle,VencGrp,HI_TRUE);
    manageTimeChStingOsd.osd_TimeStates[VencGrp]=1;
    pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/

void Osd_DestroyChTime(unsigned char ch)
{
    unsigned int RgnHandle=ch+DVR_ALL_CH_NUM;
     int VencGrp=ch;
    pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
    Hqt_Osd_RgnDetachFrmChn(RgnHandle,VencGrp);
    Hqt_Osd_RgnDestroy(RgnHandle);
    manageTimeChStingOsd.isCreateTimeRegion[VencGrp]=HI_FALSE;
    manageTimeChStingOsd.osd_TimeStates[VencGrp]=0;
    pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[VencGrp]);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void Osd_ChangeChTimePos(unsigned char ch, int x_pos, int y_pos)
{
    unsigned int RgnHandle=ch+DVR_ALL_CH_NUM;
     int VencGrp=ch;
	POINT_S  m_point;
	m_point.s32X=x_pos;
	m_point.s32Y=y_pos;

	Hqt_Osd_RgnChgPosition(RgnHandle,VencGrp,&m_point);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
static unsigned char osdCh[DVR_ALL_CH_NUM];
void AddChannelOsd(unsigned char ch)
{
	pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[ch]);
	osdCh[ch]=1;
	pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[ch]);

}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void RemoveChannelOsd(unsigned char ch)
{
	pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[ch]);
	osdCh[ch]=0;
	pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[ch]);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void SYS_Osd_Create(int ch)
{
	Osd_CreateForChString(ch);
	Osd_CreateForChTime(ch);
	AddChannelOsd(ch);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void SYS_Osd_Destroy(int ch)
{
	Osd_DestroyChTime(ch);
	Osd_DestroyChString(ch);
	RemoveChannelOsd(ch);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void Sys_Osd_UpdateByCh(int ch)
{
    time_t  DTTimer ;
    struct tm    *DTDate;
	char new_time[32];

	pthread_mutex_lock(&manageTimeChStingOsd.osd_mutex_lock[ch]);
	if(osdCh[ch]&&manageTimeChStingOsd.osd_TimeStates[ch])
	{
		DTTimer = time(NULL);
		DTDate = localtime(&DTTimer);
		sprintf(new_time, "%04d/%02d/%02d %02d:%02d:%02d", DTDate->tm_year + 1900, \
				DTDate->tm_mon + 1, \
				DTDate->tm_mday, \
				DTDate->tm_hour, \
				DTDate->tm_min, \
				DTDate->tm_sec);
		osd_update(manageTimeChStingOsd.osdTimeParam[ch].pstBitmap.pData, \
				   new_time, \
				   (char*)manageTimeChStingOsd.old_time[ch], \
				   manageTimeChStingOsd.osdTimeParam[ch].pstBitmap.u32Width, \
				   0, \
				   0, \
				   manageTimeChStingOsd.zimo_buffer);
		memcpy(manageTimeChStingOsd.old_time[ch], new_time, strlen(new_time));
	   HI_MPI_RGN_SetBitMap(ch+DVR_ALL_CH_NUM,&manageTimeChStingOsd.osdTimeParam[ch].pstBitmap);/*设置位图到OSD中*/
	}
	pthread_mutex_unlock(&manageTimeChStingOsd.osd_mutex_lock[ch]);
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void Sys_Osd_Update(void)
{
	int i;
	for(i = 0; i < DVR_ALL_CH_NUM; i++)
	{
		Sys_Osd_UpdateByCh(i);
	}
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void SYS_Osd_Init(void)
{
    Osd_Init();
}
/***********************************************************************************************************
**函    数:
**输入参数:
**功    能:
**返回  值:
***********************************************************************************************************/
void SYS_Osd_UnInit(void)
{
	Osd_UnInit();
}



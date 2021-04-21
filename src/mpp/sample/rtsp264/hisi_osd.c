#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "sample_comm.h"
/***********************************************************************************************************
**函数:
**输入参数:
**功能:
**返回值:
***********************************************************************************************************/
int Hqt_Osd_CreateRegions(unsigned int RgnHandle,int VencGrp,RECT_S rect,unsigned int layer)
{
    RGN_ATTR_S stRgnAttr;
    HI_S32 s32Ret = HI_FAILURE;
    MPP_CHN_S stChn;
    RGN_CHN_ATTR_S stChnAttr;
    int multiple=0;

    bzero(&stRgnAttr,sizeof(RGN_ATTR_S));
    stRgnAttr.enType = OVERLAY_RGN;
    stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_RGB_1555;
    multiple=rect.u32Width%2;
    stRgnAttr.unAttr.stOverlay.stSize.u32Width  =rect.u32Width-multiple;
    multiple=rect.u32Height%2;
    stRgnAttr.unAttr.stOverlay.stSize.u32Height =rect.u32Height-multiple;
    stRgnAttr.unAttr.stOverlay.u32BgColor = 64;
    s32Ret = HI_MPI_RGN_Create(RgnHandle, &stRgnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_Create (%d) failed with %#x!\n", RgnHandle, s32Ret);
        HI_MPI_RGN_Destroy(RgnHandle);
        return HI_FAILURE;
    }
	
    stChn.enModId = HI_ID_GROUP;
    stChn.s32DevId =VencGrp;
    stChn.s32ChnId = 0;
    memset(&stChnAttr,0,sizeof(stChnAttr));
    stChnAttr.bShow = HI_FALSE;
    stChnAttr.enType = OVERLAY_RGN;
    multiple=rect.s32X%4;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X =rect.s32X-multiple;
    multiple=rect.s32Y%4;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y =rect.s32Y-multiple;
    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 128;
    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;
    stChnAttr.unChnAttr.stOverlayChn.u32Layer = layer;

    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = HI_FALSE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp  = 0;

    s32Ret = HI_MPI_RGN_AttachToChn(RgnHandle, &stChn, &stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_AttachToChn (%d) failed with %#x!\n",RgnHandle, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
/***********************************************************************************************************
**函数:
**输入参数:
**功能:
**返回值:
***********************************************************************************************************/
int Hqt_Osd_RgnChange(unsigned int RgnHandle, int VencGrp, SAMPLE_RGN_CHANGE_TYPE_EN enChangeType, unsigned int u32Val)
{
    MPP_CHN_S stChn;
    RGN_CHN_ATTR_S stChnAttr;
    HI_S32 s32Ret;

    stChn.enModId = HI_ID_GROUP;
    stChn.s32DevId =VencGrp;
    stChn.s32ChnId = 0;

    // stChn.s32DevId = 0;
    // stChn.s32ChnId = VencGrp;

    s32Ret = HI_MPI_RGN_GetDisplayAttr(RgnHandle,&stChn,&stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_GetDisplayAttr (%d)) failed with %#x!\n",\
                   RgnHandle, s32Ret);
        return HI_FAILURE;
    }

    switch(enChangeType)
    {
        case RGN_CHANGE_TYPE_FGALPHA:
            stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = u32Val;
            break;
        case RGN_CHANGE_TYPE_BGALPHA:
            stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = u32Val;
            break;
        case RGN_CHANGE_TYPE_LAYER:
            stChnAttr.unChnAttr.stOverlayChn.u32Layer = u32Val;
            break;
        default:
            SAMPLE_PRT("input paramter invaild!\n");
            return HI_FAILURE;
    }
    s32Ret = HI_MPI_RGN_SetDisplayAttr(RgnHandle,&stChn,&stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_SetDisplayAttr (%d)) failed with %#x!\n", RgnHandle, s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
/***********************************************************************************************************
**函数:
**输入参数:
**功能:
**返回值:
***********************************************************************************************************/
int Hqt_Osd_RgnChgPosition(RGN_HANDLE RgnHandle, int VencGrp, POINT_S *pstPoint)
{
    MPP_CHN_S stChn;
    RGN_CHN_ATTR_S stChnAttr;
    HI_S32 s32Ret;
    stChn.enModId = HI_ID_GROUP;
    stChn.s32DevId =VencGrp;
    stChn.s32ChnId = 0;
    // stChn.s32DevId = 0;
    // stChn.s32ChnId = VencGrp;
    if(NULL == pstPoint)
    {
        SAMPLE_PRT("input parameter is null. it is invaild!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_RGN_GetDisplayAttr(RgnHandle, &stChn, &stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_GetDisplayAttr (%d)) failed with %#x!\n",\
                   RgnHandle, s32Ret);
        return HI_FAILURE;
    }

    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = pstPoint->s32X;
    stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = pstPoint->s32Y;
    s32Ret = HI_MPI_RGN_SetDisplayAttr(RgnHandle,&stChn,&stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_SetDisplayAttr (%d)) failed with %#x!\n",\
                   RgnHandle, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
/***********************************************************************************************************
**函数:
**输入参数:
**功能:
**返回值:
***********************************************************************************************************/
int Hqt_Osd_RgnLoadBmp(BITMAP_S *pstBitmap)
{
    if(pstBitmap->pData)
        free(pstBitmap->pData);

    pstBitmap->enPixelFormat = PIXEL_FORMAT_RGB_1555;
    pstBitmap->pData =(char*) malloc(2*pstBitmap->u32Width*pstBitmap->u32Height);
    if(NULL == pstBitmap->pData)
    {
        SAMPLE_PRT("malloc osd memroy err!\n");
        return HI_FAILURE;
    }
    memset(pstBitmap->pData, 0,pstBitmap->u32Width*pstBitmap->u32Height * 2);
    return HI_SUCCESS;
}
/***********************************************************************************************************
**函数:
**输入参数:
**功能:
**返回值:
***********************************************************************************************************/
int Hqt_Osd_RgnSetBitMap(unsigned int RgnHandle, BITMAP_S *pstBitmap)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_RGN_SetBitMap(RgnHandle,pstBitmap);
    if(s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_RGN_SetBitMap failed with %#x,RgnHandle=%d!\n", s32Ret,RgnHandle);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
/***********************************************************************************************************
**函数:
**输入参数:
**功能:
**返回值:
***********************************************************************************************************/
 int Hqt_Osd_RgnShowOrHide(unsigned int RgnHandle, int VencGrp, int bShow)
{
    MPP_CHN_S stChn;
    RGN_CHN_ATTR_S stChnAttr;
    HI_S32 s32Ret;

    stChn.enModId = HI_ID_GROUP;
    stChn.s32DevId =VencGrp;
    stChn.s32ChnId = 0;

    //stChn.s32DevId = 0;
    //stChn.s32ChnId = VencGrp;


    s32Ret = HI_MPI_RGN_GetDisplayAttr(RgnHandle, &stChn, &stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_GetDisplayAttr (%d)) failed with %#x!\n",\
                   RgnHandle, s32Ret);
        return HI_FAILURE;
    }

    stChnAttr.bShow = bShow;

    s32Ret = HI_MPI_RGN_SetDisplayAttr(RgnHandle,&stChn,&stChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_SetDisplayAttr (%d)) failed with %#x!\n",\
                   RgnHandle, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/***********************************************************************************************************
**函数:
**输入参数:
**功能:
**返回值:
***********************************************************************************************************/
int Hqt_Osd_RgnDetachFrmChn(unsigned int RgnHandle,unsigned int VencGrp)
{
    MPP_CHN_S stChn;
    int s32Ret;

    stChn.enModId = HI_ID_GROUP;
    stChn.s32DevId =VencGrp;
    stChn.s32ChnId = 0;

    // stChn.s32DevId = 0;
    // stChn.s32ChnId = VencGrp;


    s32Ret = HI_MPI_RGN_DetachFromChn(RgnHandle, &stChn);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_DetachFrmChn (%d) failed with %#x!\n",RgnHandle, s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
/***********************************************************************************************************
**函数:
**输入参数:
**功能:
**返回值:
***********************************************************************************************************/
int Hqt_Osd_RgnDestroy(unsigned int RgnHandle)
{
    int s32Ret;

    s32Ret = HI_MPI_RGN_Destroy(RgnHandle);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_RGN_Destroy [%d] failed with %#x\n",RgnHandle, s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}


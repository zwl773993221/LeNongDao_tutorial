#ifndef __HISI_OSD_H__
#define  __HISI_OSD_H__
int Hqt_Osd_CreateRegions(unsigned int RgnHandle,int VencGrp,RECT_S rect,unsigned int layer);
int Hqt_Osd_RgnChange(unsigned int RgnHandle, int VencGrp, SAMPLE_RGN_CHANGE_TYPE_EN enChangeType, unsigned int u32Val);
int Hqt_Osd_RgnChgPosition(RGN_HANDLE RgnHandle, int VencGrp, POINT_S *pstPoint);
int Hqt_Osd_RgnLoadBmp(BITMAP_S *pstBitmap);
int Hqt_Osd_RgnSetBitMap(unsigned int RgnHandle, BITMAP_S *pstBitmap);
int Hqt_Osd_RgnShowOrHide(unsigned int RgnHandle, int VencGrp, int bShow);
int Hqt_Osd_RgnDetachFrmChn(unsigned int RgnHandle,unsigned int VencGrp);
int Hqt_Osd_RgnDestroy(unsigned int RgnHandle);

///////////////////////////
#endif


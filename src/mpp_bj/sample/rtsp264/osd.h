#ifndef __OSD_H__
#define __OSD_H__
char Osd_Init(void);
void Osd_UnInit(void);
void Osd_CreateForChString(unsigned char ch);
void Osd_HideChString(unsigned char ch);
void Osd_ShowChString(unsigned char ch);
void Osd_DestroyChString(unsigned char ch);
void Osd_ChangeChStringPos(unsigned char ch, int x_pos, int y_pos);
void Osd_CreateForChTime(unsigned char ch);
void Osd_HideChTime(unsigned char ch);
void Osd_ShowChTime(unsigned char ch);
void Osd_DestroyChTime(unsigned char ch);
void Osd_ChangeChTimePos(unsigned char ch, int x_pos, int y_pos);
void AddChannelOsd(unsigned char ch);
void RemoveChannelOsd(unsigned char ch);
void SYS_Osd_Create(int ch);
void SYS_Osd_Destroy(int ch);
void Sys_Osd_UpdateByCh(int ch);
void Sys_Osd_Update(void);
void SYS_Osd_Init(void);
void SYS_Osd_UnInit(void);

#endif


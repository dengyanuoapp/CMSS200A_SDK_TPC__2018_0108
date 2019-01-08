#ifndef _USBDISK_H
#define _USBDISK_H

#ifdef __C251__

typedef struct
{
    uint8 vendor[8];
    uint8 productid[16];
    uint8 productver[4];
} CDROM_INFO_t;

bool UDiskInit(void *pcallback, char udisk_set);
void UDiskSetCDROMInfo(CDROM_INFO_t *cdrom_info); //设置CDROM的厂商及产品信息,要在调用UdiskInit前调用;
void UDiskSetDiskNum(char selector); //设置磁盘出现数目,要在调用UdiskInit前调用
char UDiskGetDiskInfo(void);         //取得磁盘数目情况 bit7:是否分区 bit6:是否加密 bit5:是否插卡
bool UDiskSetWP(char letter);
bool UDiskCheckWP(char letter);
void DisableSuspend(void);           //禁止suspend
bool UDiskRun( int para );           //para=0:正常启动，=1:Switch from MTP启动，
bool UDiskExit(void);
void UDiskHandle(void);
BYTE UDiskGetStatus(void);
BYTE CardWPCheck(void);
bool UDiskPullCardOut(void); //jpu
void AlwaysShowCardDisk(BYTE para);  //0:不显示Card的盘符 1：总显示Card的盘符
BYTE UDiskGetCardStatus(void);       //获取卡是否存在的信息

#endif /*__C251__*/
#endif /*_USBDISK_H*/





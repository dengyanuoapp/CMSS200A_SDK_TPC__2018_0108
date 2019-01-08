/*
********************************************************************************
*                       ACTOS
*               device manager head file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : device.h
* By     : Gongee
* Version: 1> v1.00     first version     2003-11-20 11:35
********************************************************************************
********************************************************************************
*/
#ifndef _ADFU_H
#define _ADFU_H

#include "typeext.h"




#define APINO_GetSysInfo          0x0000+API_ADFU_Base

#define APINO_PrepareforOnlineSwitch     0x1000+API_ADFU_Base

#ifdef __C251__

void BackupZram2scode(void);      //U盘要使用0x4000~0x4fff的空间做buffer，该函数把该空间原有代码备份到0x06 page
void RestoreZram2scode(void);     //恢复显示Driver代码BankB代码到默认Page 0x07


typedef struct
{
    char filename[11];         //文件名8.3
    char type;                 //"H","F","B","I","A","S"
    long DownloadAddr;        //download 地址
    long offset;               //文件中的地址
    long length;              //长度byte为单位（512取整）
    char Subtype[4];          //子类型
    long checksum;
} DIR_t;
typedef struct
{
    char filename[11];      //文件名8.3
    BYTE attr;
    BYTE reserve1[2];
    WORD version;
    long offset;            //以扇区为单位
    long length;            //一字节为单位
    BYTE reserve2[4];
    DWORD checksum;
} LDIR_t;   //32 bytes

typedef struct
{
    char AFIFlag[4];                        //={'A','F','I',' '};
    WORD VID;                                //设备vid
    WORD PID;                                //设备pid
    BYTE Version[4];                        //版本号x.x.xx
    BYTE Date[4];
    BYTE reserve1[16];
    DIR_t diritem[126];                        //最多126
    BYTE reserve2[28];
    long checksum;                            //afi head校验和
} AFIHead_t;       //4k

typedef struct
{
    //magic
    WORD magic;            // must equ. 0xdead
    //系统起始时间(半秒为单位)
    //系统当前的时间等于系统起始时间加系统time counter的时间
    unsigned long systemtime;
    unsigned int RTCRate;   //950
    //对比度选择0~31
    char DisplayContrast;
    //背景灯亮时间,默认值为0ah=5秒
    char LightTime;
    //standby定时
    char StandbyTime;
    //sleep定时
    char SleepTime;
    //界面语言, 简体:0, 英文:1, 繁体:2
    char langid;
    //复读模式
    char ReplayMode;
    //联机模式
    char OnlineMode;

    char BatteryType;                       //modify by gongee 2004-05-24 09:21
    char FMBuildInFlag;                     //add battery select and fm select flag

} ComValforFMK_t;

typedef struct
{
    //0
    BYTE LFIFlag[4];                        //={0x55,0xaa,0xf0,0x0f};
    BYTE Version[4];                        //版本号x.x.xx.xxxx
    BYTE Date[4];                           //xxxx.xx.xx
    WORD PID;                               //xxxx
    WORD VID;                               //xxxx
    long DIRItemCheckSum;                   //所有的240个目录项加起来
    BYTE descriptor[44];                    //firmware描述符
    BYTE Productor[32];
    BYTE DeviceName[32];
    BYTE reserve0[128];
    //256
    BYTE inf_usbattri[8];
    BYTE inf_usbidentification[16];
    BYTE inf_usbproductver[4];
    BYTE reserve1[4];
    BYTE inf_descriptorlen;                    //must equ. 48
    BYTE inf_descriptortype;                //must equ. 3
    BYTE inf_usbdescripstr[46];
    //256+80
    ComValforFMK_t defaultinf;
    //256+80+17
    BYTE reserve2[15];                      //保留
    //256+80+15=368
    BYTE asciilen1;
    BYTE inf_mtpmfrinfo[32];
    BYTE asciilen2;
    BYTE inf_mtpprodinfo[32];
    BYTE asciilen3;
    BYTE inf_mtpprodver[16];
    BYTE asciilen4;
    BYTE inf_mtpprodsn[16];
    WORD inf_mtpvid[2];
    WORD inf_mtppid[2];

    BYTE reserve3[38];                      //保留
    //510
    WORD headchecksum;                      //前510byte校验和
    LDIR_t diritem[240];                    //逻辑目录项
} LFIHead_t;       //8k

typedef struct
{
    BYTE jump[4];               //第一条语句
    BYTE ADFUSFlag[4];            //={'A','D','F','U'};
    BYTE type[4];                //"3551"
    WORD version;               //x.x.xx
    BYTE date[4];               //xxxx.xx.xx
    BYTE reserve[2];
    BYTE scode[0x2400 - 20];
} ADFUS_t;

typedef struct
{
    BYTE jump[4];               //第一条语句
    BYTE BRECFlag[4];            //={'B','R','E','C'};
    BYTE type[4];                //"F644"/"F641"/"F321"/"O---"
    WORD version;               //x.x.xx
    BYTE date[4];               //xxxx.xx.xx
    BYTE reserve[2];
    BYTE scode[0x4000 - 20 - 32];
    BYTE BrecInfo[30];
    WORD CheckSum;
} BREC_t;

typedef struct
{
    BYTE jump[4];               //第一条语句
    BYTE FWSCFlag[4];            //={'F','W','S','C'};
    BYTE type[4];                //"F644"/"F641"/"F321"/"O---"
    WORD version;               //x.x.xx
    BYTE date[4];               //xxxx.xx.xx
    BYTE reserve[2];
    BYTE scode[0x3000 - 20];
} FWSCTask_t;

typedef struct
{
    BYTE jump[4];               //第一条语句
    BYTE HWSCFlag[4];            //={'H','W','S','C'};
    BYTE type[4];                //"0000"
    WORD version;               //x.x.xx
    BYTE date[4];               //xxxx.xx.xx
    BYTE reserve[2];
    BYTE scode[0x3000 - 20];
} HWSCTask_t;

typedef struct
{
    char FrameType[2];              //'F','W'
    WORD VID;
    WORD PID;
    long FirmwareVersion;           //韧件版本号
    WORD Fwstatus;
    char Productor[32];             //制造商
    char DeviceName[32];            //设备名
    BYTE FirmwareDate[4];
    WORD BrecCheckSum[2];           //NandFlash物理区中第一份Brec的校验和
    long Logical_cap;               //Flash逻辑总容量

    BYTE dg_flash_info[32];

    BYTE resevred[40];           //防止数据越界溢出
} ADFU_FWScanInfo_t; //160bytes

typedef struct
{
    WORD StgInfor[4];        //Storage 的连接信息
    WORD CExCap[8];          //Cap of storages
} STGInfo_t;

typedef struct
{
    char FrameType[2];      //='H','W'
    WORD ICVersion;         //3935,3951
    BYTE SubVersion[2];     //A,B,C,D
    BYTE BromVersion[4];    //such as "3.0.00.8888"
    BYTE BromDate[4];       //such as "20,03,10,14"
    char BootDiskType[8];   //boot disk的存储类型 "F644"/"F641"/"F321"/"N---"
    STGInfo_t stginfo;

    BYTE flash_ID_table[64];

    WORD NafBscParAddr;

    BYTE resevred[48];
} ADFU_HWScanInfo_t;   //160bytes



typedef struct

{

    BYTE brec_code[4];

    BYTE brecflag[4];

    BYTE brec_size;	// sectors

    BYTE logo_stag; // 1 -- data in brec; 0 -- data out brec

    WORD logo_size; // normally 40kb, 80 sectors

    WORD logo_checksum;

    BYTE sector_cnt;

    BYTE version[2];

    BYTE ReleaseDate[11];

    WORD MinDataNumPerZone2;

    BYTE BootDiskType[8];

    BYTE reserved[55];

    BYTE Snor_info[3];
} ADFU_BrecHead_t; //96bytes



typedef struct
{
    //pc软件通过判断此标志确定是进入修复模式还是进入升级模式
    BYTE SysinfoFlag[8];    //'S','Y','S',' ','I','N','F','O'
    ADFU_HWScanInfo_t adfu_hwscaninfo;
    ADFU_FWScanInfo_t adfu_fwscaninfo;

    ADFU_BrecHead_t adfu_brechead;
} ADFU_SysInfo_t;




#define GetSysInfo(sysinfo) 	API_GetSysInfo(APINO_GetSysInfo,sysinfo)


//获取系统的软硬件信息
bool API_GetSysInfo(uint8 cmd, ADFU_SysInfo_t *sysinfo);

bool ADFUInit(void);
bool ADFURun(void);
bool ADFUExit(void);
void ADFUHandle(void);
BYTE ADFUGetStatus(void);

#endif /* __C251__ */

#ifdef __A251__
mGetSysInfo   macro
ld      hl, APINO_GetSysInfo
rst     RSTBankAPI
endm
#endif /* __A251__ */
#endif /*_ADFU_H */

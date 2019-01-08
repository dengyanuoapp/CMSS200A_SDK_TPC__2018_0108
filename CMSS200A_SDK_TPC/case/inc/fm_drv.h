
#ifndef _Drv_FM_H
#define _Drv_FM_H

#ifdef __C251__

#include "actos.h"
#include "sysdef.h"
#include "stringH.h"
#include "I2C.h"
#include "QN8035.h"

#define  SNR_THRESHOLD      0x06        // 0x09
#define  PILOT_CCA       //disable stereo judgement for the country has many mono FM stations      

/* FM 搜台方向枚举 */
typedef enum
{
    DOWN, //[0]下降
    UP //[1]上升
} FM_direct_e;

/* FM 立体声状态枚举 */
typedef enum
{
    Stereo, //[0]立体声
    Mono //[1]单声道
} FM_Audio_e;

/* FM 声音设置枚举 */
typedef enum
{
    releaseMUTE, //解除静音
    SetMUTE //静音
} FM_MUTE_e;


/* FM 波段模式枚举,  Band_CITY 表示当前播放为城市电台模式 */
typedef enum
{
    Bank_US_Europe,     //普通频段
    Bank_Japan,            //日本频段
    Band_CITY              //城市电台模式
} radio_band_e;

/* 电台状态结构 */
typedef struct
{
    uint16 freq;  //当前电台的频率。单位为khz
    uint8 station; //当前电台对应的电台表项号，from 1 to station_count，当前电台未保存时为0
    uint8 station_count; //电台表中的保存电台数。
    FM_Audio_e stereo_status;    //当前电台的立体声状态
} fm_status_t;


// 常驻内存程序
/* I2C.c */
uint8 I2C_Trans_Bytes(uint8 *buf, uint8 length);
uint8 I2C_Recev_Bytes(uint8 *buf, uint8 Regaddress, uint8 length);
/* fmdrv_rcode.c */
uint8 QNF_SetCh(uint16 freq);
uint8 QNF_SetRegBit(uint8 RegAddr, uint8 bitMask, uint8 data_val);
void QNF_ConfigScan(uint16 start, uint16 stop, uint8 step);
uint8 QND_RXValidCH(uint16 freq, uint8 step);
uint8 QND_WriteReg(uint8 RegAddr, uint8 Data);
uint8 QND_ReadReg(uint8 RegAddr);
uint16 ChanToFreq(uint16 chan);
uint16 FreqToChan(uint16 freq);

//BANK程序段
/* fmdrv_init.c */
uint8 QN_ChipInitialization(void);
void QND_RXSetTH(uint8 db);
uint8 FMdrv_setthrod(uint8 level);
uint8 FMdrv_Init(uint8 mode);
uint8 FMdrv_hardware(uint8 onoff, uint8 PA_mode);
uint8 FMdrv_Exit(uint8 mode);
uint8 FMdrv_Standby(void);
/* fmdrv_bank.c */
void QNF_RXInit(void);
void QND_TuneToCH(uint16 ch);
uint8 FMdrv_setfreq(uint16 Freq);
uint8 FMdrv_getfreq(uint16 *pfreq);
uint8 FMdrv_search(uint16 Freq, uint8 SeekDirect);
uint8 FMdrv_setband(radio_band_e band);
uint8 QNF_GetSM(void);
uint8 FMdrv_seek(uint16 startfreq, uint16 endfreq, uint8 step);
uint8 FMdrv_breakseek(void);
uint8 FMdrv_getstatus(fm_status_t *pBuf);
uint8 FMdrv_getseekflag(uint8 *flag);
uint8 FMdrv_getintensity(uint8 *pvalue);
uint8 FMdrv_mute(FM_MUTE_e mode);

// 驱动全局变量声明
extern uint8 qnd_R16;
extern uint8 qnd_R17;
extern uint8 qnd_R46;

extern uint8 FM_Slevel;	//搜台门限

extern uint8 hardseek_flag;


#endif //__C251__

#endif /* _Drv_FM_H */


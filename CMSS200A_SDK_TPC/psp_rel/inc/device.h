#ifndef _DEVICE_H
#define _DEVICE_H

#include "comval.h"
#include "actapi.h"


#define APINO_GetSDCap            0x0000+API_DEVICE_Base
#define APINO_GetVMCap            0x0001+API_DEVICE_Base
#define APINO_GetUICap            0x0002+API_DEVICE_Base
#define APINO_GetExtRtcFlag       0x0003+API_DEVICE_Base
#define APINO_GetUsbCableStatus   0x0004+API_DEVICE_Base
#define APINO_GetUsbAttribute     0x0005+API_DEVICE_Base
#define APINO_GetUsbSetupInfo     0x0006+API_DEVICE_Base
#define APINO_GetUsbVidPid        0x0007+API_DEVICE_Base
#define APINO_GetHideDiskCap      0x0008+API_DEVICE_Base
#define APINO_GetAutoRunDiskCap   0x0009+API_DEVICE_Base
#define APINO_GetBattery          0x000a+API_DEVICE_Base
#define APINO_InitDSPCode         0x000b+API_DEVICE_Base
#define APINO_StartDsp            0x000c+API_DEVICE_Base
#define APINO_StopDsp             0x000d+API_DEVICE_Base
#define APINO_SetDspMips          0x000e+API_DEVICE_Base
#define APINO_ADC_Enable          0x000f+API_DEVICE_Base
#define APINO_ADC_Disable         0x0010+API_DEVICE_Base
#define APINO_DAC_Enable          0x0011+API_DEVICE_Base
#define APINO_DAC_Disable         0x0012+API_DEVICE_Base
#define APINO_DAC_SetRate         0x0013+API_DEVICE_Base
#define APINO_DAC_GetRate         0x0014+API_DEVICE_Base
#define APINO_PA_Enable           0x0015+API_DEVICE_Base
#define APINO_PA_Disable          0x0016+API_DEVICE_Base
#define APINO_PA_SetVolume        0x0017+API_DEVICE_Base
#define APINO_PA_GetVolume        0x0018+API_DEVICE_Base
#define APINO_PADDV_Enable        0x0019+API_DEVICE_Base
#define APINO_PADDV_Disable       0x001a+API_DEVICE_Base
#define APINO_AI_Enable           0x001b+API_DEVICE_Base
#define APINO_AI_Disable          0x001c+API_DEVICE_Base
#define APINO_AI_SetGain          0x001d+API_DEVICE_Base
#define APINO_AI_GetGain          0x001e+API_DEVICE_Base
#define APINO_GetFwVersion        0x001f+API_DEVICE_Base
#define APINO_GetDefaultComval    0x0020+API_DEVICE_Base
#define APINO_GetMSCVolumeLabel   0x0021+API_DEVICE_Base
#define APINO_UH_POWEROFF         0x0022+API_DEVICE_Base
#define APINO_USBH_CON_ENABLE     0x0023+API_DEVICE_Base
#define APINO_CARD_SET     		  0x0024+API_DEVICE_Base
#define APINO_SETPLL     		  0x0025+API_DEVICE_Base
#define APINO_SETVDD     		  0x0026+API_DEVICE_Base
#define APINO_SPINOR_FW_UPGRD     0x0027+API_DEVICE_Base
#define APINO_SETSPEEDMODE        0x0028+API_DEVICE_Base
#define APINO_DRV_CheckCard       0x0029+API_DEVICE_Base
#define APINO_USBH_CHECK_UDISK    0x002a+API_DEVICE_Base
#define APINO_GETPACKAGE    	  0x002b+API_DEVICE_Base

#define APINO_USB_Detect_Ctrl   0x0000+RAMAPI_DEVICE
#define APINO_Card_Detect_Ctrl  0x0001+RAMAPI_DEVICE
#define APINO_PA_SetVolume_LR   0x0002+RAMAPI_DEVICE

#define CARD_INIT   0
#define CARD_DEC    1
#define CARD_UNDEC  2

#define FS_8K           8       //8k sample rate
#define FS_11K025       11      //11.025k sample rate
#define FS_12K          12      //12k sample rate
#define FS_16K          16      //16k sample rate
#define FS_22K05        22      //22.05k  sample rate
#define FS_24K          24      //24k sample rate
#define FS_32K          32      //32k sample rate
#define FS_44K1         44      //44.1k sample rate
#define FS_48K          48      //48k sample rate
#define FS_96K			96
#define FS_88K2			88

#define  AD_MICIN   0
#define  AD_FMIN    1
#define  AD_RESERVE 2
#define  AD_MIX		3

#define ADC_Stereo 		 	0x0C
#define ADC_LeftChanel   	0x08
#define ADC_RightChanel  	0x04

#define ADC_LFPGain_Neg6db 	0x00
#define ADC_LFPGain_Neg3db 	0x20
#define ADC_LFPGain_0db    	0x40
#define ADC_LFPGain_3db	   	0x60

#define ADC_FIFOMCU		 0x00
#define ADC_FIFODMA		 0x10
#define ADC_FIFOAHC		 0x20

#define ADC_NOTRIM		 0x00
#define ADC_TRIM		 0x01

#define ADC_16BIT		 0x00
#define ADC_21BIT        0x80

#define ADC_FIFOLEVEL_1416 0x00
#define ADC_FIFOLEVEL_1016 0x01
#define ADC_FIFOLEVEL_0616 0x02
#define ADC_FIFOLEVEL_0216 0x03


#define ADC_DGain_00db    	0x00
#define ADC_DGain_06db		0x40
#define ADC_DGain_12db		0x80

#define CLD_Gain_06db      0x00
#define CLD_Gain_12db      0x40
#define CLD_Gain_18db      0x80
#define CLD_Gain_24db      0xc0

#ifdef __C251__
#define Get_USBStatus(tmpval,retval){tmpval=SFR_BANK;SFR_BANK = BANK_USB;retval=DPDMCTRL & 0x40; SFR_BANK=BANK_PMU; retval|=(SYSTEM_VOL & 0x80);SFR_BANK=tmpval;}
/*
********************************************************************************
* Description : usb
********************************************************************************
*/
typedef struct
{
    uint16 VID;            //default：0x10d6
    uint16 PID;            //default：0x1000
} vidpid_t;
typedef struct
{
    int8 usbvendor[8];                //default："Generic"
    int8 usbproductidentification[16];    //default："USB DISK"，不足补0x20
    int8 usbproductversion[4];        //default：" 1.00"
    int8 reserve[4];
} usbattri_t;
typedef struct
{
    int8 descriptorlen;             //=sizeof(usbsetupinfo_t);      //该结构长度
    int8 descriptortype;            //default：0x03
    uint16 unicodestring[23];        //default："USB Mass Storage Class"。unicode格式，不足补0x20,0x00
    int8 reserve[16];
} usbsetupinfo_t;

typedef struct
{
    uint32 version;              //3.1.23.4567   => 0x31,0x23,0x45,0x67
    uint32 date;                 //2004/03/11    => 0x20,0x04,0x03,0x11
} fwversion_t;




//获取U盘是否插线  bi7:DC5V状态,1:DC5V为高,0：DC5V为低;bi6:检测USB是否连接,1:是,0:否.
#define GetUsbCableStatus() 				API_GetUsbCableStatus(APINO_GetUsbCableStatus)
uint8 API_GetUsbCableStatus(uint8 cmd);

//获取U盘属性，如厂商信息，产口信息，产口版本
#define GetUsbAttribute(usbattri) 			API_GetUsbAttribute(APINO_GetUsbAttribute,usbattri)
bool API_GetUsbAttribute(uint8 cmd, usbattri_t *usbattri);

//获取U盘安装显示信息
#define GetUsbSetupInfo(udisk_serial_num) 	API_GetUsbSetupInfo(APINO_GetUsbSetupInfo,udisk_serial_num)
bool API_GetUsbSetupInfo(uint8 cmd, int8 *udisk_serial_num);

//获取U盘VID&PID
#define GetUsbVidPid(vidpid) 				API_GetUsbVidPid(APINO_GetUsbVidPid,vidpid)
bool API_GetUsbVidPid(uint8 cmd, vidpid_t *vidpid);

//获取韧件版本号
#define GetFwVersion(fwver) 		API_GetFwVersion(APINO_GetFwVersion,fwver)
bool API_GetFwVersion(uint8 cmd, fwversion_t *fwver);

#define GetDefaultComval(dcomval) 	API_GetDefaultComval(APINO_GetDefaultComval,dcomval)
bool API_GetDefaultComval(uint8 cmd, comval_t *dcomval);
bool GetFMTag(int8 *FmTagAddr);

//获取U盘卷标
bool GetMSCVolumeLabel(int8 *msc_volume_label);

/*
********************************************************************************
* Description : mtp
********************************************************************************
*/
//--------------------------------------------------------------------
typedef struct
{
    uint8    InfoUniCharNum;         //Unicode字符数，最大32
    uint16    InfoData[33];            //Unicode
} mtpinfo_t;



bool GetMtpMfrInfo(mtpinfo_t *mtpinfo);

bool GetMtpProdInfo(mtpinfo_t *mtpinfo);

bool GetMtpProdVer(mtpinfo_t *mtpinfo);
bool GetMtpProdSn(mtpinfo_t *mtpinfo);
bool GetMtpVidPid(vidpid_t *vidpid);
void GetAudibleDeviceID(int8 *id);

#define GetHideDiskCap() 			API_GetHideDiskCap(APINO_GetHideDiskCap)
#define GetAutoRunDiskCap() 		API_GetAutoRunDiskCap(APINO_GetAutoRunDiskCap)
int32 API_GetHideDiskCap(uint8 cmd);
int32 API_GetAutoRunDiskCap(uint8 cmd);

/*
********************************************************************************
*       JudgeUdiskOperation                       2010-12-31 17:25
*
* Description :record operations of Udisk mode, such as read, write, format and so on.
*
* Arguments   :GetorSet ---- 0, get operation status
*                            1, clear opertion status.
*
* Returns     :0    ---- read only.
*              else ---- bit0 == 1, UDisk write.
*                        bit4 == 1, MTP write.
*
* Notes       :
*
* By          : wanghaijing         2010-12-31 17:25       record UDisk operations
********************************************************************************
*/
char JudgeUdiskOperation(char GetorSet);

void USB_Default_isr_Handle(void);
#define  USB_CON_ENABLE() API_USB_CON_ENABLE(APINO_USBH_CON_ENABLE)
uint8 API_USB_CON_ENABLE(uint8 cmd);
void UH_Poweroff(void);
#define USBH_Check_Udisk() 	API_USBH_Check_Udisk(APINO_USBH_CHECK_UDISK)
bool API_USBH_Check_Udisk(uint8 cmd);     //返回1表示当前有U盘插入，返回0表示当前没有U盘插入

#define GetSDCap() 			API_GetSDCap(APINO_GetSDCap)
#define GetVMCap() 			API_GetVMCap(APINO_GetVMCap)
#define GetUICap() 			API_GetUICap(APINO_GetUICap)
#define GetExtRtcFlag() 	API_GetExtRtcFlag(APINO_GetExtRtcFlag)
//#define GetLCDWidth() 		API_GetLCDWidth(APINO_GetLCDWidth)
//#define GetLCDHeight() 		API_GetLCDHeight(APINO_GetLCDHeight)

//获取系统区的容量，以扇区为单位
int32 API_GetSDCap(uint8 cmd);
//获取Vram区的容量，以扇区为单位
int32 API_GetVMCap(uint8 cmd);
//获取Mtp区的容量，以扇区为单位
// int32 GetMICap(void);
//获取USB 信息区的容量，以扇区为单位
int32 API_GetUICap(uint8 cmd);
//获取是否支持外部RTC的标志
int8 API_GetExtRtcFlag(uint8 cmd);
//获取LCD的宽度
// int16 API_GetLCDWidth(uint8 cmd);
//获取LCD的高度
// int16 API_GetLCDHeight(uint8 cmd);

//16进制显示uart整数打印函数 num:待打印整数
// void UartPutHexNum(uint16 num);

//10进制显示uart整数打印函数 num:待打印整数
// void UartPutDecNum(uint16 num);

//uart串口类printf函数
// void UartPrintf (const int8 *fmt, ...);
//--------------------------------------------------------------------



/*
********************************************************************************
* Description : battery
********************************************************************************
*/
//获取当前的电量
//para: none
//ret:0 ~ 15 电量值
#define GetBattery()	API_GetBattery(APINO_GetBattery)
uint8 API_GetBattery(uint8 cmd);

/*
********************************************************************************
* Description : dsp
********************************************************************************
*/
//初始化dsp代码或数据
//para: dspcodename 代码文件名，
//      mode        =0 dsp code
//                  =1 table
//ret:  true/false
bool DSP_InitCode(const int8 *dspcodename, int8 mode);

//初始化dsp代码或数据
//para: dspcodename 代码文件名
//      mod
//          x  x  x  x  x  x  x  x
//          |_____|  |___________|
//                |              |___bin type
//                |__________________codec type
//ret:  true/false
bool DSP_InitCodeExp(int8 *DSPFileName, uint8 songtype, uint8 audiotype);
//初始化dsp
//para: mips: 00010:24mips 00011:36mips 00100:48mips 00101:60mips 00110:72mips 00111:84mips
//01000:98m......11111:372m
//ret:  true/false
bool StartDsp(uint8 mips);
//stop dsp
//para: void
//ret:  true/false
bool StopDsp(void);
bool StopDspInUSB(void);
//设置dsp速度
//para: mips: 00:12mips 01:24mips 02:36mips 03:48mips 04:60mips 05:80mips
//ret:  true/false
bool SetDspMips(uint8 mips);

//获取wma的table
//para: uint8 table no
//ret:  true/false
bool SetDspMips(uint8 tableno);




/*
********************************************************************************
* Description : analog
********************************************************************************
*/
typedef unsigned char rate_t;
typedef unsigned char adin_t;  //ad  input
#define  AD_AuxBT   0
#define  AD_AuxMIC  1
#define  AD_AuxFM   2
#define  AD_BTMIC		3
#define  AD_BTFM	  4
#define  AD_FMMIC		5
#define  AD_MIC		  6
#define  AD_FM	    7
#define  AD_Aux			8
#define  AD_BT		  9
typedef unsigned char ain_t;
#define AI_MICGAIN 0
#define AI_FMGAIN  1
#define AI_AUXGAIN  2
#define AI_BTGAIN  3

typedef unsigned char  volume_t;
#define DA_VolumeL   0
#define DA_VolumeR  1

/*
typedef struct
{
        int8 micin:1;    //此位为1,ai mic select
        int8 linein:1;   //此位为1,fm输入被选择 only support by 51
        int8 fmin:1;     //此位为1,linein输入被选择 only support by 51
		int8 micin20:1;  //reserve
}ain_t;
*/

typedef struct
{
    adin_t adin;    		//adc 输入选择
    uint8 channelmode;      //0ch: Stereo ; 08h: only left ; 04h: only Right
    rate_t rate;    		//采样率选择 ADC Frequency Sampling:8/16/24/32/48 etc
    uint8 inputgain;        //mic: 0-7 for 20-40db, fm: 0-7 for -3.0 - +7.5db, input source gain，mic录音推荐值5即放大50倍，fm录音推荐值2即不放大也不缩小
    uint8 lfpgain;			//0: -6 db, 20h: -3db, 40h: 0db, 60h: +3db, adc analog gain，推荐值40h即不放大也不缩小
    uint8 fifoaccess;	   //  ; 0: MCU, 10h: DMA, 20h: AHC, 30h: AHC
    uint8 trimmingflag;	  //  ; 0: no trimming, 01h: trimming
    uint8 FSBit;		   // 0: need mix    , 80h: not mix
    uint8 fifolevel;	   //0: 14 of 16,01h: 10 of 16, 02h: 6 of 16, 03h: 2 of 16
    uint8 digitalgain;	  //0: 0 db, 40h: +6db, 80h: +12db, c0h: reserve, adc digital gain
} adc_t;

//普通用户只需要配置前面5个，高级用户可配置后面5个。为方便用户使用，请补充其他参数的宏描述



//***********************************************************
typedef struct
{
    int8 dacselect; //0: internal dac 1: external dac       //2003-12-5 10:50
    rate_t rate;    //dac 采样率选择
    int8 reserve[2];//保留
} dac_t;

//***********************************************************
//      power amplifier input 的模式
//       对于gl3935  输只支持dacin : 也就是dac的输入
typedef struct
{
    int8 micin: 1;  //此位为1,micin输入被选择 only support by 51
    int8 fmin: 1;   //此位为1,fm输入被选择 only support by 51
    int8 linein: 1; //此位为1,linein输入被选择 only support by 51
    int8 dacin: 1;  //此位为1,dac in select
} pain_t;


typedef struct
{
    pain_t pa_in;  //输入选择
    int8 volume;    //音量选择
    int8 reserve[2];//保留
} pa_t;

//激活Analog analog
#define EnableAIN(ain) API_EnableAIN(APINO_AI_Enable,ain)
bool API_EnableAIN(uint8 cmd, adin_t ain);
//关闭Analog analog
#define DisableAIN() API_DisableAIN(APINO_AI_Disable)
void API_DisableAIN(uint8 cmd);
//设置Analog analog
#define SetAINGain(gain0,gain1) API_SetAINGain(APINO_AI_SetGain,gain0,gain1)
bool API_SetAINGain(uint8 cmd, uint8 gain0, uint8 gain1);
//设置Analog analog
#define GetAINGain(ain) API_GetAINGain(APINO_AI_GetGain,ain)
uint8 API_GetAINGain(uint8 cmd, ain_t ain);

//激活Audio ADC
#define EnableADC(adcattr) API_EnableADC(APINO_ADC_Enable,adcattr)
bool API_EnableADC(uint8 cmd, adc_t *adcattr);
//关闭Audio ADC
#define DisableADC() API_DisableADC(APINO_ADC_Disable)
void API_DisableADC(uint8 cmd);



//激活内部/外部DAC
#define EnableDAC(dacattr) API_EnableDAC(APINO_DAC_Enable,dacattr)
bool API_EnableDAC(uint8 cmd, dac_t *dacattr);
//关闭内部/外部DAC
#define DisableDAC() API_DisableDAC(APINO_DAC_Disable)
void API_DisableDAC(uint8 cmd);
//关闭内部/外部DAC
#define SetDACRate(rate,dacselec) API_SetDACRate(APINO_DAC_SetRate,rate,dacselec)
bool API_SetDACRate(uint8 cmd, rate_t rate, uint8 dacselec);
//获取dac sample rate
#define GetDACRate() API_GetDACRate(APINO_DAC_GetRate)
rate_t API_GetDACRate(uint8 cmd);

//激活内部pa
#define EnablePA(paattr) API_EnablePA(APINO_PA_Enable,paattr)
bool API_EnablePA(uint8 cmd, pa_t *paattr);
//关闭内部pa
#define DisablePA() API_DisablePA(APINO_PA_Disable)
void API_DisablePA(uint8 cmd);
//设置pa音量
#define SetPAVolume(volume) API_SetPAVolume(APINO_PA_SetVolume,volume)
bool API_SetPAVolume(uint8 cmd, uint8 volume);
//获取pa音量
#define GetPAVolume(volume) API_GetPAVolume(APINO_PA_GetVolume,volume)
uint8 API_GetPAVolume(uint8 cmd, volume_t volume);
//设置pa音量(区分左右声道使用)
#define SetPAVolume_LR(volumel,volumer) API_SetPAVolume_LR(APINO_PA_SetVolume_LR,volumel,volumer)
bool API_SetPAVolume_LR(uint8 cmd, uint8 volumel, uint8 volumer);
//激活直驱pa
#define EnablePADDV(paattr) API_EnablePADDV(APINO_PADDV_Enable,paattr)
bool API_EnablePADDV(uint8 cmd, pa_t *paattr);
//关闭直驱pa
#define DisablePADDV() API_DisablePADDV(APINO_PADDV_Disable)
void API_DisablePADDV(uint8 cmd);

//#define EnableCLD() API_EnableCLD(APINO_CLD_Enable)
// void API_EnableCLD(uint8 cmd);

//#define DisableCLD() API_DisableCLD(APINO_CLD_Disable)
// void API_DisableCLD(uint8 cmd);

//设置 CLD gain
//#define SetCLDGain(gain) API_SetCLDGain(APINO_CLD_SetGain,gain)
// void API_SetCLDGain(uint8 cmd, uint8 gain);
//获取 CLD gain
//#define GetCLDGain() API_GetCLDGain(APINO_CLD_GetGain)
// uint8 API_GetCLDGain(uint8 cmd);
// SetPLL
#define SetPLL(pllsel) 		API_SetPLL(APINO_SETPLL,pllsel)
void API_SetPLL(uint8 cmd, uint8 pllsel);
// SETVDD
#define SetVDD(vddsel) 		API_SetVDD(APINO_SETVDD,vddsel)
void API_SetVDD(uint8 cmd, uint8 vddsel);

#define SpiNor_FW_Upgrd() 	API_SpiNor_FW_Upgrd(APINO_SPINOR_FW_UPGRD)
void API_SpiNor_FW_Upgrd(uint8 cmd);

#define CardInOut_MSG()		API_CardInOut_MSG(APINO_CardInOut_MSG)
void API_CardInOut_MSG(uint8 cmd);

#define CardSetParam(clk_bus_width,timer,mode)   API_CardSetParam(APINO_CARD_SET,clk_bus_width,timer,mode)
uint8 API_CardSetParam(uint8 cmd, uint8 clk_bus_width, uint8 timer, uint8 mode);

#define DRV_SetSpeedMode(speedmode) API_DRV_SetSpeedMode(APINO_SETSPEEDMODE,speedmode)
void API_DRV_SetSpeedMode(uint8 cmd, uint8 speedmode);

#define GetPackage() 	API_GetPackage(APINO_GETPACKAGE)
uint8 API_GetPackage(uint8 cmd);

#define USB_Detect_Ctrl(function) API_USB_Detect_Ctrl(APINO_USB_Detect_Ctrl,function)
void API_USB_Detect_Ctrl(uint8 cmd, uint8 function);

#define Card_Detect_Ctrl(clk_bus_width, timer, mode) API_Card_Detect_Ctrl(APINO_Card_Detect_Ctrl,clk_bus_width, timer, mode)
void API_Card_Detect_Ctrl(uint8 cmd, uint8 clk_bus_width, uint8 timer, uint8 mode);

#endif /* __C251__ */

#ifdef __A251__
mGetUsbCableStatus   macro                      //获取U盘是否插线  0:没插线   其他:插线
MOV  R11, #APINO_GetUsbCableStatus
ECALL RSTSYSAPI
endm
mGetUsbAttribute   macro                        //获取U盘属性，如厂商信息，产口信息，产口版本
MOV  R11, #APINO_GetUsbAttribute
ECALL RSTSYSAPI
endm
mGetUsbSetupInfo   macro                        //获取U盘安装显示信息
MOV  R11, #APINO_GetUsbSetupInfo
ECALL RSTSYSAPI
endm
mGetUsbVidPid   macro                           //获取U盘VID&PID
MOV  R11, #APINO_GetUsbVidPid
ECALL RSTSYSAPI
endm
mGetBattery     macro
ld      a, APINO_GetBattery
ECALL RSTSYSAPI
endm
mInitDSPCode    macro
MOV  R11, #APINO_InitDSPCode
ECALL RSTSYSAPI
endm
mInitDSPCodeExp macro
MOV  R11, #APINO_InitDSPCodeExp
ECALL RSTSYSAPI
endm

//mInitDSPCodeExp2 macro
//        MOV  WR8, #API_InitDSPCodeExp2
//        ECALL RSTBankAPI
//        endm
mStartDsp       macro
MOV  R11, #APINO_StartDsp
ECALL RSTSYSAPI
endm
mStopDsp        macro
MOV  R11, #APINO_StopDsp
ECALL RSTSYSAPI
endm
mStopDspInUSB   macro
MOV  R11, #APINO_StopDsp
ECALL RSTSYSAPI
endm
mSetDspMips     macro
MOV  R11, #APINO_SetDspMips
ECALL RSTSYSAPI
endm
mGetWmaTable    macro
MOV  R11, #APINO_GetWmaTable
ECALL RSTSYSAPI
endm
mGetOggTable    macro
MOV  R11, #APINO_GetOggTable
ECALL RSTSYSAPI
endm


mEnableAIn      macro
MOV  R11, #APINO_AI_Enable
ECALL RSTSYSAPI
endm
mDisableAIn     macro
MOV  R11, #APINO_AI_Disable
ECALL RSTSYSAPI
endm
mSetAGain       macro
MOV  R11, #APINO_AI_SetGain
ECALL RSTSYSAPI
endm
mGetAGain       macro
MOV  R11, #APINO_AI_GetGain
ECALL RSTSYSAPI
endm
mADC_Enable     macro
MOV  R11, #APINO_ADC_Enable
ECALL RSTSYSAPI
endm
mADC_Disable    macro
MOV  R11, #APINO_ADC_Disable
ECALL RSTSYSAPI
endm

mDAC_Enable    macro
MOV  R11, #APINO_DAC_Enable
ECALL RSTSYSAPI
endm

mDAC_Disable    macro
MOV  R11, #APINO_DAC_Disable
ECALL RSTSYSAPI
endm
mDAC_SetRate    macro
MOV  R11, #APINO_DAC_SetRate
ECALL RSTSYSAPI
endm
mDAC_GetRate    macro
MOV  R11, #APINO_DAC_GetRate
ECALL RSTSYSAPI
endm
mPA_Enable      macro
MOV  R11, #APINO_PA_Enable
ECALL RSTSYSAPI
endm
mPA_Disable     macro
MOV  R11, #APINO_PA_Disable
ECALL RSTSYSAPI
endm
mPA_SetVolume   macro
MOV  R11, #APINO_PA_SetVolume
ECALL RSTSYSAPI
endm
mPA_GetVolume   macro
MOV  R11, #APINO_PA_GetVolume
ECALL RSTSYSAPI
endm

//mtp
/*
mGetMtpVidPid   macro                      //获取U盘是否插线  0:没插线   其他:插线
        MOV  WR8, #API_GetMtpVidPid
        LCALL RSTBankAPI
        endm
mGetMtpMfrInfo   macro                        //获取U盘属性，如厂商信息，产口信息，产口版本
        MOV  WR8, #API_GetMtpMfrInfo
        LCALL RSTBankAPI
        endm

mGetMtpProdInfo   macro                           //获取U盘VID&PID
        MOV  WR8, #API_GetMtpProdInfo
        LCALL RSTBankAPI
        endm

mGetMtpProdVer   macro                        //获取U盘安装显示信息
        MOV  WR8, #API_GetMtpProdVer
        LCALL RSTBankAPI
        endm
mGetMtpProdSn  macro                           //获取U盘VID&PID
        MOV  WR8, #API_GetMtpProdSn
        LCALL RSTBankAPI
        endm
*/
mGetAudibleDeviceID  macro                      //获取设备ID
MOV  R11, #APINO_GetAudibleDeviceID
ECALL RSTSYSAPI
endm

mGetHideDiskCap  macro                          //获取隐藏盘的容量
MOV  R11, #APINO_GetHideDiskCap
ECALL RSTSYSAPI
endm

mGetAutoRunDiskCap  macro                       //获取AutoRun光盘的容量
MOV  R11, #APINO_GetAutoRunDiskCap
ECALL RSTSYSAPI
endm

mGetSDCap       macro                          //获取系统区的容量
MOV  R11, #APINO_GetSDCap
ECALL RSTSYSAPI
endm
mGetVMCap       macro                          //获取Vram区的容量
MOV  R11, #APINO_GetVMCap
ECALL RSTSYSAPI
endm
/*
mGetMICap       macro                          //获取Mtp区的容量
        MOV  WR8, #API_GetMICap
        LCALL RSTBankAPI
        endm	*/
mGetUICap       macro                          //获取USB 信息区的容量
MOV  R11, #APINO_GetUICap
ECALL RSTSYSAPI
endm
mGetExtRtcFlag  macro                          //获取是否支持外部RTC的标志
MOV  R11, #APINO_GetExtRtcFlag
ECALL RSTSYSAPI
endm
mGetLCDWidth  macro							  //获取LCD的长度
MOV  R11, #APINO_GetLCDWidth
ECALL RSTSYSAPI
endm
mGetLCDHeight  macro                          //获取LCD的高度
MOV  R11, #APINO_GetLCDHeight
ECALL RSTSYSAPI
endm

mUH_Poweroff macro
MOV  R11, #APINO_UH_POWEROFF
ECALL RSTSYSAPI
endm

mUSB_ISR_DEFAULT macro
MOV  R11, #APINO_USB_ISR_DEFAULT
ECALL RSTSYSAPI
endm

mUSB_CON_ENABLE macro
MOV  R11, #APINO_USBH_CON_ENABLE
ECALL RSTSYSAPI
endm

mUSBH_Check_Udisk macro
MOV  R11, #APINO_USBH_CHECK_UDISK
ECALL RSTSYSAPI
endm

mCOM_ISR_DEFAULT   macro
MOV  R11, #APINO_COM_ISR_DEFAULT
ECALL RSTSYSAPI
endm

mGetMSCVolumeLabel  macro                       //获取U盘的卷标
MOV  R11, #APINO_GetMSCVolumeLabel
ECALL RSTSYSAPI
endm

mDRV_SetSpeedMode  macro
MOV  R11, #APINO_SETSPEEDMODE
ECALL RSTSYSAPI
endm


#endif /* __A251__ */


#endif
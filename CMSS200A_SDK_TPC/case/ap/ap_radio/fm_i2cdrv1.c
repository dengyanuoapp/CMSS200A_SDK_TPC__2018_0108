/********************************************************************************************
 Airoha Technology FM module driver(I2C DRIVER)  for ATJ2097
 last update 2007/11/02
 Version:V0.5
 ********************************************************************************************/
#include "ap_radio.h"

#define     PA_CTL_PORTNUM   0x05

#pragma name(FM_CTRL_D)

uint8 WriteBuffer[64] =
    //{ 0xc0, 0x01, 0x00, 0x00, 0x04, 0x00, 0x80, 0xaf, 0x00, 0x00, 0x46, 0xc6, 0x50, 0x96, 0x00, 0x00, 0x40, 0x00, 0x00,
{
    0xc0, 0x01, 0x00, 0x00, 0x00, 0x10, 0x80, 0xaf, 0x00, 0x00, 0x46, 0xc6, 0x50, 0x96, 0x00, 0x00, 0x40, 0x00,
    0x00, 0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x05, 0x90, 0x00, 0xf4, 0x84, 0x70,
    0x01, 0x40, 0xf0, 0x21, 0x80, 0x7a, 0xd0, 0x3e, 0x40, 0x55, 0xa9, 0xe8, 0x48, 0x50, 0x80, 0x00, 0x00,
    0x37, 0xb6, 0x40, 0x0c, 0x07, 0x9b, 0x4c, 0x1d, 0x81, 0x11, 0x45, 0x80
};
uint8 ReadBuffer[4] =
{ 0, 0, 0, 0 };
uint8 Set_Mute = 0x40; //FM mute, 02h bit 14(0 = mute 1 normal)
//uint8 Set_SearchMode = 0x01; //search mode(search or not),02h bit 8
//uint8 Set_Mono = 0x00; //Mono or Stereo, 02h bit 13(0= stereo)
//uint8 Set_Standby = 0x01; //power disable or enable, 02h bit 0(0 disable,1 enable)
uint8 Set_Xtal = 0x00; //0x50    //clk_mode,02h bit[6:4] 0x00 = 32.768 0xD0,0x50= 24M
//uint8 Set_SeekStep = 0x00; //03H[1:0] seek step such as :100K, 50K and so on
//uint8 Set_DTC = 0x00;         //de-emphasis time constant,04h bit 11(0 = 75us,1 = 50us)
uint8 Set_Band = 0x00; //Band,03h bit[3:2](00 = US,01 = Japan)
uint8 Set_SeekEnable = 0x00; //whether stop seeking at the upper or lower band limit,02h bit 7(1 stop)
//uint8 Set_RST = 0x00;       //soft reset   0x02 = reset ,0x00 = not reset (now not used)
uint8 Set_SeekDirect = 0x00; //Seek up or down,02h bit 9(0 = down,1 = up,now no use just set to 1)
uint8 Set_SeekLevel = 0x88; //seek level,05h bit[14:8]
uint16 Freq_RF = 0x00; //Freq infor,3h bit[15:6],freq = FREQ_RF*channel sapcing + 87M(76M)
//uint8 StationFlag = 0x00;	   //the current channel is a station,0Bh bit 8(1 = station)
uint8 StereoFlag = 0x00; //Stereo indication,0AH bit 10(0 = mono,1 = stereo)
uint8 StationLevel = 0x00; //current station's level,0bh bit[15:9]


uint8 Is_Stere0 = 0;
uint8 EnterFlag = 0;
//extern uint8 earphoneinflag;
extern void I2C_Init(void);
extern void I2C_Exit(void);
extern void FM_Wait50ms(void);
extern uint8 WriteOneFrame(uint8 count);
extern uint8 ReadOneFrame(void);
extern uint8 ReadStatus(void);
extern void SetMfpToNand(void);
uint8 FM_Standby_Drv(uint8 StandbyFlag);
void Assemble_WriteBuffer(void);
void DisAssemble_WriteBuffer(void);
uint8 FM_SetFrequency(uint16 Freq);
uint8 CheckStation(void);
void OpenPA(void);
/*
 ********************************************************************************
 *             uint8 FM_Init(uint8 band,uint8 slevel)
 *
 * Description : FM初始化,设置搜索门限,波段,步进等
 *
 * Arguments   :	  	 波段选择值,门限
 *
 * Returns     :		 若初始化成功,则返回1,否则,返回0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_Init(uint8 band, uint8 slevel)
{
    uint8 result;
    uint8 Analogtempbuf[2] =
    { 0, 0 };
    if (band == 0)
    {
        Set_Band = 0;
    }
    else
    {
        Set_Band = 0x04;
    }
    WriteBuffer[0] &= (uint8) 0xfe; //初始化,disable SEEK

    WriteBuffer[3] &= (uint8) 0xf0;
    WriteBuffer[3] |= Set_Band;

    Set_SeekLevel = slevel; //变量初始化
    WriteBuffer[6] = 0x80;
    WriteBuffer[6] |= Set_SeekLevel;

    //Set_Standby = 1;
    //Set_SearchMode = 0;

    I2C_Init(); //I2C初始化
    result = FM_Standby_Drv(1);
    FM_Wait50ms();
    SetMfpToNand(); //将GPIO切到NAND FLASH

    SetAINGain(0x02, 0x05);
    Analogtempbuf[0] = 0x01;
    EnableAIN(Analogtempbuf[0]);
    if (EnterFlag == 0)
    {
        //       OpenPA();
    }
    return result;
}
/*
 ********************************************************************************
 *             void FM_Exit(void)
 *
 * Description : 退出FM
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void FM_Exit(void)
{
#if 0
    SetPAVolume(0);
    DisableAIN();
    if (EnterFlag == 0)
    {

        DisablePA();

    }
#endif
}
/*
 ********************************************************************************
 *             uint8 FM_SetFreq(uint16 Freq)
 *
 * Description : 设置频点,进行tune操作
 *
 * Arguments   :	  需设置的频点值
 *
 * Returns     :		//设置频点是否成功,若为真台,返回1
 *					       否则,返回0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_SetFreq(uint16 Freq)
{
    uint8 result;
    I2C_Init(); //将GPIO切到I2C
    result = FM_SetFrequency(Freq);
    result = CheckStation();
    if (result != 0)
    {
        Is_Stere0 = 1;
    }
    else
    {
        Is_Stere0 = 0;
    }
    //SetMfpToNand(); //将GPIO切到NAND FLASH
    return result;
}
/*
 ********************************************************************************
 *             uint8 FM_GetStatus(FMStatus_Tab_t * Buf)
 *
 * Description : 获取搜索到的真台的频点和立体声信息
 *
 * Arguments   :	  保存电台信息的结构体指针
 *
 * Returns     :		//是否读取状态成功,如果读取成功,则返回值为1
 *					       否则,返回0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_GetStatus(FMStatus_Tab_t *Buf)
{
    uint8 result;
    uint8 i;
    I2C_Init(); //将GPIO切到I2C
    for (i = 0; i < 2; i++)
    {
        result = ReadStatus();
        if (result != 0)
        {
            break;
        }
    }
    if (result != 0)
    {
        DisAssemble_WriteBuffer();
        if (Is_Stere0 == 1) //获取搜索到的真台的频点和立体声信息
        {
            Buf->FM_Stereo_Status = 0;
        }
        else
        {
            Buf->FM_Stereo_Status = 1;
        }
        Buf->FM_CurrentFreq = Freq_RF;
        SetMfpToNand();
        return 1;
    }
    Buf->FM_Stereo_Status = 1;
    SetMfpToNand();
    return 0;
}
/*
 ********************************************************************************
 *             uint8 FM_Standby(void)
 *
 * Description : FM standby
 *
 * Arguments   :
 *
 * Returns     :		 若设置standby成功,则返回1,否则,返回0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_Standby(void)
{
    uint8 result;
    I2C_Init(); //将GPIO切到I2C
    result = FM_Standby_Drv(0);
    SetMfpToNand(); //将GPIO切到NAND FLASH
    return result;
}
/*
 ********************************************************************************
 *             uint8 FM_Mute(FM_MUTE_t mode)
 *
 * Description : FM静音设置
 *
 * Arguments   :	  是否静音,0为取消静音,1为静音
 *
 * Returns     :		 若设置静音控制成功,则返回1,否则,返回0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_Mute(FM_MUTE_t mode)
{
    uint8 result;
    uint8 sfr_bak;
    uint8 i;
    I2C_Init(); //将GPIO切到I2C
    WriteBuffer[0] &= (uint8) 0xfe; //disable seek ,注意硬件SEEK方式,在设置频点\静音时,必须disable seek!!!!
    if (mode == 0)
    {
        WriteBuffer[0] |= (0x01 << 6);
        sfr_bak = SFR_BANK;
        SFR_BANK = BANK_AUIP;
        PA_CTL |= (0x01 << PA_CTL_PORTNUM);
        SFR_BANK = sfr_bak;
    }
    else
    {
        WriteBuffer[0] &= ~(0x01 << 6);
        sfr_bak = SFR_BANK;
        SFR_BANK = BANK_AUIP;
        PA_CTL &= ~(0x01 << PA_CTL_PORTNUM);
        SFR_BANK = sfr_bak;
    }
    for (i = 0; i < 2; i++)
    {
        result = WriteOneFrame(2);
        if (result != 0)
        {
            break;
        }
    }
    SetMfpToNand(); //将GPIO切到NAND FLASH
    return result;
}
/*
 ********************************************************************************
 *             uint8 FM_Search(fm_play_status_t * Buf,uint16 Freq,uint8 SeekDirect)
 *
 * Description : FM搜台
 *
 * Arguments   :
 param1:  保存电台信息的结构体指针
 param2:  软件搜台方式,需设置的频率值
 param3:  搜索方向
 * Returns     :		如果搜到的电台为真台,则返回1;否则,返回0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_Search(fm_play_status_t *Buf, uint16 Freq, uint8 SeekDirect)
{
    uint8 result, i;
#ifdef  HARDWARE_SEEK
    Set_SeekDirect = SeekDirect << 1; //硬件SEEK方式,需设置SEEK方向
    Freq = Freq; //avoid keil warning
#else
    Freq_RF = Freq; //软件搜台,设置tune频点
    SeekDirect = SeekDirect; //avoid keil warning
#endif
    I2C_Init();
    Assemble_WriteBuffer();
    for (i = 0; i < 2; i++)
    {
#ifdef  HARDWARE_SEEK
        result = WriteOneFrame(2); //硬件SEEK方式,只需写02寄存器,enable seek
#else
        result = WriteOneFrame(8);
        // result=WriteOneFrame(4);
#endif
        if (result != 0)
        {
            break;
        }
    }
    if (result != 0)
    {
        if (!CheckStation())
        {
            Is_Stere0 = 0;
            Buf->stereo_status = 1;
            SetMfpToNand();
            return 0;
        }
        Is_Stere0 = 1;
        Buf->stereo_status = 0;
        /*	 if(StereoFlag==1)
         {
         Buf->stereo_status=0;
         }
         else
         {
         Buf->stereo_status=1;
         } */
        Buf->freq = Freq_RF;
        SetMfpToNand();
        return 1;
    }
    Is_Stere0 = 0;
    Buf->stereo_status = 1;
    SetMfpToNand();
    return 0;
}

/*
 ********************************************************************************
 *             void FM_VolumeSet(uint8 Vol)
 *
 * Description : 设置PA音量
 *
 * Arguments   :	  需设置的音量值
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void FM_VolumeSet(uint8 Vol)
{
    set_dac_volume(Vol);
}
/*
 ********************************************************************************
 *             uint8 CheckStation(void)
 *
 * Description : 真台判断
 *
 * Arguments   :
 *
 * Returns     :	 若搜到的电台为真台,返回1;否则,返回0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 CheckStation(void)
{
    uint8 i = 2;
    uint8 result = 0;
    uint8 temp;
    FM_Wait50ms(); //延时20ms，等待FM Tune或SEEK完成
    do
    {
        i--; //读取两次
        result = ReadStatus();
    }
    while ((result == 0) && (i > 0));
    if (result != 0)
    {
#ifdef HARDWARE_SEEK
        temp = ReadBuffer[0] | 0xdf; //硬件SEEK方式，真台判断处理
        if(temp != 0xff)
        {
            DisAssemble_WriteBuffer();
            return 1;
        }
        else
        {
            return 0;
        }
#else
        temp = ReadBuffer[2] & 0x01; //软件搜台方式，真台判断处理
        if (temp == 0x01)
        {
            DisAssemble_WriteBuffer();
            return 1;
        }
        else
        {
            return 0;
        }
#endif
    }
    return 0;
}

/*
 ********************************************************************************
 *             uint8 FM_SetFrequency(uint16 Freq)
 *
 * Description : 频点设置
 *
 * Arguments   :	需设置的频率值
 *
 * Returns     :	 若频点设置成功，返回1;否则,返回0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_SetFrequency(uint16 Freq)
{
    uint8 i, result;
    WriteBuffer[0] &= (uint8) 0xfe; //禁止SEEK ,注意硬件SEEK方式,在设置频点\静音时,必须disable seek!!!!
    WriteBuffer[1] |= (0x01 << 0); //Enable FM
    WriteBuffer[2] = (uint8) (Freq >> 8);
    WriteBuffer[3] &= 0x3f;
    WriteBuffer[3] |= (uint8) (Freq % 256);
    WriteBuffer[3] |= (0x01 << 4); //enable tune
    WriteBuffer[6] = 0x80;
    WriteBuffer[6] |= Set_SeekLevel;
    for (i = 0; i < 2; i++)
    {
        result = WriteOneFrame(8); //设置频点,写相关寄存器
        if (result != 0)
        {
            return 1;
        }
    }
    return 0;
}

/*
 ********************************************************************************
 *            uint8 FM_Standby_Drv(uint8 StandbyFlag)
 *
 * Description : FM standby或wake up
 *
 * Arguments   :	 standby标志,1表示wake up,0表示standby
 *
 * Returns     :	 若 standby或wake up成功，返回1;否则,返回0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_Standby_Drv(uint8 StandbyFlag)
{
    uint8 result;
    WriteBuffer[1] &= (uint8) 0xfe; //FM disable
    // Set_Standby = StandbyFlag;
    if (StandbyFlag != 0)
    {
        WriteBuffer[1] |= (0x01 << 0); //FM enable
    }
    result = WriteOneFrame(64);
    if (result == 0)
    {
        result = WriteOneFrame(64);
    }
    return result;
}
/*
 ********************************************************************************
 *           void Assemble_WriteBuffer(void)
 *
 * Description : 设置FM的写寄存器,控制搜台
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void Assemble_WriteBuffer(void)
{
#ifdef HARDWARE_SEEK				//硬件SEEK方式
    WriteBuffer[0] |= 0x01; //seek enable
    WriteBuffer[0] &= (uint8)0xfd; //清除上次的seek direct
    WriteBuffer[0] |= Set_SeekDirect; //设置 SEEK Direct
    WriteBuffer[1] |= (0x01 << 0); //FM Enable
    WriteBuffer[1] &= 0x7f; //Seekmode=0

#else							   //软件搜台方式
    WriteBuffer[0] &= (uint8) 0xfe; //禁止SEEK
    WriteBuffer[1] |= (0x01 << 0); //Enable FM
    WriteBuffer[2] = (uint8) (Freq_RF >> 8); //设置搜索频点
    WriteBuffer[3] &= 0x3f;
    WriteBuffer[3] |= (uint8) (Freq_RF % 256);
    WriteBuffer[3] |= (0x01 << 4); //enable tune
    WriteBuffer[6] = 0x80;
    WriteBuffer[6] |= Set_SeekLevel;
#endif
}
/*
 ********************************************************************************
 *           void DisAssemble_WriteBuffer(void)
 *
 * Description : 读FM的寄存器,读取电台信息
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void DisAssemble_WriteBuffer(void)
{
    if ((ReadBuffer[0] & (0x01 << 2)) != 0) //stereo立体声 判断
    {
        StereoFlag = 1;
    }
    else
    {
        StereoFlag = 0;
    }
    Freq_RF = (uint16) (ReadBuffer[0] & 0x03); //读取搜索到的电台频点
    Freq_RF <<= 8;
    Freq_RF += ReadBuffer[1];
}

void OpenPA(void)
{
    pa_t paattr;
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    paattr.pa_in.dacin = 0;
    paattr.pa_in.fmin = 1; //选择PA的输入为FMIN
    paattr.pa_in.linein = 0;
    paattr.pa_in.micin = 0;

    SFR_BANK = 0x01; //使能FMIN到PA的通路
    MRCR2 |= 0x30;
    SFR_BANK = BANK_AUIP;
    FMOP_CTL &= (uint8) 0xf8;
    FMOP_CTL |= 0x05;
    FMOP_CTL |= 0x18;
    SFR_BANK = sfr_bak;
    paattr.volume = g_comval.volume;
    paattr.reserve[0] = g_comval.volume;
    EnablePA(&paattr);

}

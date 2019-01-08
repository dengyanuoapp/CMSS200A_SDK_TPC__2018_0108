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
 * Description : FM��ʼ��,������������,����,������
 *
 * Arguments   :	  	 ����ѡ��ֵ,����
 *
 * Returns     :		 ����ʼ���ɹ�,�򷵻�1,����,����0
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
    WriteBuffer[0] &= (uint8) 0xfe; //��ʼ��,disable SEEK

    WriteBuffer[3] &= (uint8) 0xf0;
    WriteBuffer[3] |= Set_Band;

    Set_SeekLevel = slevel; //������ʼ��
    WriteBuffer[6] = 0x80;
    WriteBuffer[6] |= Set_SeekLevel;

    //Set_Standby = 1;
    //Set_SearchMode = 0;

    I2C_Init(); //I2C��ʼ��
    result = FM_Standby_Drv(1);
    FM_Wait50ms();
    SetMfpToNand(); //��GPIO�е�NAND FLASH

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
 * Description : �˳�FM
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
 * Description : ����Ƶ��,����tune����
 *
 * Arguments   :	  �����õ�Ƶ��ֵ
 *
 * Returns     :		//����Ƶ���Ƿ�ɹ�,��Ϊ��̨,����1
 *					       ����,����0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_SetFreq(uint16 Freq)
{
    uint8 result;
    I2C_Init(); //��GPIO�е�I2C
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
    //SetMfpToNand(); //��GPIO�е�NAND FLASH
    return result;
}
/*
 ********************************************************************************
 *             uint8 FM_GetStatus(FMStatus_Tab_t * Buf)
 *
 * Description : ��ȡ����������̨��Ƶ�����������Ϣ
 *
 * Arguments   :	  �����̨��Ϣ�Ľṹ��ָ��
 *
 * Returns     :		//�Ƿ��ȡ״̬�ɹ�,�����ȡ�ɹ�,�򷵻�ֵΪ1
 *					       ����,����0
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_GetStatus(FMStatus_Tab_t *Buf)
{
    uint8 result;
    uint8 i;
    I2C_Init(); //��GPIO�е�I2C
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
        if (Is_Stere0 == 1) //��ȡ����������̨��Ƶ�����������Ϣ
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
 * Returns     :		 ������standby�ɹ�,�򷵻�1,����,����0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_Standby(void)
{
    uint8 result;
    I2C_Init(); //��GPIO�е�I2C
    result = FM_Standby_Drv(0);
    SetMfpToNand(); //��GPIO�е�NAND FLASH
    return result;
}
/*
 ********************************************************************************
 *             uint8 FM_Mute(FM_MUTE_t mode)
 *
 * Description : FM��������
 *
 * Arguments   :	  �Ƿ���,0Ϊȡ������,1Ϊ����
 *
 * Returns     :		 �����þ������Ƴɹ�,�򷵻�1,����,����0
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
    I2C_Init(); //��GPIO�е�I2C
    WriteBuffer[0] &= (uint8) 0xfe; //disable seek ,ע��Ӳ��SEEK��ʽ,������Ƶ��\����ʱ,����disable seek!!!!
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
    SetMfpToNand(); //��GPIO�е�NAND FLASH
    return result;
}
/*
 ********************************************************************************
 *             uint8 FM_Search(fm_play_status_t * Buf,uint16 Freq,uint8 SeekDirect)
 *
 * Description : FM��̨
 *
 * Arguments   :
 param1:  �����̨��Ϣ�Ľṹ��ָ��
 param2:  �����̨��ʽ,�����õ�Ƶ��ֵ
 param3:  ��������
 * Returns     :		����ѵ��ĵ�̨Ϊ��̨,�򷵻�1;����,����0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_Search(fm_play_status_t *Buf, uint16 Freq, uint8 SeekDirect)
{
    uint8 result, i;
#ifdef  HARDWARE_SEEK
    Set_SeekDirect = SeekDirect << 1; //Ӳ��SEEK��ʽ,������SEEK����
    Freq = Freq; //avoid keil warning
#else
    Freq_RF = Freq; //�����̨,����tuneƵ��
    SeekDirect = SeekDirect; //avoid keil warning
#endif
    I2C_Init();
    Assemble_WriteBuffer();
    for (i = 0; i < 2; i++)
    {
#ifdef  HARDWARE_SEEK
        result = WriteOneFrame(2); //Ӳ��SEEK��ʽ,ֻ��д02�Ĵ���,enable seek
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
 * Description : ����PA����
 *
 * Arguments   :	  �����õ�����ֵ
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
 * Description : ��̨�ж�
 *
 * Arguments   :
 *
 * Returns     :	 ���ѵ��ĵ�̨Ϊ��̨,����1;����,����0
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
    FM_Wait50ms(); //��ʱ20ms���ȴ�FM Tune��SEEK���
    do
    {
        i--; //��ȡ����
        result = ReadStatus();
    }
    while ((result == 0) && (i > 0));
    if (result != 0)
    {
#ifdef HARDWARE_SEEK
        temp = ReadBuffer[0] | 0xdf; //Ӳ��SEEK��ʽ����̨�жϴ���
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
        temp = ReadBuffer[2] & 0x01; //�����̨��ʽ����̨�жϴ���
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
 * Description : Ƶ������
 *
 * Arguments   :	�����õ�Ƶ��ֵ
 *
 * Returns     :	 ��Ƶ�����óɹ�������1;����,����0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 FM_SetFrequency(uint16 Freq)
{
    uint8 i, result;
    WriteBuffer[0] &= (uint8) 0xfe; //��ֹSEEK ,ע��Ӳ��SEEK��ʽ,������Ƶ��\����ʱ,����disable seek!!!!
    WriteBuffer[1] |= (0x01 << 0); //Enable FM
    WriteBuffer[2] = (uint8) (Freq >> 8);
    WriteBuffer[3] &= 0x3f;
    WriteBuffer[3] |= (uint8) (Freq % 256);
    WriteBuffer[3] |= (0x01 << 4); //enable tune
    WriteBuffer[6] = 0x80;
    WriteBuffer[6] |= Set_SeekLevel;
    for (i = 0; i < 2; i++)
    {
        result = WriteOneFrame(8); //����Ƶ��,д��ؼĴ���
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
 * Description : FM standby��wake up
 *
 * Arguments   :	 standby��־,1��ʾwake up,0��ʾstandby
 *
 * Returns     :	 �� standby��wake up�ɹ�������1;����,����0
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
 * Description : ����FM��д�Ĵ���,������̨
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
#ifdef HARDWARE_SEEK				//Ӳ��SEEK��ʽ
    WriteBuffer[0] |= 0x01; //seek enable
    WriteBuffer[0] &= (uint8)0xfd; //����ϴε�seek direct
    WriteBuffer[0] |= Set_SeekDirect; //���� SEEK Direct
    WriteBuffer[1] |= (0x01 << 0); //FM Enable
    WriteBuffer[1] &= 0x7f; //Seekmode=0

#else							   //�����̨��ʽ
    WriteBuffer[0] &= (uint8) 0xfe; //��ֹSEEK
    WriteBuffer[1] |= (0x01 << 0); //Enable FM
    WriteBuffer[2] = (uint8) (Freq_RF >> 8); //��������Ƶ��
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
 * Description : ��FM�ļĴ���,��ȡ��̨��Ϣ
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
    if ((ReadBuffer[0] & (0x01 << 2)) != 0) //stereo������ �ж�
    {
        StereoFlag = 1;
    }
    else
    {
        StereoFlag = 0;
    }
    Freq_RF = (uint16) (ReadBuffer[0] & 0x03); //��ȡ�������ĵ�̨Ƶ��
    Freq_RF <<= 8;
    Freq_RF += ReadBuffer[1];
}

void OpenPA(void)
{
    pa_t paattr;
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    paattr.pa_in.dacin = 0;
    paattr.pa_in.fmin = 1; //ѡ��PA������ΪFMIN
    paattr.pa_in.linein = 0;
    paattr.pa_in.micin = 0;

    SFR_BANK = 0x01; //ʹ��FMIN��PA��ͨ·
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

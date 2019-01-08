/*******************************************************************************
 *                              AS260A
 *                            Module: fm drvier for qn8035
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>        <time>           <version >            <desc>
 *     mikeyang     2011-11-03 15:00     1.0             build this file
 *******************************************************************************/

#include "fm_drv.h"


#pragma name(FM_DRV_INIT)

const uint16 rssi_snr_TH_tbl [20] =
{
    CCA_SENSITIVITY_LEVEL_0, CCA_SENSITIVITY_LEVEL_1,
    CCA_SENSITIVITY_LEVEL_2, CCA_SENSITIVITY_LEVEL_3,
    CCA_SENSITIVITY_LEVEL_4, CCA_SENSITIVITY_LEVEL_5,
    CCA_SENSITIVITY_LEVEL_6, CCA_SENSITIVITY_LEVEL_7,
    CCA_SENSITIVITY_LEVEL_8, CCA_SENSITIVITY_LEVEL_9,
    CCA_SENSITIVITY_LEVEL_10, CCA_SENSITIVITY_LEVEL_11,
    CCA_SENSITIVITY_LEVEL_12, CCA_SENSITIVITY_LEVEL_13,
    CCA_SENSITIVITY_LEVEL_14, CCA_SENSITIVITY_LEVEL_15,
    CCA_SENSITIVITY_LEVEL_16, CCA_SENSITIVITY_LEVEL_17,
    CCA_SENSITIVITY_LEVEL_18, CCA_SENSITIVITY_LEVEL_19
};

/**********************************************************************
 * Description: 初始化FM模组
 *
 * Arguments: none
 *
 * Returns: success/fail
 *
 * Note:
 *
 ***********************************************************************/
uint8 QN_ChipInitialization(void)
{
    //reset all registers, CCA_CH_DIS =1
    QND_WriteReg(0x00, 0x81);//对所有寄存器复位
    TM_DelayMS(20); //延时10MS以上，等待复位完成

    // change crystal frequency setting here
    //设置bit7选择时钟源
    QNF_SetRegBit(0x01, 0x80, 0x00);//setting clock source type:  0  sine-wave clock or  1  digital clock
    //以0x15,0x16,0x17进行设置来选择时钟频率
    //如果时钟频率为32.768KHz则可以不对这3个寄存器进行操作(使用默认值)
    QND_WriteReg(XTAL_DIV0, QND_XTAL_DIV0); //设置时钟频率
    QND_WriteReg(XTAL_DIV1, QND_XTAL_DIV1);
    QND_WriteReg(XTAL_DIV2, QND_XTAL_DIV2);

    TM_DelayMS(50);
    QND_WriteReg(0x54, 0x47);//mod pll setting
    QND_WriteReg(0x19, 0x40);//AGC setting
    QND_WriteReg(0x2d, 0xD6);//notch filter threshold adjusting
    QND_WriteReg(0x43, 0x10);//notch filter threshold enable

    //发送有限状态机校验命令给qn8035
    QND_WriteReg(0x00, 0x51);//reset the FSM
    //发送有限状态机检验命令完成
    QND_WriteReg(0x00, 0x21);//enter standby mode
    //等待有限状态机检验完成
    TM_DelayMS(100);

    //these variables are used in QNF_SetCh() function.
    qnd_R16 = QND_ReadReg(0x16);
    qnd_R17 = QND_ReadReg(0x17);
    qnd_R46 = QND_ReadReg(0x46);

    //check chipID==QN8035
    if (QND_ReadReg(0x06) == CHIPID_QN8035)
    {
        return 1;
    }
    return 0;
}

/***********************************************************************
void QND_RXSetTH(uint8 db)
 ***********************************************************************
 Description: Setting the threshold value of automatic scan channel
 db:
 Setting threshold for quality of channel to be searched,
 the db value range:0~(63-CCA_THRESHOLD).
 Return Value:
 None
***********************************************************************/

void QND_RXSetTH(uint8 db)
{
    uint8 rssiTH;
    uint8 snrTH;
    uint16 rssi_snr_TH;


    //rssi_snr_TH = rssi_snr_TH_tbl[db];
    rssiTH = (uint8) (rssi_snr_TH >> 8);	  //0x1b~0x1e
    //snrTH = (uint8) (rssi_snr_TH & 0xff);//0x1E06~0x1e0a
#ifdef PILOT_CCA
    snrTH = 0x0f;
#else
    snrTH = 0x04;
#endif
    //QNF_SetRegBit(0x49, 0x0f, 0x0A);
    QNF_SetRegBit(GAIN_SEL, 0x08, 0x08);//NFILT program is enabled
    //selection filter:filter3
    QNF_SetRegBit(CCA1, 0x30, 0x30);
    //Enable the channel condition filter3 adaptation,Let ccfilter3 adjust freely
    QNF_SetRegBit(SYSTEM_CTL2, 0x40, 0x00);
#ifdef PILOT_CCA
    QND_WRITE(0x2f, 0x00);
#endif

    //selection pilot threshold: 2/250(default)
    QNF_SetRegBit(0x30, 0x1C, 0x08);

    QND_WRITE(0x37, 0x00);
    QNF_SetRegBit(0x38, 0x3F, 0x01);
    //selection the time of CCA FSM wait SNR calculator to settle:20ms
    //0x00:	    20ms(default)
    //0x40:	    40ms
    //0x80:	    60ms
    //0xC0:	    100m
    QNF_SetRegBit(0x39, 0xC0, 0x00);
    //selection the time of CCA FSM wait RF front end and AGC to settle:20ms
    //0x00:     10ms
    //0x40:     20ms(default)
    //0x80:     40ms
    //0xC0:     60ms
    QNF_SetRegBit(0x3a, 0xC0, 0x80);
    QNF_SetRegBit(CCA, 0x3F, db); //setting RSSI threshold for CCA
    QNF_SetRegBit(0x39, 0x3F, snrTH); //setting SNR threshold for CCA
}

uint8 FMdrv_setthrod(uint8 level)
{
    FM_Slevel = level;
    QND_RXSetTH(level);
    return 1;
}
/*
 ********************************************************************************
 *             uint8 FMdrv_Init(uint8 mode)
 *
 * Description : FM 初始化函数
 *
 * Arguments   : mode: 暂不使用
 *
 * Returns     : 若初始化成功,则返回1,否则,返回0
 *
 * Notes       : 由驱动安装函数调用
 *
 ********************************************************************************
 */
uint8 FMdrv_Init(uint8 mode)
{
    uint8 ChipStatus;
    //mode = mode;
    ChipStatus = QN_ChipInitialization();
    if (ChipStatus != 0)
    {
        //enter RX mode
        QNF_SetRegBit(0x00, 0x30, 0x10);
    }
    QND_RXSetTH(mode);
    return ChipStatus;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_hardware(uint8 onoff,  uint8 PA_mode)
 *
 * Description : FM 相关硬件操作, 如AIN, PA等
 *
 * Arguments   : onoff:  1--打开FM时    0--关闭FM时
 *               PA_mode:   1-- 要操作PA    0--不操作PA
 *
 * Returns     : 若成功,则返回1,否则,返回0
 *
 * Notes       : 1. 此函数要结合驱动的安装卸载来调用
 *               2. PA_mode 主要是为兼容某些模组, 在main AP 中需要进standby而用
 ********************************************************************************
 */
uint8 FMdrv_hardware(uint8 onoff, uint8 PA_mode)
{
    onoff = onoff;
    PA_mode = PA_mode;
    return 1;
}

/*
 ********************************************************************************
 *             uint8 FMdrv_Exit(uint8 mode)
 *
 * Description : FM驱动退出函数
 *
 * Arguments   : mode: 无实际作用
 *
 * Returns     : 无实际作用
 *
 * Notes       : 由驱动卸载函数调用
 *
 ********************************************************************************
 */
uint8 FMdrv_Exit(uint8 mode)
{
    mode = mode;
    //first, enter standby
    //QNF_SetRegBit(0x4C, 0x08, 0x00);
    //QNF_SetRegBit(0x00, 0x30, 0x20);

    return 1;
}

/* FM 模组进入Standby */
uint8 FMdrv_Standby(void)
{
    //first, enter standby
    QNF_SetRegBit(0x4C, 0x08, 0x00);
    QNF_SetRegBit(0x00, 0x30, 0x20);
    return 1;
}


/*
 *******************************************************************************
 *                ACTOS AP
 *        main ap of sdk 3.0
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *       History Record:
 *
 *******************************************************************************
 */
#include "ap_radio.h"

#pragma name(FM_DEAL)

uint16 FMdeal(uint16 RunFMStatues)
{
    uint16 result = 0;
    uint8 key;
    uint8 count = 0;
    uint8 modekeystore = NULL;
    bool BandSwitch = TRUE, FirstWait = TRUE;
    RunFMStatues = RunFMStatues;
    //clear the message pool
    while (ap_get_message() != AP_KEY_NULL)
    {
        ; //清空消息池
    }

    while (1)
    {
        if (BandSwitch == TRUE) //初始化，重画 BandSwitch
        {
            result = drawBandSwitch(); //绘制 波段转换开关
            if (result != 0)
            {
                return result;
            }
            BandSwitch = FALSE;
            FirstWait = TRUE;
        }
        if (need_draw == TRUE) //屏幕重画
        {
            need_draw = FALSE;
            RefreshMainScreen();
        }
        if (FirstWait == TRUE)
        {
            FirstWait = FALSE;
            //ap_sleep(3);        //初次进入AP等待1.5S释放MUTE(静音)
            fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL); //设置声音大小
            fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL); //获取状态
            if (g_comval.volume != 0)
            {
                fmSendCommand(FM_MUTE, releaseMUTE, NULL); //fmSendCommand(MC_MUTE, releaseMUTE);
            }
            if (RunFMStatues == ALARM_TO_FM)
            {
                VMWrite(&g_radio_config, VM_AP_RADIO, sizeof(radio_config_t));
                return RESULT_FMREC_START;
            }
        }

        deal_pc_eqsrs(0);

        result = deal_key_msg();
        if (result)
        {
            return result;
        }
    }
    return key; //不会运行到这里
}

//绘制 波段转换开关
uint16 drawBandSwitch(void)
{
    uint8 sfr_bak;

    if (g_radio_config.band_mode == Bank_US_Europe) //FM 波段模式 ，如果为普通频段
    {
        radio_param.param1 = g_radio_config.fmstation_us; //US Tab
    }
    else
    {
        radio_param.param1 = g_radio_config.fmstation_jp; //JP Tab
    }

    (fm_status_t *) radio_param.param2 = &g_radio_config.FMStatus;//当前播放的电台信息

    fmSendCommand(FM_MID_RESTORE, &radio_param, NULL); //恢复

    fmSendCommand(FM_MID_SETVOLUME, 0, NULL); //设置声音为0

    if (!fmSendCommand(FM_OPEN, SEARCHLEVEL, NULL)) //如果打开失败，硬件错误提示
    {
        LEDClearScreen();
        /* ERR */
        LEDDisplay(NUMBER1, NUM_E, 1);
        LEDDisplay(NUMBER2, NUM_R, 1);
        LEDDisplay(NUMBER3, NUM_R, 1);
        ap_sleep(3);
        fmSendCommand(FM_MID_SETVOLUME,  (void *) g_comval.volume, NULL);
        return RESULT_MAIN; //返回主菜单
    }

    fmSendCommand(FM_MUTE, SetMUTE, NULL); //设置静音

    fmSendCommand(FM_MID_PLAY, NULL, NULL); //播放电台

    init_fmin_channel();

    sfr_bak = SFR_BANK;
    SFR_BANK = 0x13;
    I2S_CTL1 = 0;
    SFR_BANK = sfr_bak;

    return 0;
}

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

#pragma name(AP_MAIN)

/* ���¶���FMӦ���г�פ���� */

//radio ui ����������Ϣ
radio_config_t g_radio_config;

//Ӧ�ý���ģʽ. 0xff--ֻ��ΪʹFMģ�����standby
uint8 EnterFlag = 0xff;

//�Զ���̨������,��ǰ���ѵ�̨�ĵ�̨��
uint8 AutoTabNum;

//��̨�����л�ȡ��״̬
fm_status_t g_FMStatus;

//Ӳ����̨���, bit0  ����Ӳ��seek �Ƿ����    1:  �ѽ���   0:  δ����
//bit1  ����Ӳ��seek �ҵ����Ƿ���Ч̨   1:  ��Ч̨
uint8 seekflag;

//radio ���е�̨�б�ṹ��Ϣ
radio_citylist_t g_city_station;

/* ָ��ǰ�����ŵĵ�̨�б����*/
uint16 *pfreq_play;

//RDS�رմ�״̬
radio_RDS_e use_RDS;

//fm_play_status_t FMStatusUS; 	//US��̨��Ϣ�ݴ�,8 bytes
//fm_play_status_t FMStatusJP; 	//Japan��̨��Ϣ�ݴ�,8bytes
fm_status_t FMStatusUS =
{ USFREQ_MIN, 0, 0, Mono };
fm_status_t FMStatusJP =
{ JPFREQ_MIN, 0, 0, Mono };

//��ǰ��̨��Ϣ
FMRADIO_param_t radio_param;

uint8 flag_r; //��̨�Ƿ��ѵ���һ��̨֮�󷵻ر�־λ

//��VRAM�ж�����EQ,SRS������ͨ��EQ��������
EQ_VM_t g_eq_para;

int8 AuCodecRun = 0;
int16 SentenceDectTime;
uint8 SentenceDectEnergy;
int16 SentenceDectTime_bak;
int8 c_decEffectFlag;
int8 SetenceFlag = 0;
uint8 m_decSpdifTimer_Sav, m_decAnalogTimer_Sav;
uint32 m_decSpdifddrSav;

int8 last_SetenceFlag;

#ifdef AP_RADIO_REPORT_EN
char report_buf[16]; //��������ݵ�buffer
info_report_t info =
{   report_buf, 0, TRUE};
#endif

uint8 report_enter_ap(uint8 report_msg);

/*
 ********************************************************************************
 *			  uint16 main(uint16 param)
 *
 * Description : Radio UI Ӧ��������
 *
 * Arguments   :
 param:
 0, ����FM,���е�̨����
 1, Alarm����FM
 2, ʹFM����Standby
 *
 * Returns     : �����Ϣ��0
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint16 main(uint16 param)
{
    uint16 result = 0;
    uint8 KeyTone_bak = 0;
    read_var();
#ifdef AP_RADIO_REPORT_EN
    result = report_enter_ap(EXTEND_FM);
    if(result != 0)
    {
        if(param != RESULT_RADIO)
        {
            fmSendCommand(FM_CLOSE, 0, NULL);
        }
        return result;
    }
#endif

    result = FMinit(param); //FM��ʼ��
    if (result == RESULT_MAIN)
    {
        return result;
    }

    if (g_comval.KeyTone != 0)
    {
        KeyTone_bak = g_comval.KeyTone; //���ݰ�����
        g_comval.KeyTone = 0; //�رհ�����
    }

    result = FMdeal(param); //FM����
    g_comval.KeyTone = KeyTone_bak; //�ָ�������

    fmSendCommand(FM_MID_SAVE, &radio_param, NULL); //�������
    exit_fmin_channel();
    if (result == RESULT_RECORD) //�ػ�FM������Rec�ȼ�¼��
    {
        result = RESULT_FMREC_START;
#ifdef AP_RADIO_REPORT_EN
        fmSendCommand(FM_MUTE, SetMUTE, NULL);
        result = report_enter_ap(EXTEND_RECORD);
        //init_fmin_channel();
        if(result == 0)
        {
            //return result;
            result = RESULT_FMREC_START;
        }
        if (g_comval.volume != 0)
        {
            fmSendCommand(FM_MUTE, releaseMUTE, NULL); //�������
        }
        fmSendCommand(FM_MID_SETVOLUME, (void *) g_comval.volume, NULL);
#endif
    }
    FMexit(result); //FM�˳�����

    return result;
}
/*
 ********************************************************************************
 *             void charge_pump_onoff(uint8 flag)
 *
 * Description : �򿪺͹ر�charge pump
 *
 *
 * Arguments   : flag:0-�رգ�1-��
 *
 * Returns     : void
 *
 * Notes       :���ڴ�charge pumpʱ��Ӱ����̨Ч�����������Զ��͹��Զ�������̨ʱ
 *                 ����ر�charge pump,����̨֮���ٴ�,����uhost����charge pump���磬�ر��Ժ�
 *                 �������usb�������u�̲�����Ϣ����������̨��������Ҫ����u�̲�����Ϣ
 *		         ͬʱ���������´�charge pump��ϵͳ��⵽uhost��Ϣ��Ҫһ����ʱ�䣬���Ե�
 *                 ��ʱ����500��ms����������⣬Ȼ���ٹ���u����Ϣ
 *
 ********************************************************************************
 */

void charge_pump_onoff(uint8 flag)
{
    uint8 sfr_bak = SFR_BANK;
    //enable charge pump
    SFR_BANK = BANK_PMU;
    if(flag == 1)
    {
        CP_CTL0 |= 0x01; //��ɱ�ʹ��
        CP_CTL1 |= 0x08;	//charge pumpֱͨ����ʹ��
    }
    else
    {
        CP_CTL0 &= 0xFE; //��ɱùر�
        CP_CTL1 &= 0xF7;
    }
    SFR_BANK = sfr_bak;
    //�������ʱ��Ϊ����ȷ��⵽u����Ϣ���Ա����
    if(flag == 1)
    {
        TM_DelayMS(200);
        TM_DelayMS(200);
        TM_DelayMS(200);
    }

}
uint16 FMinit(uint16 param)
{
    uint16 result = RESULT_NULL;

    SetPLL(PLL_24MHZ);

    if ((param == FM_TO_STANDBY) || (param == RESULT_FMREC2STANDBY)) //��FMģ���standby
    {
        fmSendCommand(FM_OPEN, SEARCHLEVEL, NULL); //fmSendCommand(MC_OPEN, SEARCHLEVEL);
        fmSendCommand(FM_MUTE, SetMUTE, NULL); //fmSendCommand(MC_MUTE, SetMUTE);
        fmSendCommand(FM_CLOSE, 0, NULL); //fmSendCommand(MC_CLOSE, 0);
        SetPLL(PLL_48MHZ);
        return RESULT_MAIN;
    }

    //-------��װud����------------
    DRV_ActiveUD(0x00, MODE_SLOW);
    //OpenSPK(sfr_bak);
    EnterFlag = 0; //when fm  open,  open PA
    mute_flag = FALSE;

    SetCurUI_Type(FMFreq_UI);

    if (param == ALARM_TO_FM) //��������Ƶ��
    {
        alarmtimerflag = 0;
    }

    return result;
}

void read_var(void)
{

    read_ap_comval();
    //   VMRead(&g_alarm, VM_ALARM, sizeof(g_alarm)); //��ȡ����ȫ�ֱ���

    VMRead(&g_radio_config, VM_AP_RADIO, sizeof(g_radio_config)); //��ȡ������Ϣ
    if (g_radio_config.magic != MAGIC_FMRADIO) //�ж��Ƿ���Ҫ��ʼ��
    {
        memset(&g_radio_config, 0x0, sizeof(g_radio_config));
        g_radio_config.magic = MAGIC_FMRADIO;
        g_radio_config.band_mode = Bank_US_Europe; //Ĭ�� ��ͨƵ��
        memcpy(&g_radio_config.FMStatus, &FMStatusUS, sizeof(fm_status_t)); //���� ��ͨƵ�� ������

        memcpy(&g_FMStatus, &FMStatusUS, sizeof(fm_status_t)); //���� ��ͨƵ�� ������

        //Ĭ��Ϊʹ��RDS  ��̨����
        use_RDS = RDS_Open;
    }
}

void FMexit(uint16 result)
{
    if ((result != RESULT_FMREC_NOSTART) && (result != RESULT_FMREC_START)) //�ǵ�̨¼���˳���for FM record by ccm
    {
        //DealFadeInOut();
        fmSendCommand(FM_CLOSE, 0, NULL);
    }
    //TM_KillClock((int8) Timer_Sav); //�رն�ʱ��
    //���ǵ�̨¼���˳�����ʱFM���ڹ��������ܶ�HOSC���в���
    VMWrite(&g_radio_config, VM_AP_RADIO, sizeof(radio_config_t));
    VMWrite(&g_comval, VM_SYSTEM, sizeof(ap_comval_t));
    //    VMWrite(&g_alarm, VM_ALARM, sizeof(alarm_vars_t));

    SetPLL(PLL_48MHZ);
}

/* �����̨*/
uint16 fm_menu_save(uint16 *arg)
{
    arg = arg;
    if (fmSendCommand(FM_MID_SAVESTATION, NULL, NULL) != FALSE) //���浱ǰ��̨��Ӧ�ĵ�̨����š�
    {
        fmSendCommand(FM_MID_SAVE, &radio_param, NULL); //���浱ǰ��̨
    }
    return RESULT_NULL;
}

/* ��̨¼��*/
uint16 fm_menu_record(uint16 *arg)
{
    arg = arg;
    return RESULT_FMREC_NOSTART; //RESULT_FMREC_START;
}

/* ɾ����̨*/
uint16 fm_menu_delete(uint16 *arg)
{
    arg = arg;
    if (fmSendCommand(FM_MID_DELETE, NULL, NULL) != FALSE)
    {
        fmSendCommand(FM_MID_SAVE, &radio_param, NULL);
    }
    fmSendCommand(FM_SEARCH, &g_FMStatus, UP);
    fmSendCommand(FM_GETSTATUS, &g_FMStatus, NULL);
    return RESULT_NULL;
}

/* ɾ��ȫ��*/
uint16 fm_menu_delete_all(uint16 *arg)
{
    arg = arg;
    fmSendCommand(FM_MID_DELALL, NULL, NULL);
    fmSendCommand(FM_MID_SAVE, &radio_param, NULL);
    return RESULT_NULL;
}

/* ��ͨƵ��*/
uint16 fm_menu_normal_band(uint16 *arg)
{
    arg = arg;
    if (g_radio_config.band_mode == Bank_Japan)
    {
        g_radio_config.band_mode = Bank_US_Europe;
        radio_param.param1 = g_radio_config.fmstation_us; //US Tab  by ljs

        memcpy(&FMStatusJP, &g_FMStatus, sizeof(g_FMStatus)); //����jpƵ������
        memcpy(&g_FMStatus, &FMStatusUS, sizeof(g_FMStatus)); //����usƵ������
    }
    return RESULT_NULL;
}

/* �ձ�Ƶ��*/
uint16 fm_menu_japan_band(uint16 *arg)
{
    arg = arg;
    //���ԭ������ͨģʽ������Ҫ���ձ�ģʽ
    if (g_radio_config.band_mode == Bank_US_Europe)
    {
        g_radio_config.band_mode = Bank_Japan;
        radio_param.param1 = g_radio_config.fmstation_jp; //JP Tab   by ljs

        memcpy(&FMStatusUS, &g_FMStatus, sizeof(fm_status_t)); //����usƵ������
        memcpy(&g_FMStatus, &FMStatusJP, sizeof(fm_status_t)); //����jsƵ������
    }
    return RESULT_NULL;
}

/* RDS ����*/
uint16 fm_menu_rds(uint16 *arg)
{
    arg = arg;
    return RESULT_NULL;
}

/* FM ¼��*/
uint16 fm_menu_fm_record(uint16 *arg)
{
    arg = arg;

    return 0;
}

/* �˳�*/
uint16 fm_menu_exit(uint16 *arg)
{
    arg = arg;
    //����Ҫ����ֱ���˳�
    return RESULT_NULL;
}

#ifdef AP_RADIO_REPORT_EN
uint8 report_enter_ap(uint8 report_msg)
{
    uint8 retval = NULL;
    if(g_comval.SuppTTS != 0)
    {
        report_buf[0] = ' ';
        report_buf[1] = report_msg;
        report_buf[2] = ' ';
        report_buf[3] = 0;
        check_report_state(&info);
        retval = wait_report_end(NULL);
    }
    return retval;
}
#endif

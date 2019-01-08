/*
 *******************************************************************************
 *                ACTOS AP
 *        ap common lib file, udisk use this file too
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ap_common.c,v 1.1.1.1 2006/05/22 03:23:59 selina Exp $
 *******************************************************************************
 */

#include "ap_common.h"
#define POWER_SAVE_MODE
//========== global field ===============
#pragma name(COM_COM)

uint16 g_standby_time = 0; //˯��ʱ��,0.5��Ϊ��λ
uint16 g_rtc_counter = 0; //rtc ��Ϣ����
uint16 key_count = 0; //��ǰ������Ϣ�����Ĵ���
uint8 key_value = Msg_KeyNull; //��ǰ������ֵ
uint8 lowpower_count;//���ܵ��͵�ļ���
uint8 file_name[12]; //����ļ���������

char temp_buffer[9];
time_t time;
date_t date;//���ڽṹ��
time_t tmp_time;
uint8 DigitalCount; //��ǰ���ֽ��ĸ���
uint8 DigitalBuf[4]; //�洢���ּ�����Ӧ������
uint8 set_what;//��ǰ��������
uint8 show_what;//��ǰ�����������л�����
uint8 show_ui_flag;//�����ʼ������־
uint8 show_count;//��ʼ�����������ļ���
uint8 CurUI_Type;//��ǰ��������ʾ����
//alarm_vars_t g_alarm;
ap_comval_t g_comval; //ϵͳ����,ap���ܸ���
BOOL need_draw;
uint8 mute_flag = FALSE;
//uint8 linein_status; //linein�ߴ���״̬��0-û�в���linein�ߣ�1-linein�߲���
uint8 holdkey_flag;//�����������
/*
uint8 insert_msg[MAX_TYPE] =
{ AP_MSG_LINEIN_IN, AP_MSG_MIC_IN,AP_MSG_USB_PLUGIN};
uint8 pull_out_msg[MAX_TYPE] =
{ AP_MSG_LINEIN_OUT, AP_MSG_MIC_OUT,USB_PLUGIN_FINISH };
*/

/********************************************************************************
 * Description :���õ�ǰʹ�õĽ������Ͳ���ˢ��ȫ���ı��
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
void SetCurUI_Type(uint8 UI_Type)
{
    CurUI_Type = UI_Type;
    show_count = 0;
    need_draw = TRUE;
    DigitalCount = 0;
    memset(DigitalBuf, 0xFF, 4);
    memset(temp_buffer, 0xFF, 9);
    if (UI_Type == Clock_UI)
    {
        memset(&tmp_time, 0xff, sizeof(time_t));
    }
    else if ((UI_Type == TimeSet_UI) || (UI_Type == AlarmSet_UI))
    {
        LEDClearScreen();
    }
}
/********************************************************************************
 * Description :�ⲿpa��Դ���ؿ���
 *
 * Arguments  :
 *
 * Returns     :
 *            ��
 * Notes       :
 *
 ********************************************************************************/
void switch_pa_power(uint8 flag)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_GPIO;
    if(flag == 0)
    {
        PA_POWER_GPIO_DAT &= ~ (1 << PA_POWER_GPIO_NUM);
    }
    else
    {
        PA_POWER_GPIO_DAT |=  (1 << PA_POWER_GPIO_NUM);
    }
    SFR_BANK = sfr_bak;
}
void close_usb_phyctrol(void)
{
#ifdef POWER_SAVE_MODE
    uint8 sfrbak, i;
    sfrbak = SFR_BANK;
    SFR_BANK = BANK_USB;
    USBEIRQ = 0xc0;
    USBEIRQ = 0x00;

    //set Uram clk to MCU
    //   SFR_BANK = BANK_CMU_RMU;
    //    MEMCLKSELCTL1 = 0x00; //PCMRAM and URAM to MCU clk

    SFR_BANK = BANK_USB;
    Usbirq_hcusbirq = 0xff;
    Usbien_hcusbien = 0x00;
    SHORTPCKIRQ = 0x20;

    SFR_BANK = BANK_USB;
    /* software disconnect bit.  */
    UsbCTRL_STUS |= 0x40;
    SFR_BANK = BANK_CMU_RMU;
    MRCR2 = MRCR2 & 0xf7;  //USBReset
    i = 0x40;
    while(i--)
    {
        ; //do nothing
    }

    MRCR2 = MRCR2 | 0x08; //USBReset = nomal;
    SFR_BANK = BANK_USB;
    TM_DelayMS(2);

    //phy enable,FPGA invalid ,note:enable is close
    USB_PHYCTRL = USB_PHYCTRL | (uint8) 0xc0;
    DPDMCTRL = 0x10; // �ָ�uhost��?    //restore SFR_BANK
    SFR_BANK = sfrbak;
#endif
}
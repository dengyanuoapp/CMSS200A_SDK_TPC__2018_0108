/*
 *******************************************************************************
 *                ACTOS AP
 *           ap common lib file
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *  $Id: ui_err_msg.c,v 1.1.1.1 2006/05/22 03:23:59 selina Exp $
 *******************************************************************************
 */

#include "ap_common.h"
//#include "common.h"
#pragma name(COM_ERR_MSG)
/*
 ********************************************************************************
 * Description : ��ʱһ��ʱ�䣬��0.5��Ϊ��λ
 *
 * Arguments   :
 *    seconds:��ʱ��ʱ�䣬һ����λΪ0.5��

 * Returns     :
 *
 * Notes       :
 *    ����ʱ�����У����˴���RTC��Ϣ�⣬������һ����Ϣ����������
 ********************************************************************************
 */

void delay_HalfSecond(uint8 seconds)
{
    uint8 n = 0; //rtc counter
    uint8 key;
    //�����Ϣ�أ��Է���Ϣ������RTC��Ϣ�����¼�ʱ��׼ȷ
    while (ap_get_message() != NULL)
        ;
    while (1)
    {
        key = ap_get_message();
        //sleep ʱ����timeout
        if (key == AP_MSG_RTC)
        {
            n++;
            if (n >= (uint8) seconds)
            {
                break;
            }
        }
        else
        {
            continue;
        }
    }
}
/*
 ********************************************************************************
 * Description : ϵͳ������Ϣ
 *
 * Arguments   :
 *    msgID,      ������ϢID
 *    string_id,  ����

 * Returns     :
 *        0
 * Notes       :
 *    ������˳�,û�а����ȴ�5��֮���˳�.
 ********************************************************************************
 */
uint16 ui_err_msg(uint16 msgID)
{

    ClearScreen(NULL);
    ResShowMultiString(msgID, 255, 32);
    UpdateScreen(NULL);
    delay_HalfSecond(4);
    return 0;
}

/*
 ********************************************************************************
 * Description : ϵͳ������Ϣ
 *
 * Arguments   :
 *    msgID,      ������ϢID
 *    string_id,  ����

 * Returns     :
 *        0
 * Notes       :
 *    �����˻�
 ********************************************************************************
 */
uint16 ui_show_msg(uint16 msgID) //��DELAY
{
    ClearScreen(NULL);
    ResShowMultiString(msgID, 255, 16);
    UpdateScreen(NULL);
    delay_HalfSecond(2);
    return 0;
}


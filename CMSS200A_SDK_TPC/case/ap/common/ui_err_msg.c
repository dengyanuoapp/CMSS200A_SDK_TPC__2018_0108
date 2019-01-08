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
 * Description : 延时一定时间，以0.5秒为单位
 *
 * Arguments   :
 *    seconds:延时的时间，一个单位为0.5秒

 * Returns     :
 *
 * Notes       :
 *    在延时过程中，除了处理RTC消息外，其他的一切消息都不做处理
 ********************************************************************************
 */

void delay_HalfSecond(uint8 seconds)
{
    uint8 n = 0; //rtc counter
    uint8 key;
    //清空消息池，以防消息池中有RTC消息，以致计时不准确
    while (ap_get_message() != NULL)
        ;
    while (1)
    {
        key = ap_get_message();
        //sleep 时不管timeout
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
 * Description : 系统警告信息
 *
 * Arguments   :
 *    msgID,      错误信息ID
 *    string_id,  语言

 * Returns     :
 *        0
 * Notes       :
 *    任意键退出,没有按键等待5秒之后退出.
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
 * Description : 系统警告信息
 *
 * Arguments   :
 *    msgID,      错误信息ID
 *    string_id,  语言

 * Returns     :
 *        0
 * Notes       :
 *    立即退回
 ********************************************************************************
 */
uint16 ui_show_msg(uint16 msgID) //无DELAY
{
    ClearScreen(NULL);
    ResShowMultiString(msgID, 255, 16);
    UpdateScreen(NULL);
    delay_HalfSecond(2);
    return 0;
}


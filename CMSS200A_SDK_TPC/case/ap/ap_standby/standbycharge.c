#include "ap_common.h"
#include "actos.h"

#pragma name(AP_STANDBYCHARGE)

uint8 GetCureBatValue(void)
{
    uint8 bat_value = 0;
    uint8 i;
    bat_value = GetBattery();
    for (i = 0; i < 5; i++)
    {
        if (bat_value <= batvalue[i])
        {
            break;
        }
    }
    if (i >= 5)
    {
        i = 4;
    }
    if (i == 0)
    {
        return 0;
    }
    else
    {
        return (i * 2) - 2;
    }

}
uint8 StandbyCharge(void)
{
    uint8 key;
    uint8 bat_value = 0;
    uint8 result;
    uint8 retval;
    uint8 full_flag = FALSE, charging_flag = TRUE;
    uint8 charge_status;

    ClearScreen(NULL);
    UpdateScreen(NULL);

    while (ap_get_message() != NULL)
    {

    } //清空消息池
    while (1)
    {
        key = ap_get_message(); //获取消息
        switch (key)
        {
        case AP_KEY_MODE | AP_KEY_UP:
            //   case AP_KEY_PLAY | AP_KEY_UP:
            retval = GetUsbCableStatus();
            if ((retval & 0x40) != 0)
            {
                result = RESULT_UDISK;
            }
            else
            {
                result = RESULT_MAIN;
            }
            break;
        case RESULT_AUTO_BACK://AP_MSG_WAIT_TIMEOUT: //8秒退出
            result = RESULT_NULL;
            break;

        case AP_MSG_RTC:
        case AP_MSG_CHARGING:
            if (!GetUsbCableStatus()) //USB cable out
            {
                //sKY_ChargeSet(2);
                //sKY_ChargeGetandSet(); //usb拔出时一定要检测一次充电状态
                KY_GetChargeStatus();
                result = RESULT_IGNORE;
                break;
            }
            charge_status = KY_GetChargeStatus();      // sKY_ChargeSet(0);
            if (charge_status == 0x01)
            {
                //                if ((full_flag == TRUE) && (charging_flag == FALSE))
                //                {
                //                    charging_flag = TRUE;
                //                    full_flag = FALSE;
                //                    ResShowPic(CHARGING, 0, 128);
                //                    ResShowPic(COMMBK,0, 0);
                //                }
                ResShowPic((uint16) (BATTLVL1 + bat_value), 60, 24);
                bat_value++;
                if (bat_value > 4)
                {
                    bat_value = 0;
                    //	bat_value=GetCureBatValue();
                }

            }
            else
            {
                if ((charge_status == 0x02) && (full_flag == FALSE))
                {
                    full_flag = TRUE;
                    charging_flag = FALSE;
                    ResShowPic(BATTLVL1, 60, 24);
                    //ResShowPic(BATTCHA9, 12, 52);
                }
                else if (charge_status == 0x0)
                {
                    //ui_err_msg(NOBATTER); 没有电池提示；
                    result = RESULT_MAIN;
                    break;
                }
                else
                {
                }
            }
            result = RESULT_NULL;
            break;
        default:
            result = ap_handle_hotkey(key);
            if (result == RESULT_REDRAW)
            {
                //                SetBackgdColor(Color_BLACK_def);
                ClearScreen(NULL);
                result = RESULT_NULL;
                break;
            }
            else if ((result != 0) && (result != RESULT_STANDBY))
            {
                break;
            }
            else
            {
                break;
            }
        } /*end of swith */
        if (result != RESULT_NULL)
        {
            break;
        }
    } /* end of while(1) */
    return result;
}

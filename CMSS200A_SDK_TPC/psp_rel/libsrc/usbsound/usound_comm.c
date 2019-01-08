/********************************************************************************
 *                              AS260A
 *                            Module: USB Sound
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2012-10-23 16:40     1.0                build this file
 ********************************************************************************/
/*!
 * \file     usound_comm.c
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               公用接口
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 *
 *      Copyright(c) 2001-2012 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2012/10/23
 *******************************************************************************/
#include "usound_intern.h"
#include "usound_extern.h"
#pragma name(USC_COMM)
#pragma renamecode(?PR?USC_COMM)

/******************************************************************************/
/*!
 * \par  Description:
 *     check usb line connect
 * \param[in]    none
 * \param[out]   none.
 * \return       usb connect status

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
uint8 is_usb_connect(void)
{
    uint8 ret_var, sfr_temp;

    ClearWatchDog();
    sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;

    /* if plug-in bit is disconnect, line is pulled out.  */
    if (0x00 == (DPDMCTRL & 0x40))
    {
        ret_var = USB_DISCONNECT;
        usb_state |= USOUND_PLUG_OUT;
    }
    else
    {
        ret_var = USB_IS_CONNECT;
    }

    SFR_BANK = sfr_temp;

    return ret_var;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     time delay
 * \param[in]    mscount:delay count
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void mdelay(uint8 mscount)
{
    uint32 count;
    count = 0x00004000 * mscount;
    while (count != 0x00)
    {
        count = count - 1;
    }
}

/*void usdelay(uint8 uscount)
{
    uint32 count;
    count = 0x10 * uscount;
    while (count != 0x00)
    {
        count = count - 1;
    }
}*/


/******************************************************************************/
/*!
 * \par  Description:
 *     set hid data
 * \param[in]    keymsg:hid data value
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void usound_key_msg(uint8 keymsg)
{
    hid_report = keymsg;
    hid_send_flag = 2;
}

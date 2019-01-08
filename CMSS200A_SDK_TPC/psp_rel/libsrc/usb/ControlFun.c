/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : ControlFun.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-16 08:19
 *********************************************************************************************************
 */

#include "udisklib.h"

#pragma name(CONTROLFUN_C)
#pragma renamecode(?PR?CONTROLFUN_C)

#pragma ROM(HUGE)
/*
 ********************************************************************************
 *                           Control_Read
 * Description : Perform control read operation.
 * Arguments   : de: The address of data to be transfered to the host;
 *             : bc: The data length;
 *             : (sp+2): Request transfer Length.
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
void Control_Read(char *data_p, int data_length, int request_length)
{
    char *dataAddr;
    int i, sendLength, count;

    dataAddr = data_p;

    if (request_length < data_length)
    {
        sendLength = request_length;
    }
    else
    {
        sendLength = data_length;
    }

    while (sendLength != 0)
    {
        count = ((sendLength - 0x40) >= 0) ? 0x40 : (sendLength % 0x40);
        for (i = 0; i < count; i++)
        {
            //fill ep0 fifo
            Ep0indata = (char *) dataAddr[i];
        }
        In0bc_hcout0bc = (uint8) count;
        //FIFO Empty?
        while ((Ep0cs_hcep0cs & 0x04) != 0)
        {
            //拔线处理
            ifIsConnect();
        }
        sendLength = sendLength - count;
        dataAddr = dataAddr + count;
    }

    //AOTG ack with zero packet status stage
    Ep0cs_hcep0cs = Ep0cs_hcep0cs | 0x02;

    //clear setup packet interrupt
    Usbirq_hcusbirq = 0xff;
}

/*
 ********************************************************************************
 *                           NoData_PhaseReturn
 * Description : Return zero length data to host for Phase stage in control transfer.
 * Arguments   : none
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
void NoData_PhaseReturn(void)
{
    //AOTG ack with zero packet status stage
    Ep0cs_hcep0cs = Ep0cs_hcep0cs | 0x02;
    //clear setup packet interrupt
    Usbirq_hcusbirq = 0xff;
}

/*
 ********************************************************************************
 *                           Set_EPA_BulkInMode
 * Description : Set Endpoint A Bulk IN type.
 * Arguments   : none
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
void Set_EPA_BulkInMode(void)
{
    //usb speed mode
    if ((BKDOOR & 0x40) != 0)
    {
        //EPA High Speed,Max Packet Size=512
        Hcout1maxpckl = 0x00;
        Hcout1maxpckh = 0x02;
    }
    else
    {
        //EPA Full Speed,Max Packet Size=64
        Hcout1maxpckl = 0x40;
        Hcout1maxpckh = 0x00;
    }

    //quad buffer, bulk transfer, endpoint enable
    In1ctrl_hcout1ctrl = 0x8B;

    //select ep1
    EPRST = 0x01;
    //reset toggle
    EPRST = 0x21;
    //reset fifo
    EPRST = 0x41; //reset fifo
    //set EPA fifo auto mode, with full packet auto in
    Fifoctrl = 0x21;
}

/*
 ********************************************************************************
 *                           Set_EPB_BulkOutMode
 * Description : Set Endpoint B Bulk Out type.
 * Arguments   : none
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
void Set_EPB_BulkOutMode(void)
{
    //usb speed mode
    if ((BKDOOR & 0x40) != 0)
    {
        //EPB High Speed,Max Packet Size=512
        Hcin2maxpckl = 0x00;
        Hcin2maxpckh = 0x02;
        //set EPA & EPB max packet size
        max_pkt_size = 0x200;
    }
    else
    {
        //EPB Full Speed,Max Packet Size=64
        Hcin2maxpckl = 0x40;
        Hcin2maxpckh = 0x00;
        //set EPA & EPB max packet size
        max_pkt_size = 0x40;
    }

    Out2ctrl_hcin2ctrl = 0x8B;//0x8B;

    //set EPB short packet nakout mode
    AUTONAKCTRL = AUTONAKCTRL | 0x02;
    //disable out packet counter
    OUTPCKCNTH = 0x00;

    //set fifo out direction(EPB)

    //select ep2
    EPRST = 0x02;
    //reset toggle
    EPRST = 0x22;
    //reset fifo
    EPRST = 0x42; //reset fifo
    //set EPB fifo auto mode, recieve full packet, move to next sub-buffer
    Fifoctrl = 0x22;
}

/*
 *********************************************************************************************************
 *                                           Rest_Endp_Toggle
 *
 * Description: Reset usb bulk endpoint data toggle(data0/data1).
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : for compatible of WIN98.
 *********************************************************************************************************
 */
void Rest_Endp_Toggle(void)
{
    //reset endpointA toggle
    EPRST = 0x01;
    EPRST = 0x21;

    //reset endpointB toggle
    EPRST = 0x02;
    EPRST = 0x22;
}

/*
 ********************************************************************************
 *                           Stall_Endpoint
 * Description : syntax error, Must Stall EndPoint.
 * Arguments   : e: The endpoint whitch is selected to stall;
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
void Stall_Endpoint(char Ep_Num)
{
    switch (Ep_Num)
    {
    case ENDPOINT_0:
        Ep0cs_hcep0cs = Ep0cs_hcep0cs | 0x01;
        break;
    case ENDPOINT_A:
        In1ctrl_hcout1ctrl = In1ctrl_hcout1ctrl | 0x40;
        break;
    case ENDPOINT_B:
        Out2ctrl_hcin2ctrl = Out2ctrl_hcin2ctrl | 0x40;
        break;
    case ENDPOINT_C:
        In3ctrl_hcout3ctrl = In3ctrl_hcout3ctrl | 0x40;
    default:
        break;
    }
}

/*
 ********************************************************************************
 *                           Recover_Stall_Endpoint
 * Description : Recover the Stall EndPoint.
 * Arguments   : e: The endpoint whitch is selected to recover;
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
void Recover_Stall_Endpoint(char Ep_Num)
{
    switch (Ep_Num)
    {
    case ENDPOINT_0:
        Ep0cs_hcep0cs = Ep0cs_hcep0cs & (uint8) 0xFE;
        break;
    case ENDPOINT_A:
        In1ctrl_hcout1ctrl = In1ctrl_hcout1ctrl & (uint8) 0xBF;
        break;
    case ENDPOINT_B:
        Out2ctrl_hcin2ctrl = Out2ctrl_hcin2ctrl & (uint8) 0xBF;
        break;
    case ENDPOINT_C:
        In3ctrl_hcout3ctrl = In3ctrl_hcout3ctrl & (uint8) 0xBF;
    default:
        break;
    }
}

/*
 *********************************************************************************************************
 *                                           TestModeTest
 *
 * Description: USB hardware test.
 *
 * Arguments  : e: The test mode selector;
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void TestModeTest(uint8 mode_selector)
{
    mode_selector++;

    while (1)
    {
        ClearWatchdog();
    }
}

/*
 *********************************************************************************************************
 *                                           Set_HighSpeed_Mode
 *
 * Description: Enable High Speed mode
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : if call,please note that is bank function
 *********************************************************************************************************
 */
void Set_HighSpeed_Mode(void)
{
    //if is high speed,cancel high speed disable
    if ((BKDOOR & 0x40) != 0x00)
    {
        BKDOOR = (BKDOOR & 0x7f) | 0x10;
    }
}

#pragma ROM(HUGE)

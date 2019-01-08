/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : UDisk20SetupPktParsing.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-10 13:11
 *********************************************************************************************************
 */

#include "udisklib.h"

#pragma name(SETUPPKT_C)

#pragma ROM(HUGE)
void Set_Endpoint_PacketSize(void);
void SetSerialNumber(void);
void Cnvrt_ASCII_To_UNI16(uint8 *addr, uint8 length);
uint8 Count_String_CharLngth(uint8 *addr, uint8 length);
void Cnvrt_Hex_To_ASCII(uint8 *addr, uint8 length);
uint8 Hex_to_ASCII(uint8 hex_value);
#pragma ROM(HUGE)

#pragma renamecode(?PR?SETUPPKT_C)
#pragma ROM(HUGE)
/*
 *********************************************************************************************************
 *                                           Setup_Packet_Handle
 *
 * Description: SetupPacket Main Function Entry.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Setup_Packet_Handle(void)
{
    uint8 Request_Type, Request_Command;
    int Request_Value, Request_Index, Request_Length;

    ((uint8 *) &setup_paket_data)[0] = Setupdat0;
    ((uint8 *) &setup_paket_data)[1] = Setupdat1;
    ((uint8 *) &setup_paket_data)[2] = Setupdat3;
    ((uint8 *) &setup_paket_data)[3] = Setupdat2;
    ((uint8 *) &setup_paket_data)[4] = Setupdat5;
    ((uint8 *) &setup_paket_data)[5] = Setupdat4;
    ((uint8 *) &setup_paket_data)[6] = Setupdat7;
    ((uint8 *) &setup_paket_data)[7] = Setupdat6;

    //clear EP0 SETUP interrupt
    USBEIRQ = USBEIRQ | (uint8) 0x80;
    Usbirq_hcusbirq = Usbirq_hcusbirq | 0x01;

    Request_Type = (uint8) setup_paket_data.bmRequestType;
    Request_Command = (uint8) setup_paket_data.bRequest;
    Request_Value = setup_paket_data.wValue;
    Request_Index = setup_paket_data.wIndex;
    Request_Length = setup_paket_data.wLength;

    switch (Request_Command)
    {
    case USB_REQ_GET_STATUS:
        switch (Request_Type)
        {
        case USB_RECIP_DEVICE | USB_DIR_IN:
            Control_Read((char *) &DeviceStatus, sizeof(DeviceStatus), Request_Length);
            break;

        case USB_RECIP_INTERFACE | USB_DIR_IN:
            Control_Read((char *) &InterfaceStatus, sizeof(InterfaceStatus), Request_Length);
            break;

        case USB_RECIP_ENDPOINT | USB_DIR_IN:
            if (Request_Index == 0x81)
            {
                Control_Read((char *) &EndPoint1Status, sizeof(EndPoint1Status), Request_Length);
            }
            else if (Request_Index == 0x02)
            {
                Control_Read((char *) &EndPoint2Status, sizeof(EndPoint2Status), Request_Length);
            }
            //else if( Request_Index == 0x83 )
            //    Control_Read( (char *)&EndpointC_Status, sizeof(EndpointC_Status), Request_Length );
            else
            {
                Stall_Endpoint(ENDPOINT_0);
            }
            break;

        default:
            Stall_Endpoint(ENDPOINT_0);
            break;
        }
        break;

    case USB_REQ_CLEAR_FEATURE:
        if ((Request_Type == (USB_RECIP_ENDPOINT | USB_DIR_OUT)) && (Request_Value == USB_ENDPOINT_HALT))
        {
            if (Request_Index == 0x81)
            {
                EndPoint1Status = 0;
                //Flush FIFO,Clear STALL
                Recover_Stall_Endpoint(ENDPOINT_A);
                NoData_PhaseReturn();
            }
            else if (Request_Index == 0x02)
            {
                EndPoint2Status = 0;
                //Flush FIFO,Clear STALL
                Recover_Stall_Endpoint(ENDPOINT_B);
                NoData_PhaseReturn();
            }
            else
            {
                Stall_Endpoint(ENDPOINT_0);
            }
        }
        else
        {
            Stall_Endpoint(ENDPOINT_0);
        }
        break;

    case USB_REQ_SET_FEATURE:
        if ((Request_Type == (USB_RECIP_DEVICE | USB_DIR_OUT)) && (Request_Value == USB_DEVICE_TEST_MODE)
                && ((Request_Index & 0xFF) == 0))
        {
            NoData_PhaseReturn();
            //High byte of wIndex is the test mode descriptor
            TestModeTest((char) * (((char *) & (Request_Index)) + 1));
        }
        else if ((Request_Type == (USB_RECIP_ENDPOINT | USB_DIR_OUT)) && (Request_Value == USB_ENDPOINT_HALT))
        {
            if (Request_Index == 0x81)
            {
                EndPoint1Status = 1;
                //Flush FIFO,STALL EndpointA
                Stall_Endpoint(ENDPOINT_A);
                NoData_PhaseReturn();
            }
            else if (Request_Index == 0x02)
            {
                EndPoint2Status = 1;
                //Flush FIFO,STALL EndpointB,NAK OUT Mode
                Stall_Endpoint(ENDPOINT_B);
                NoData_PhaseReturn();
            }
            else
            {
                Stall_Endpoint(ENDPOINT_0);
            }
        }
        else
        {
            Stall_Endpoint(ENDPOINT_0);
        }
        break;

    case USB_REQ_SET_ADDRESS:
        NoData_PhaseReturn();
        break;

        //USB_REQ_GET_DESCRIPTOR:0x06
    case USB_REQ_GET_DESCRIPTOR:
        if (Request_Type != (USB_RECIP_DEVICE | USB_DIR_IN))
        {
            Stall_Endpoint(ENDPOINT_0);
            break;
        }

        //High byte of wValue is the descriptor num
        switch ((char) (Request_Value >> 8))
        {
        case USB_DT_DEVICE:
            Control_Read((char *) &HS_Device_Dscrptr, 0x12, Request_Length);
            break;

        case USB_DT_CONFIG:
            Set_Endpoint_PacketSize();
            Control_Read((char *) &HS_Config_Dscrptr, 0x20, Request_Length);
            Request_Length++;
            break;

        case USB_DT_STRING:
            if ((char) (Request_Value) == 0)
            {
                Control_Read((char *) &language_string, *((uint8 *) &language_string), Request_Length);
            }
            else if ((char) (Request_Value) == 1)
            {
                Control_Read((char *) &manufacure_string, *((uint8 *) &manufacure_string), Request_Length);
            }
            else if ((char) (Request_Value) == 2)
            {
                Control_Read((char *) &device_name_string, *((uint8 *) &device_name_string), Request_Length);
            }
            else if ((char) (Request_Value) == 3)
            {
                Control_Read((char *) &SerialNumber, *((uint8 *) &SerialNumber), Request_Length);
            }
            else
            {
                Stall_Endpoint(ENDPOINT_0);
            }
            break;

        case USB_DT_DEVICE_QUALIFIER:
            Control_Read((char *) &HS_Device_Qualifier_Dscrptr, 0x0a, Request_Length);
            break;

        case USB_DT_OTHER_SPEED_CONFIG:
            Control_Read((char *) &HS_OtherSpeed_Config_Dscrptr, 0x20, Request_Length);
            break;

        default:
            Stall_Endpoint(ENDPOINT_0);
            break;
        }
        break;

    case USB_REQ_GET_CONFIGURATION:
        if (Request_Type != (USB_RECIP_DEVICE | USB_DIR_IN))
        {
            Stall_Endpoint(ENDPOINT_0);
            break;
        }
        Control_Read((char *) &ConfigValue, sizeof(ConfigValue), Request_Length);
        break;

    case USB_REQ_SET_CONFIGURATION:
        usb_pwr_suspnd_fnsh++;
        //bit[7]:1 need process suspend,0 don't process suspend; bit[1:0]:0 no suspend; 1 recieve suspend; 2 has handle suspend
        Suspend_Flag = Suspend_Flag | 0x80;
        if ((Request_Type != (USB_RECIP_DEVICE | USB_DIR_OUT)) || (Request_Value > 1))
        {
            Stall_Endpoint(ENDPOINT_0);
            break;
        }
        NoData_PhaseReturn();
        ConfigValue = (char) Request_Value;
        Set_EPA_BulkInMode();
        Set_EPB_BulkOutMode();
        //set config后恢复高速模式,在HS传输中出错转FS会是导致传输中断
        Set_HighSpeed_Mode();
        break;

    case USB_REQ_GET_INTERFACE:
        if ((Request_Type != (USB_RECIP_INTERFACE | USB_DIR_IN)) || (Request_Index != 0))
        {
            Stall_Endpoint(ENDPOINT_0);
            break;
        }
        Control_Read((char *) &AlternateSetting, sizeof(AlternateSetting), Request_Length);
        break;

    case USB_REQ_SET_INTERFACE:
        if ((Request_Type != (USB_RECIP_INTERFACE | USB_DIR_OUT)) || (Request_Index != 0) || (Request_Value != 0))
        {
            Stall_Endpoint(ENDPOINT_0);
            break;
        }
        NoData_PhaseReturn();
        break;
    case BULK_ONLY_MASS_STORAGE_RESET:
        if ((Request_Type == 0x21) || (Request_Index == 0) || (Request_Value == 0) || (Request_Length == 0))
        {
            Rest_Endp_Toggle();
            NoData_PhaseReturn();
        }
        else
        {
            Stall_Endpoint(ENDPOINT_0);
        }
        break;
    case GET_MAX_LUN:
        if ((Request_Type == 0xa1) || (Request_Index == 0) || (Request_Value == 0) || (Request_Length == 1))
        {
            Control_Read((char *) &MaxLUN, sizeof(MaxLUN), Request_Length);
        }
        else
        {
            Stall_Endpoint(ENDPOINT_0);
        }
        break;
    default:
        //Not Support USB_REQ_SET_DESCRIPTOR, USB_REQ_SYNCH_FRAME.
        Stall_Endpoint(ENDPOINT_0);
        break;
    }
}

/*
 *********************************************************************************************************
 *                                           Set_Endpoint_PacketSize
 *
 * Description: This function set the maxpacket size for endpoint a & b.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    :
 *********************************************************************************************************
 */
void Set_Endpoint_PacketSize(void)
{
    int max_pkt_len, max_pkt_len_alter;

    ClearWatchdog();

    if ((BKDOOR & 0x40) != 0x00)
    {
        max_pkt_len = 0x0002; //0x200转为小端模式,High Speed
        max_pkt_len_alter = 0x4000; //0x40转为小端模式
    }
    else
    {
        max_pkt_len = 0x4000; //0x40转为小端模式,Full Speed
        max_pkt_len_alter = 0x0002; //0x200转为小端模式
    }

    HS_EndpointA_Dscrptr.wMaxPacketSize = max_pkt_len;
    HS_EndpointB_Dscrptr.wMaxPacketSize = max_pkt_len;
    HS_OtherSpeed_EndpointA_Dscrptr.wMaxPacketSize = max_pkt_len_alter;
    HS_OtherSpeed_EndpointB_Dscrptr.wMaxPacketSize = max_pkt_len_alter;

    if (has_set_usb_dscrptr == 0)
    {
        has_set_usb_dscrptr++;
        SetSerialNumber();
    }
}

/*
 *********************************************************************************************************
 *                                           SetSerialNumber
 *
 * Description: This function set the maxpacket size for endpoint a & b.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    :
 *********************************************************************************************************
 */

void SetSerialNumber(void)
{
    uint8 length;
    if ((HS_Device_Dscrptr.iSerialNumber != 0x00) && ((udisk_setting & 0x03) != 0x01))
    {
        GetUsbSetupInfo((int8 *) SerialNumber.wString);
        length = Count_String_CharLngth((int8 *) SerialNumber.wString, 0x30);
        if (length == 0x00)
        {
            HS_Device_Dscrptr.iSerialNumber = 0x00;
        }
        else
        {
            //serial number total length, including the head;
            SerialNumber.bLength = length * 4 + 2;
            // 1个hex数转换为2个ASCII,高4bit在前,低4bit在后;
            Cnvrt_Hex_To_ASCII((int8 *) SerialNumber.wString, length);
            //1个ASCII转换为2byte Unicode16
            Cnvrt_ASCII_To_UNI16((uint8 *) SerialNumber.wString, length * 2);

            SerialNumber.bDescriptorType = 0x03;
        }
    }

    ClearWatchdog();

    //Just for temp buffer,careful not cover sn
    GetUsbAttribute((usbattri_t *) device_name_string.wString);
    memcpy(manufacure_string.bSring, device_name_string.wString, 8);
    memcpy(device_name_string.wString, device_name_string.wString + 8, 16);

    length = Count_String_CharLngth((int8 *) manufacure_string.bSring, 8);
    Cnvrt_ASCII_To_UNI16((uint8 *) manufacure_string.bSring, length);
    manufacure_string.bLength = length * 2 + 2;
    manufacure_string.bDescriptorType = 0x03;

    length = Count_String_CharLngth((int8 *) device_name_string.wString, 16);
    Cnvrt_ASCII_To_UNI16((uint8 *) device_name_string.wString, length);
    device_name_string.bLength = length * 2 + 2;
    device_name_string.bDescriptorType = 0x03;
}

/*
 *********************************************************************************************************
 *                                           Cnvrt_ASCII_To_UNI16
 *
 * Description: Convert the string to unicode 16, one byte ASCII to two byte unicode, fill high byte with 0.
 *
 * Arguments  : addr: string address; length: maximun char length(should be not more than 255byte).
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Cnvrt_ASCII_To_UNI16(uint8 *addr, uint8 length)
{
    uint8 *data_p1;
    uint8 *data_p2;

    //数组的下标从0开始
    data_p1 = addr + (length - 1); //The last byte address
    data_p2 = addr + (length - 1) * 2 + 1; //the data address to be load

    //当ASCII码长度为0或1时作下面的转换会覆盖地址指向memory之前的数据
    while (length != 0x00)
    {
        *data_p2 = 0x00;
        data_p2--;
        *data_p2 = *data_p1; //Original ASCII char

        data_p1--;
        data_p2--;
        length--;
    }
}

/*
 *********************************************************************************************************
 *                                           Count_String_CharLngth
 *
 * Description: Calculate the string char length.
 *
 * Arguments  : Addr: string address; length: maximun char length.
 *
 * Returns    : string length(byte unit, maximun 255byte long).
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 Count_String_CharLngth(uint8 *addr, uint8 length)
{
    uint8 count = 0x00;
    //为0表示字符串已经到最后
    while (length != 0x00)
    {
        if (*addr == 0x00)
        {
            break;
        }
        count++;
        addr++;
        length--;
    }

    return count;
}

/*
 *********************************************************************************************************
 *                                           Cnvrt_Hex_To_ASCII
 *
 * Description: Convert Hex value to Ascii code.
 *
 * Arguments  : addr: hex value address; length: byte length.
 *
 * Returns    : ascii length(byte unit, maximun 255byte long).
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Cnvrt_Hex_To_ASCII(uint8 *addr, uint8 length)
{
    uint8 *data_p1;
    uint8 *data_p2;

    //数组的下标从0开始
    data_p1 = addr + (length - 1); //The last byte address
    data_p2 = addr + (length - 1) * 2 + 1; //the data address to be load

    while (length != 0x00)
    {
        //Original ASCII char
        *data_p2 = Hex_to_ASCII(*data_p1);
        data_p2--;
        *data_p2 = Hex_to_ASCII((*data_p1) >> 4);

        data_p1--;
        data_p2--;
        length--;
    }
}

/*
 *********************************************************************************************************
 *                                           Hex_to_ASCII
 *
 * Description: Convert 1byte Hex value to Ascii code.
 *
 * Arguments  : hex_value: hex value;
 *
 * Returns    : ascii code.
 *
 * Note(s)    : hex value is 4 bit, high 4 bits will be discard.
 *********************************************************************************************************
 */
uint8 Hex_to_ASCII(uint8 hex_value)
{
    uint8 assii_value;
    hex_value = hex_value & 0x0f;
    if (hex_value < 0x0a)
    {
        assii_value = hex_value + '0';
    }
    else
    {
        assii_value = hex_value + 'A' - 0x0a;
    }
    return assii_value;
}
#pragma ROM(HUGE)


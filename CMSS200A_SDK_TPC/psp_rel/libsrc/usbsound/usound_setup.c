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
 * \file     usound_setup.c
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               枚举代码
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 *
 *      Copyright(c) 2001-2012 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2012/10/23
 *******************************************************************************/
#include "actos.h"
#include "usound_setup.h"
#include "usound_intern.h"
#include "usound_extern.h"

#pragma name(USC_SETUP)
#pragma renamecode(?PR?USC_SETUP)

/******************************************************************************/
/*!
 * \par  Description:
 *     count string lenth.
 * \param[in]    pstraddr:point to string、strlgth:max lenth.
 * \param[out]   none.
 * \return       actual lenth

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static uint8 count_string_charlngth(uint8 *pstraddr, uint8 strlgth)
{
    unsigned char charcounter = 0x00;
    unsigned char i;
    for (i = 0; i < strlgth; i++)
    {
        if (*pstraddr == 0)
        {
            break;
        }
        charcounter++;
        pstraddr++;
    }
    return charcounter;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     convert ascII code to unique code.
 * \param[in]    pstraddr:point to string、strlgth:max lenth.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void cnvrt_ascii_to_uni16(uint8 *pstraddr, uint8 strlgth)
{
    uint8 *data_p1;
    uint8 *data_p2;

    //数组的下标从0开始
    data_p1 = pstraddr + (strlgth - 1); //The last byte address
    data_p2 = pstraddr + (strlgth - 1) * 2 + 1; //the data address to be load

    //当ASCII码长度为0或1时作下面的转换会覆盖地址指向memory之前的数据
    while (strlgth != 0x00)
    {
        *data_p2 = 0x00;
        data_p2--;
        *data_p2 = *data_p1; //Original ASCII char

        data_p1--;
        data_p2--;
        strlgth--;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *     data endian reverse.
 * \param[in]    pstraddr:point to data、strlgth:max lenth.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void endian_reverse(uint8 *data_p, uint16 data_length)
{
    char tmp;
    char *head_p, *end_p;
    head_p = data_p;
    end_p = data_p + data_length - 1;

    for (; head_p < end_p; ++head_p, --end_p)
    {
        tmp = *end_p;
        *end_p = *head_p;
        *head_p = tmp;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *     iso out token process.
 * \param[in]    pstraddr:point to string、strlgth:max lenth.
 * \param[out]   none.
 * \return       actual lenth

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void descriptor_init(usound_param *pud_param)
{
    uint8 string_len;

    string_len = count_string_charlngth((uint8 *) &manufacure_string.wManufacturerName, sizeof(manufacure_string.wManufacturerName)/2);
    manufacure_string.bLength = 0x02 + string_len * 2;
    manufacure_string.bDescriptorType = 0x03;
    cnvrt_ascii_to_uni16((uint8 *) &manufacure_string.wManufacturerName, string_len);

    string_len = count_string_charlngth((uint8 *) &device_name_string.wProductName, sizeof(device_name_string.wProductName)/2);
    device_name_string.bLength = 0x02 + string_len * 2;
    device_name_string.bDescriptorType = 0x03;
    cnvrt_ascii_to_uni16((uint8 *) &device_name_string.wProductName, string_len);

    string_len = count_string_charlngth((uint8 *) &SerialNumber.wManufacturerName, sizeof(SerialNumber.wManufacturerName)/2);
    SerialNumber.bLength = 0x02 + string_len * 2;
    SerialNumber.bDescriptorType = 0x03;
    cnvrt_ascii_to_uni16((uint8 *) &SerialNumber.wManufacturerName, string_len);
    
    //memcpy(All_Descriptors_of_Configuration.Audio_Class_Format_Setting1.tSamFreq, sample_rate_arr[pud_param->record_sample_rate], 3);
    memcpy(All_Descriptors_of_Configuration.Audio_Class_Format_Setting1.tSamFreq, sample_rate_arr[pud_param->play_sample_rate], 3);
    memcpy(All_Descriptors_of_Configuration.Audio_Class_Format_Setting2.tSamFreq, sample_rate_arr[pud_param->record_sample_rate], 3);

   if (DOWNLOAD_BIT_DEPTH_24== pud_param->bit_depth)
    {  
        All_Descriptors_of_Configuration.Endpoint_Descriptor_Setting1.wMaxPacketSize = 0x4002;//0x2001; //0x320;
        All_Descriptors_of_Configuration.Audio_Class_Format_Setting1.bSubFrameSize = 0x03;//0x04;//0x03;
        All_Descriptors_of_Configuration.Audio_Class_Format_Setting1.bBitResolution = 0x18;
    }  
   else
    {
        //All_Descriptors_of_Configuration.Endpoint_Descriptor_Setting1.wMaxPacketSize = 0x8001;//0x2001; //0x320;;
		;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *     set endp0 ACK bit.
 * \param[in]    none.
 * \param[out]   none.
 * \return       actual lenth

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void no_data_control(void)
{
    uint8 sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    Ep0cs_hcep0cs |= 0x02;
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     read endp0 data.
 * \param[in]    data_address:point to string、data_length:actual lenth、request_length：request length.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void control_write(uint8 *data_address, uint8 data_length, uint8 request_length)
{
    uint8 sfr_temp;
    uint8 transfer_length, temp_count, ret_var;
    uint8 *p_temp;

    sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    /* if length of data to be received is more than PC sent, only send data length that pc sent.  */
    if (data_length <= request_length)
    {
        transfer_length = data_length;
    }
    else
    {
        transfer_length = request_length;
    }
    p_temp = data_address;

    Out0bc_hcin0bc = 0x00;
    /* wait transfer complete. */
    while (1)
    {
        /* check if line is pulled out.  */
        ret_var = is_usb_connect();
        if (USB_IS_CONNECT != ret_var)
        {
            break;
        }

        /* check if busy.   */
        if (0x00 == (Ep0cs_hcep0cs & 0x08))
        {
            break;
        }
    }/* while(1) */

    if (ret_var == USB_IS_CONNECT)
    {
        for (temp_count = 0; temp_count < transfer_length; temp_count++)
        {
            *p_temp++ = Ep0outdata;
        }
    }
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     write endp0 data.
 * \param[in]    data_address:point to string、actul_dlen:actual lenth、rqst_dlen：request length.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void control_read(uint8 *d_addr, uint8 actul_dlen, uint8 rqst_dlen)
{
    uint8 translen_sum;
    uint8 translen_once;
    uint8 i;
    uint8 sfr_temp;
    //calucate data length of this control read
    sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    if (rqst_dlen == 0x00)
    {
        translen_sum = actul_dlen;
    }
    else if (rqst_dlen > actul_dlen)
    {
        translen_sum = actul_dlen;
    }
    else
    {
        translen_sum = rqst_dlen;
    }

    //transferring
    while (translen_sum != 0)
    {
        if (translen_sum > EP0_MAX_PACKET_SIZE)
        {
            translen_once = EP0_MAX_PACKET_SIZE;
            translen_sum = translen_sum - EP0_MAX_PACKET_SIZE;
        }
        else
        {
            translen_once = translen_sum;
            translen_sum = 0x00;
        }
        for (i = 0; i < translen_once; i++)
        {
            Ep0indata = *d_addr++;
        }
        //wtite in0 conter,it will start packet transfer and set in0busy
        In0bc_hcout0bc = translen_once;
        //wait transfer end,Inbsy_hcoutbsy = b'0'
        while ((Ep0cs_hcep0cs & 0x04) != 0x00)
        {
            if (USB_IS_CONNECT != is_usb_connect())
            {
                break;
            }
        }
    }
    //hsnak clear; when stall bit is 0,it will ack in status stage
    no_data_control();
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     write endp0 data.
 * \param[in]    ep_num:endp NO..
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void stall_endpoint(uint8 ep_num)
{
    uint8 sfr_temp;
    sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    switch (ep_num)
    {
    case EP0:
        Ep0cs_hcep0cs |= 0x01;
        break;
    case EP1:
        In1ctrl_hcout1ctrl |= 0x40;
        break;
    case EP2:
        Out2ctrl_hcin2ctrl |= 0x40;
        break;
    case EP3:
        In3ctrl_hcout3ctrl |= 0x40;
        break;
    default:
        break;
    }
    SFR_BANK = sfr_temp;
    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     recover stall endpoint.
 * \param[in]    ep_num:endp NO..
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void recover_stall_endpoint(uint8 ep_num)
{
    uint8 sfr_temp;
    sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    switch (ep_num)
    {
    case EP0:
        Ep0cs_hcep0cs &= ~0x01;
        break;
    case EP1:
        In1ctrl_hcout1ctrl &= ~0x40;
        break;
    case EP2:
        Out2ctrl_hcin2ctrl &= ~0x40;
        break;
    case EP3:
        In3ctrl_hcout3ctrl &= ~0x40;
        break;
    default:
        break;
    }
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     init out endpoint.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void set_out_iso_ep(void)
{
    uint8 sfr_temp;
    sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    if (DOWNLOAD_BIT_DEPTH_24== gud_param.bit_depth)
    {       
        Hcin2maxpckl = 0x40;//0x20;
        Hcin2maxpckh = 0x02; //0x01;
        //EP2STADDRH = 0x03;//0x03;  //uram+fir_rds +pcm
        //EP2STADDRL = 0x80;//0x80;
    }
    else
    {
        Hcin2maxpckl = 0xc0; //0x80;
        Hcin2maxpckh = 0x00; //0x01;
        //EP2STADDRH = 0x03;//0x03;   //uram+fir_rds +pcm
        //EP2STADDRL = 0x80;//0x80;
    }
    EP2STADDRH = 0x03;//uram+fir_rds +pcm = 0x200 + 0xc0*2 + 0x1000
    EP2STADDRL = 0x80;
    Out2ctrl_hcin2ctrl = 0x84; //single buff、iso ep 192*1

    EPRST = 0x02;
    EPRST = 0x22;
    EPRST = 0x42;

    Fifoctrl |= 0x22;
    SFR_BANK = sfr_temp;
}
/******************************************************************************/
/*!
 * \par  Description:
 *     init in endpoint.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void set_in_iso_ep(void)
{
    uint8 sfr_temp;
    sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    Hcout1maxpckh = 0x00;
    Hcout1maxpckl = 0x64; //100 byte

    //不能从0x0000地址开始，因为那个地址被配置成了EP0,EP3 fifo
    EP1STADDRH = 0x00;
    EP1STADDRL = 0xc0;
    In1ctrl_hcout1ctrl = 0x86; //triple buff、iso ep 320

    EPRST = 0x01;
    EPRST = 0x21;
    EPRST = 0x41;

    Fifoctrl |= 0x21;
    SFR_BANK = sfr_temp;

}

/******************************************************************************/
/*!
 * \par  Description:
 *     init interrupt in endpoint.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
static void set_in_int_ep(void)
{
    uint8 sfr_temp;
    sfr_temp = SFR_BANK;
    SFR_BANK = BANK_USB;
    Hcout3maxpck = 0x04; //4 byte
    In3ctrl_hcout3ctrl = 0x8c; // int ep

    EPRST = 0x03;
    EPRST = 0x23;
    EPRST = 0x43;

    Fifoctrl |= 0x23;
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     设置PA音量.
 * \param[in]    vol_val.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void set_pa_vol(uint8 vol_val)
{
    uint8 sfr_temp = SFR_BANK;
    SFR_BANK = BANK_AUIP;
    if (vol_val == 0)
    {
        DAC_ANALOG0 &= 0xfe;
    }
    else
    {
        DAC_ANALOG0 |= 0x01;
    }
    PA_VOLUME = (PA_VOLUME & 0xf8) | (vol_val & 0x07);
    SFR_BANK = sfr_temp;
}

/******************************************************************************/
/*!
 * \par  Description:
 *     设置ADC op1 增益.
 * \param[in]    vol_gain.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
#if 0
void set_adc_gain(uint8 vol_gain)
{
    uint8 sfr_temp = SFR_BANK;
    SFR_BANK = BANK_AUIP;
   // ADC_GAIN1 = (ADC_GAIN1 & 0xf8) | (vol_gain & 0x07);
    SFR_BANK = sfr_temp;
}
#endif
/******************************************************************************/
/*!
 * \par  Description:
 *     setup process.
 * \param[in]    none.
 * \param[out]   none.
 * \return       none

 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 *******************************************************************************/
void deal_setup(void)
{
    uint8 sfr_temp = SFR_BANK;
    uint8 temp_count;
    uint8 *p_temp;
    uint32 new_sample;
	uint8 test_temp[4] = {0x00, 0x00, 0x00, 0x00};
	
    SFR_BANK = BANK_USB;
    p_temp = &setup_data.bmRequestType;

    ((uint8 *) &setup_data)[0] = Setupdat0; //get setup data.
    ((uint8 *) &setup_data)[1] = Setupdat1;
    ((uint8 *) &setup_data)[2] = Setupdat3;
    ((uint8 *) &setup_data)[3] = Setupdat2;
    ((uint8 *) &setup_data)[4] = Setupdat5;
    ((uint8 *) &setup_data)[5] = Setupdat4;
    ((uint8 *) &setup_data)[6] = Setupdat7;
    ((uint8 *) &setup_data)[7] = Setupdat6;

    switch (setup_data.bmRequestType)
        //judge type of device request.
    {
    case RD_STD_DEV:
        if (setup_data.bRequest == GET_DESCRIPTOR)
        {
            if (setup_data.wValue == VALUE_DEVICE) //get device descriptor.
            {
                control_read(&Device_Descriptor.bLength, Device_Descriptor.bLength, (uint8) setup_data.wLength);
            }
            else if (setup_data.wValue == VALUE_CONFIG) //get device configuration.
            {
                control_read(&All_Descriptors_of_Configuration.Configuration_Descriptor.bLength,
                             (uint8) (All_Descriptors_of_Configuration.Configuration_Descriptor.wTotalLength >> 8),
                             (uint8) setup_data.wLength);
            }
            else if (setup_data.wValue == VALUE_STRING_LANGID) // get string of language ID.
            {
                control_read(&language_string.bLength, language_string.bLength, (uint8) setup_data.wLength);
            }
            else if (setup_data.wValue == VALUE_STRING_MANUFACTURER) // get string of manufaturer.
            {
                control_read(&manufacure_string.bLength, manufacure_string.bLength, (uint8) setup_data.wLength);
            }
            else if (setup_data.wValue == VALUE_STRING_PRODUCT) //get string of product.
            {
                control_read(&device_name_string.bLength, device_name_string.bLength, (uint8) setup_data.wLength);
            }
            else if (setup_data.wValue == VALUE_STRING_SERIAL_NUMBER) //get string of serial number.
            {
                control_read(&SerialNumber.bLength, SerialNumber.bLength, (uint8) setup_data.wLength);
            }
            else //others error.
            {
                no_data_control();
            }
        }
        else
        {
            stall_endpoint(EP0);
        }

        break;

    case RD_STD_IF: //get Descriptor.
        if (setup_data.bRequest == GET_DESCRIPTOR)
        {
            if (setup_data.wIndex == INDEX_INTERFACE3)
            {
                control_read(ReportDescriptor, sizeof(ReportDescriptor), (uint8) setup_data.wLength);
            }
        }
        break;

    case WR_STD_DEV: //set address.
        if (setup_data.bRequest == SET_ADDRESS)
        {
            no_data_control();
        }
        else if (setup_data.bRequest == SET_CONFIGURATION) //set configuration.
        {
//#ifdef _USB_RECORD_
            set_in_iso_ep();
//#endif
            set_out_iso_ep();
            set_in_int_ep();
            no_data_control();
        }
        else
        {
            stall_endpoint(EP0);
        }
        break;

    case WR_STD_IF: //set interface.
        if (setup_data.bRequest == SET_INTERFACE)
        {
            if (setup_data.wIndex == INDEX_INTERFACE1)
            {
                if (setup_data.wValue == VALUE_ALTERNATE_SETTING0) //setting 0 means that zero-bandwidth.
                {
                    usb_state &= (~PLAY_RUNNING);
                    SFR_BANK = BANK_DMA_USB1;
                    DMAnCTL0 &= 0xfe; //stop dma
                    SFR_BANK = BANK_USB;
                    start_play_flag = 0;
                }
                else if (setup_data.wValue == VALUE_ALTERNATE_SETTING1)
                {
                    //usb_state |= PLAY_RUNNING;
                    wr_play_buff_ptr = 0;
                    play_data_count = 0;
                }
            }
            else if (setup_data.wIndex == INDEX_INTERFACE2)
            {
                if (setup_data.wValue == VALUE_ALTERNATE_SETTING0)
                {
                    usb_state &= (~RECORD_RUNNING);
                    SFR_BANK = BANK_DMA_USB2;
                    DMAnCTL0 &= 0xfe; //stop dma
                    SFR_BANK = BANK_USB;
                    start_record_flag = 0;
                }
                else if (setup_data.wValue == VALUE_ALTERNATE_SETTING1)
                {
                    usb_state |= RECORD_RUNNING;
                    rd_record_buff_ptr = 0;
                    record_data_count = 0;
                    start_record_flag = 0;
                }
            }

            no_data_control();
        }
        else
        {
            stall_endpoint(EP0);
        }
        break;

    case SET_UNIT_CONTROL: //set unit
        if (setup_data.wIndex == FEATURE_UNIT_INDEX1)
        {
            if (setup_data.bRequest == SET_CUR)
            {
                temp_count = VOLUME_LENGTH;
                if (setup_data.wValue == ((MUTE_CONTROL << 8) | MAIN_CHANNEL_NUMBER0))
                {
                    p_temp = &play_mute_ch0;
                    temp_count = MUTE_LENGTH;
                }
                else if (setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0))
                {
                    p_temp = (uint8 *) &play_vol_ch0;
                }
                else
                {
                    ;
                }

                control_write(p_temp, temp_count, (uint8) setup_data.wLength);
                endian_reverse(p_temp, temp_count);
                no_data_control();

                if (play_mute_ch0 == 0x01)
                {
                    //set_pa_vol(0);
                }
                else
                {
                 //   set_pa_vol((uint8)(play_vol_ch0 >> 12));
					cur_vol_index = (uint8)(play_vol_ch0/PLAY_RESOTION_VOLUME);
					vol_change_flag = 1;
                }
                break;
            }
			else if (setup_data.bRequest == SET_RES)
            {
				temp_count = (uint8) setup_data.wLength;
				control_write(test_temp, temp_count, (uint8) setup_data.wLength);
                stall_endpoint(EP0);
				break;
                //no_data_control();
			}
        }
        else if (setup_data.wIndex == FEATURE_UNIT_INDEX2)
        {
            if (setup_data.bRequest == SET_CUR)
            {
                temp_count = VOLUME_LENGTH;
                if (setup_data.wValue == ((MUTE_CONTROL << 8) | MAIN_CHANNEL_NUMBER0))
                {
                    p_temp = &record_mute_ch0;
                    temp_count = MUTE_LENGTH;
                }
                else if (setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0))
                {
                    p_temp = (uint8 *) &record_vol_ch0;
                }
                else
                {
                    ;
                }

                control_write(p_temp, temp_count, (uint8) setup_data.wLength);
                endian_reverse(p_temp, temp_count);
                no_data_control();
#if 0
                if (record_mute_ch0 == 0x01)
                {
                    set_adc_gain(0);
                }
                else
                {
                    set_adc_gain((uint8)(record_vol_ch0 >> 12));
                }
#endif
				break;
            }
			else if (setup_data.bRequest == SET_RES)
            {
            	temp_count = (uint8) setup_data.wLength;
				control_write(test_temp, temp_count, (uint8) setup_data.wLength);
                stall_endpoint(EP0);
                break;
				//no_data_control();
			}
        }
        else if (setup_data.wIndex == SELECTOR_UNIT_INDEX)
        {
            if (setup_data.bRequest == SET_CUR)
            {
                temp_count = 0x01;
                p_temp = (uint8 *) &cur_selector;
                control_write(p_temp, temp_count, (uint8) setup_data.wLength);
            }
        }
        no_data_control();
        break;

    case GET_UNIT_CONTROL: //get unit
        temp_count = VOLUME_LENGTH;
        p_temp = (uint8 *) &volume_value;
        if (setup_data.wIndex == FEATURE_UNIT_INDEX1)
        {
            if (setup_data.bRequest == GET_CUR)
            {
                if (setup_data.wValue == ((MUTE_CONTROL << 8) | MAIN_CHANNEL_NUMBER0))
                {
                    p_temp = &play_mute_ch0;
                    temp_count = MUTE_LENGTH;
                }
                else if (setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0))
                {
                    p_temp = (uint8 *) &play_vol_ch0;
                }
                else
                {
                    ;
                }
            }
            else if (setup_data.bRequest == GET_MIN)
            {
                volume_value = PLAY_MINIMUM_VOLUME;
            }
            else if (setup_data.bRequest == GET_MAX)
            {
                volume_value = PLAY_MAXIMUM_VOLUME;
            }
            else if (setup_data.bRequest == GET_RES)
            {
                volume_value = PLAY_RESOTION_VOLUME;
            }
            else
            {
                ;
            }

            endian_reverse(p_temp, temp_count);
            control_read(p_temp, temp_count, (uint8) setup_data.wLength);
            endian_reverse(p_temp, temp_count);
        }
        else if (setup_data.wIndex == FEATURE_UNIT_INDEX2)
        {
            if (setup_data.bRequest == GET_CUR)
            {
                if (setup_data.wValue == ((MUTE_CONTROL << 8) | MAIN_CHANNEL_NUMBER0))
                {
                    p_temp = &record_mute_ch0;
                    temp_count = MUTE_LENGTH;
                }
                else if (setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0))
                {
                    p_temp = (uint8 *) &record_vol_ch0;
                }
                else
                {
                    ;
                }
            }
            else if (setup_data.bRequest == GET_MIN) //get minimum volume of main channel 0,1,2.
            {
                volume_value = RECORD_MINIMUM_VOLUME;
            }
            else if (setup_data.bRequest == GET_MAX) //get maximum volume of main channel 0,1,2.
            {
                volume_value = RECORD_MAXIMUM_VOLUME;
            }
            else if (setup_data.bRequest == GET_RES) //get resolution volume of main channel 0,1,2.
            {
                volume_value = RECORD_RESOTION_VOLUME;
            }
            else
            {
                ;
            }

            endian_reverse(p_temp, temp_count);
            control_read(p_temp, temp_count, (uint8) setup_data.wLength);
            endian_reverse(p_temp, temp_count);
        }
        else if (setup_data.wIndex == SELECTOR_UNIT_INDEX)
        {
            if (setup_data.bRequest == GET_CUR)
            {
                temp_count = 0x01;
                p_temp = (uint8 *) &cur_selector;
                control_read(p_temp, temp_count, (uint8) setup_data.wLength);
            }
        }
		else if (setup_data.wIndex == 0x0003)
		{
			if (setup_data.bRequest == SET_CUR)
			{
				temp_count = (uint8) setup_data.wLength;
                control_read(test_temp, temp_count, (uint8) setup_data.wLength);         
			}
		}
        else
		{
		}		
        break;

    case SET_ENDPOINT_CONTROL: //set endpoint
        if (setup_data.wValue == SAMPLING_FREQ_CONTROL)
        {
            if (setup_data.bRequest == SET_CUR)
            {
                new_sample = 0x00000000;
                temp_count = SAMPLE_FREQUENCY_LENGTH;
                p_temp = (uint8 *) &new_sample;

                control_write(p_temp, temp_count, (uint8) setup_data.wLength);
                no_data_control();
                endian_reverse(p_temp, temp_count);

                if (new_sample != current_sample)
                {
                    current_sample = new_sample;
                }

                //if(pa_mute == 0x00)
                //{
                //    SFR_BANK = BANK_AUIP;
                //    PA_CTL = PA_CTL | 0x10;
                //    SFR_BANK = BANK_USB;
                //
                //    pa_mute = 0x01;
                //}
            }
            else
            {
                ;
            }
        }
        break;
		
	case GET_ENDPOINT_CONTROL: 
		if (setup_data.wValue == SAMPLING_FREQ_CONTROL)
        {
	        if (setup_data.bRequest == GET_CUR)
			{
				temp_count = (uint8) setup_data.wLength;
				p_temp = (uint8 *) &current_sample;
				endian_reverse(p_temp, temp_count);
	            control_read(p_temp, temp_count, (uint8) setup_data.wLength);
	            endian_reverse(p_temp, temp_count);
			}
		}
		no_data_control();//可能会发set clearfeature到ep3in
		break;
	
    default:
        no_data_control();//可能会发set clearfeature到ep3in
        break;
    }
    SFR_BANK = sfr_temp;
}


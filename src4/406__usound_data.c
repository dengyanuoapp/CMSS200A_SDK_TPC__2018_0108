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
 * \file     usound_data.c
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               定义全局变量
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 *
 *      Copyright(c) 2001-2012 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2012/10/23
 *******************************************************************************/
#include "usound_setup.h"
#include "usound_extern.h"

#pragma userclass(near=USD_RDATA)
#pragma ROM(HUGE)
//uint8 pa_mute = 0;
uint8 usb_state = 0, play_mute_ch0 = 0, record_mute_ch0 = 0;
uint8 cur_selector = 0;
device_requests_t setup_data;
uint16 volume_value = 0, play_vol_ch0 = 0x6000, record_vol_ch0 = 0x4000; /*, current_volume_ch1 = 0, current_volume_ch2 = 0*/
uint32 current_sample = 0;
uint8 is_key_msg = 0x00;
uint32 wr_play_buff_ptr = 0;
int16 play_data_count = 0;
uint32 rd_record_buff_ptr = 0;
int16 record_data_count = 0;
uint32 usb_isr = 0;
uint32 dma_adc_isr = 0;
uint32 dma_dac_isr = 0;
uint8 g_PAOpenFlag = 0;
uint8 start_play_flag = 0;
uint8 start_record_flag = 0;
uint8 muram_addr = 0;
uint8 hid_send_flag = 0;
uint8 hid_report = 0;
uint8 mcu_pll_ctl_bak = 0;
uint8 cpuclkctl_bak = 0;
uint8 pa_volume_bak = 0;
uint8 cur_vol_index;
uint8 vol_change_flag;
uint8 g_record_packet_size = 32; //默认是16K 采样率上传的:16x2
usound_param gud_param = {0};

uint8 sample_rate_arr[4][3] = 
{
    {0x80, 0x3E, 0x00},       //16K
    {0x44, 0xAC, 0x00},       //44.1K
    {0x80, 0xBB, 0x00},       //48K
    {0x00, 0x77, 0x01},      //96k
};

uint8 ReportDescriptor[0x21] =
{
    0x05, 0x0c, //USAGE_PAGE (Consumer)
    0x09, 0x01, //USAGE (Consumer Control)
    0xa1, 0x01, //COLLECTION (Application)
    0x15, 0x00, //Logical Minimum (0x00)
    0x25, 0x01, //Logical Maximum (0x01)
    0x09, 0xe9, //USAGE (Volume Up)
    0x09, 0xea, //USAGE (Volume Down)
    0x09, 0xe2, //USAGE (Mute)
    0x09, 0xcd, //USAGE (Play/Pause)
    0x09, 0xb5, //USAGE (Scan Next Track)
    0x09, 0xb6, //USAGE (Scan Previous Track)
    0x09, 0xb3, //USAGE (Fast Forward)
    0x09, 0xb4, //USAGE (Rewind)
    0x75, 0x01, //Report Size (0x01)
    0x95, 0x08, //Report Count (0x08),报告的个数为8，即总共有8个bits
    0x81, 0x42, //Input (Data,Variable,Absolute,No Wrap,Linear,Preferred State,Null state)
    0xc0, //END_COLLECTION
};

device_descriptor_t Device_Descriptor =
{
    sizeof(device_descriptor_t), //size of descriptor
    DEVICE, //descriptor type
    0x0001, //USB release version 2.0
    0x00, //class code
    0x00, //sub-class code
    0x00, //protocol code
    0x40, //max packet size = 64byte
#if 0
    0xd610, //vendor ID
#else
	0x0020, //vendor ID
#endif
    0x00dd, //product ID
    0x1611, //Firmware release number, DDMM
    0x01, //index of string descriptor of manufacturer
    0x02, //index of string descriptor of product
    0x03, //index of string descriptor of serial number
    0x01, //number of possible configuration
};

//-----------------------------------------------------------
//start of Configuration
//-----------------------------------------------------------
device_audio_configuration_t All_Descriptors_of_Configuration =
{

    /*device_config_descriptor_t Configuration_Descriptor =*/
    {
        sizeof(device_config_descriptor_t), //size of descriptor
        CONFIG, //descriptor type
        sizeof(device_audio_configuration_t) << 8, //total length
#ifdef _USB_RECORD_
        0x04, //number of interface = 3,for audio class
#else
		0x03,
#endif
        0x01, //configuration value
        0x00, //configuration string index

        /*D7 Reserved, set to 1. (USB 1.0 Bus Powered)
          D6 Self Powered
          D5 Remote Wakeup
          D4..0 Reserved, set to 0.
          */
#if 0 
        0x80,//0xC0, //attribute (bus powered, remote wakeup disable)
        0xE0, //bus powered, self powered, remote wakeup enabled)
#else
        0x60, //self powered, remote wakeup enabled)
#endif

#if 0 
        0x32,//0x96, //max power (500mA),96h(300mA)
        0xfa,//max power (500mA)
#else
        0x00,//Not applicable.
#endif
    },

    /*usb_interface_descriptor_t Interface0_Descriptor =*/
    {
        sizeof(usb_interface_descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        0x00, //interface number,
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x01, //interface sub-class code, AUDIOCONTROL Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Audio_Class_Head_t Audio_Class_Head =*/
    {
        sizeof(Audio_Class_Head_t), 
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //HEADER descriptor subtype.
        0x0001, //Audio Device compliant to the USB Audio specification version 1.00.
#ifdef _USB_RECORD_
        (sizeof(Audio_Class_Head_t) + (sizeof(Audio_Class_Input_t) * 2) + (sizeof(Audio_Class_Output_t) * 2)
        + sizeof(Audio_Class_Selector_t) + sizeof(Audio_Class_Feature1_t)
        + sizeof(Audio_Class_Feature2_t)) << 8, //total length.
        0x02, //The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface Collection to which this AudioControl interface belongs.
        0x01, //interface number of first AudioStreaming or MIDIStreaming interface in the collection.
        0x02, //interface number of second AudioStreaming or MIDIStreaming interface in the collection.
#else
		(sizeof(Audio_Class_Head_t) + sizeof(Audio_Class_Input_t) + sizeof(Audio_Class_Output_t)
        + sizeof(Audio_Class_Feature1_t)) << 8,  //total length.
        0x01, //The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface Collection to which this AudioControl interface belongs.
        0x01, //interface number of first AudioStreaming or MIDIStreaming interface in the collection.
        //0x02, //interface number of second AudioStreaming or MIDIStreaming interface in the collection.
#endif
    },

    /*Audio_Class_Input_t Audio_Class_Input1 =*/
    {
        sizeof(Audio_Class_Input_t), 
        0x24, //descriptor type, CS_INTERFACE .
        0x02, //INPUT_TERMINAL  descriptor subtype.
        0x01, //Terminal ID.
        0x0101, //Terminal Type. USB streaming
        0x00, //This Input Terminal has no association.
        0x02, //This Terminal's output audio channel cluster has 2 logical output channels.
        0x0300, //Channel Config bitmap.
        0x00, //Channel Name.
        0x00, //Terminal name.
    },
#ifdef _USB_RECORD_
    /*Audio_Class_Input_t Audio_Class_Input2 =*/
    {
        sizeof(Audio_Class_Input_t), 0x24, //descriptor type, CS_INTERFACE .
        0x02, //INPUT_TERMINAL  descriptor subtype.
        0x02, //Terminal ID.
        0x0102, //Terminal Type.Microphone
        0x00, //This Input Terminal has no association.
        0x01, //This Terminal's output audio channel cluster has 2 logical output channels.
        0x0100, //Channel Config bitmap.
        0x00, //Channel Name.
        0x00, //Terminal name.
    },
#endif
    /*Audio_Class_Output_t Audio_Class_Output1 =*/
    {
        sizeof(Audio_Class_Output_t), 
        0x24, //descriptor type, CS_INTERFACE .
        0x03, //OUTPUT_TERMINAL  descriptor subtype.
        0x06, //Terminal ID.
        0x0103, //Terminal Type.Speaker
        0x00, //This Output Terminal has no association.
        0x09, //ID of the Unit or Terminal to which this Terminal is connected.
        0x00, //Terminal name.
    },
#ifdef _USB_RECORD_
    /*Audio_Class_Output_t Audio_Class_Output2 =*/
    {
        sizeof(Audio_Class_Output_t), 0x24, //descriptor type, CS_INTERFACE .
        0x03, //OUTPUT_TERMINAL  descriptor subtype.
        0x07, //Terminal ID.
        0x0101, //Terminal Type. USB streaming
        0x00, //This Output Terminal has no association.
        SELECTOR_UNIT_ID, //ID of the Unit or Terminal to which this Terminal is connected.
        0x00, //Terminal name.
    },
    
    /*Audio_Class_Seclector_t Audio_Class_Selector =*/
    {
        sizeof(Audio_Class_Selector_t), 0x24, //CS_INTERFACE descriptor type.
        0x05, //SELECTOR_UNIT descriptor subtype.
        SELECTOR_UNIT_ID, //SelectorID
        0x01, //Number of Input Pins of this Unit:p
        0x0a, //bSourceID
        0X00, //index of a string descriptor,describing the select unit
    },
#endif
    /*Audio_Class_Feature1_t Audio_Class_Feature1 =*/
    {
        sizeof(Audio_Class_Feature1_t), 
        0x24, //descriptor type, CS_INTERFACE .
        0x06, //FEATURE_UNIT descriptor subtype.
        FEATURE_UNIT_ID1, //Unit ID.
        0x01, //ID of the Unit or Terminal to which this Feature Unit is connected.
        0x01, //Size in bytes of an element of the bmaControls() array.
        0x03, //bmaControls(0), bitmap of master channel 0.
        0x00, //bmaControls(1), bitmap of master channel 1.
        0x00, //bmaControls(2), bitmap of master channel 2.
        0x00, //Feature name.
    },
#ifdef _USB_RECORD_
    /*Audio_Class_Feature2_t Audio_Class_Feature2 =*/
    {
        sizeof(Audio_Class_Feature2_t), 0x24, //descriptor type, CS_INTERFACE .
        0x06, //FEATURE_UNIT descriptor subtype.
        FEATURE_UNIT_ID2, //Unit ID.
        0x02, //ID of the Unit or Terminal to which this Feature Unit is connected.
        0x01, //Size in bytes of an element of the bmaControls() array.
        0x03, //bmaControls(0), bitmap of master channel 0.
        0x00, //bmaControls(1), bitmap of master channel 1.
        0x00, //Feature name.
    },
#endif
    /*Standard_AS_Interface_Descriptor_t Interface1_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        0x01, //interface value
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_AS_Interface_Descriptor_t Interface1_Setting1 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        0x01, //interface value
        0x01, //alternative setting
        0x01, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*ClassSpecific_AS_Interface_Descriptor_t Audio_Class_General_Setting1 =*/
    {
        sizeof(ClassSpecific_AS_Interface_Descriptor_t), //size of descriptor
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //AS_GENERAL descriptor subtype.
        0x01, //The Terminal ID of the Terminal to which the endpoint of this interface is connected.
        0x01, //Delay introduced by the data path. Expressed in number of frames.
        0x0100, //Format Tag, PCM.
    },

    /*Format_Type_Descriptor_t Audio_Class_Format_Setting1 =*/
    {
        sizeof(Format_Type_Descriptor_t), 0x24, //descriptor type, CS_INTERFACE .
        0x02, //FORMAT_TYPE descriptor subtype.
        0x01, //FormatType, FORMAT_TYPE .
        0x02, //bNrChannels, Indicates the number of physical channels in the audio data stream.
#ifdef BIT_RESOLUTION_16
        0x02, //bSubframeSize, The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.
        0x10, //bBitResolution, The number of effectively used bits from the available bits in an audio subframe.
#endif
#ifdef BIT_RESOLUTION_24
        0x03, //bSubframeSize, The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.
        0x18, //bBitResolution, The number of effectively used bits from the available bits in an audio subframe.
#endif
        0x01, //bSamFreqType, Indicates how the sampling frequency can be programmed.
        //{ 0x80, 0xbb, 0x00, }, //22.05K, Sampling frequency 1 in Hz for this isochronous data endpoint.
        //{0x40, 0x1F, 0x00,},       //8K
        //{0xE0, 0x2E, 0x00,},       //12K
        //{0x80, 0x3E, 0x00,},       //16K
        //{0xC0, 0x5D, 0x00,},       //24K
        //{0x00, 0x7D, 0x00,},       //32K
        //{0x44, 0xAC, 0x00,},       //44.1K
        //{0x80, 0xBB, 0x00,},       //48K
        {0x00, 0x00, 0x00,}, 
    },

    /*Standard_Endpiont_Descriptor_t Endpoint_Descriptor_Setting1 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x02, //endpoint address (out endpoint addressed at 2)
        0x09, //endpoint type (ISOCHRONOUS, adaptive)
        ISO_OUT_MAX_PACKET_SIZE << 8, //max packet size (192 bytes)
        0x01, //The polling interval value is every 1 Frames.
        0x00, /* unused.  */
        0x00, /* unused.  */
    },

    /*ClassSpecific_Endpiont_Descriptor_t Audio_Data_Endpoint_Setting1 =*/
    {
        sizeof(ClassSpecific_Endpiont_Descriptor_t), //size of descriptor
        0x25, //descriptor type, CS_ENDPOINT
        0x01, //descriptor subtype
        0x01, //bmAttributes, sampling frequency control, no pitch control.
        0x01, //bLockDelayUnits,
        0x0100, //wLockDelay
    },
#ifdef _USB_RECORD_
    /*Standard_AS_Interface_Descriptor_t Interface2_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        0x02, //interface value
        0x00, //alternative setting
        0x00, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_AS_Interface_Descriptor_t Interface2_Setting1 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        0x02, //interface value
        0x01, //alternative setting
        0x01, //number of endpoint
        0x01, //interface class code, Audio Interface class.
        0x02, //interface sub-class code, AUDIOSTREAMING Subclass.
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*ClassSpecific_AS_Interface_Descriptor_t Audio_Class_General_Setting2 =*/
    {
        sizeof(ClassSpecific_AS_Interface_Descriptor_t), //size of descriptor
        0x24, //descriptor type, CS_INTERFACE .
        0x01, //AS_GENERAL descriptor subtype.
        0x07, //The Terminal ID of the Terminal to which the endpoint of this interface is connected.
        0x01, //Delay introduced by the data path. Expressed in number of frames.
        0x0100, //Format Tag, PCM.
    },

     /*Format_Type_Descriptor_t Audio_Class_Format_Setting2 =*/
    {
        sizeof(Format_Type_Descriptor_t), 0x24, //descriptor type, CS_INTERFACE .
        0x02, //FORMAT_TYPE descriptor subtype.
        0x01, //FormatType, FORMAT_TYPE .
        0x01, //bNrChannels, Indicates the number of physical channels in the audio data stream.
        0x02, //bSubframeSize, The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.
        0x10, //bBitResolution, The number of effectively used bits from the available bits in an audio subframe.
        0x01, //bSamFreqType, Indicates how the sampling frequency can be programmed.
		{0x00, 0x00, 0x00,},    
    },

    /*Standard_Endpiont_Descriptor_t Endpoint_Descriptor_Setting2 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x81, //endpoint address (in endpoint addressed at 1)
        0x05, //endpoint type (ISOCHRONOUS)
        ISO_IN_MAX_PACKET_SIZE << 8, 0x01, //The polling interval value is every 1 Frames.
        0x00, /* unused.  */
        0x00, /* unused.  */
    },

    /*ClassSpecific_Endpiont_Descriptor_t Audio_Data_Endpoint_Setting2 =*/
    {
        sizeof(ClassSpecific_Endpiont_Descriptor_t), //size of descriptor
        0x25, //descriptor type, CS_ENDPOINT
        0x01, //descriptor subtype
        0x00, //bmAttributes, sampling frequency control, no pitch control.
        0x00, //bLockDelayUnits,
        0x0000, //wLockDelay
    },
#endif
    /*Standard_AS_Interface_Descriptor_t Interface3_Setting0 =*/
    {
        sizeof(Standard_AS_Interface_Descriptor_t), //size of descriptor
        INTERFACE, //descriptor type
        0x03, //interface value
        0x00, //alternative setting
        0x01, //number of endpoint
        0x03, //HID Interface Class
        0x00, //
        0x00, //interface protocol code, doesn't use any class-specific protocols.
        0x00, //interface string index, doesn't have a string descriptor describing this iInterface.
    },

    /*Standard_HID_Descriptor_t HID_Descriptor_Setting0 =*/
    {
        sizeof(Standard_Endpiont_Descriptor_t), //size of descriptor
        0x21, //descriptor type
        0x0001, //bcdHID
        0x00, //bCountryCode
        0x01, //bNumDescriptors
        0x22, //bDescriptorType
        0x2100, //wDescriptorLength
    },

    /*Standard_Endpiont_Descriptor1_t Endpiont_Descriptor_Setting3 =*/
    {
        sizeof(Standard_Endpiont_Descriptor1_t), //size of descriptor
        ENDPOINT, //descriptor type
        0x83, //bEndpointAddress:-> Direction: IN - EndpointID: 3
        0x03, //bmAttributes:-> Interrupt Transfer Type
        0x0400, //wMaxPacketSize
        0x20, //bInterval
    },

};
//-----------------------------------------------------------
//end of Configuration
//-----------------------------------------------------------

string_language_id_t language_string =
{
    0x04, /* length*/
    0x03, /* Second Byte of this descriptor.  */
    0x0904, /* LANGID code array, US english,0x409 */
};

string_product_t device_name_string =
{
    0x22, 0x03, 'E',  'a',  'r',  'P',  'o',  'd',  's',  '_',  'T',  'C',  '0',  
	'1',  '_',  'J',  'C',  'D',  0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

string_manufacturer_t manufacure_string =
{ 	
	0x12, 0x03, 'C',  '&',  'T', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

serialnum_descriptor_t SerialNumber =
{
    0x22, 0x03, 'U',  '2',  'd',  'c',  '0',  'z',  '7',  'w',  's',  '5', 
	'3',  'a',  '7',  '9',  'r',  'n',  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#pragma userclass(near=default)

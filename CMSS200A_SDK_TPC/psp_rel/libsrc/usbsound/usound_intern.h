#ifndef __USOUND_INTERN_H__
#define __USOUND_INTERN_H__

//#include "ak211x.h"
//#include "stringH.h"
#include "typeext.h"
#include "usbdisk.h"
#include "device.h"
#include "irq.h"
#include "uddrv.h"
#include "otg.h"
#include "usound_setup.h"

#define  USB_DMA_NO      0x00

#define EP0IN              		0x00
#define EP0OUT             		0x01
#define EP1IN              		0x02
#define EP2OUT             		0x03
#define EP3IN              		0x04

#define  PLAY_BUFF_START     	0x4900 //0x5800 //0x3000 //0x5800//0x3000

#ifdef BIT_RESOLUTION_16
#define  PLAY_BUFF_HALF      	0x800
#define  PLAY_BUFF_SIZE      	0x1000
#define  PLAY_HIGH_THRESHOLD 	0xa00
#define  PLAY_LOW_THRESHOLD  	0x600
#define  PLAY_HIGH_OVERFLOW  	0xf00
#define  PLAY_LOW_OVERFLOW   	0x100
#endif
#ifdef BIT_RESOLUTION_24
#if 0
#define  PLAY_BUFF_HALF      	0x2580
#define  PLAY_BUFF_SIZE      	0x4B00
#define  PLAY_HIGH_THRESHOLD 	0x4680
#define  PLAY_LOW_THRESHOLD  	0x500
#define  PLAY_HIGH_OVERFLOW  	0x4A00
#define  PLAY_LOW_OVERFLOW   	0x240
#else
#define  PLAY_BUFF_HALF      	0xC00
#define  PLAY_BUFF_SIZE     	0x1800
#define  PLAY_HIGH_THRESHOLD 	0xC00
#define  PLAY_LOW_THRESHOLD 	0x900
#define  PLAY_HIGH_OVERFLOW  	0x1200
#define  PLAY_LOW_OVERFLOW   	0x300
#endif
#endif

#define  RECORD_BUFF_START     	0x4000
#define  RECORD_BUFF_HALF      	0x380
#define  RECORD_BUFF_SIZE      	0x700
#define  RECORD_HIGH_THRESHOLD 	0x460
#define  RECORD_LOW_THRESHOLD  	0x2a0
#define  RECORD_HIGH_OVERFLOW  	0x700
#define  RECORD_LOW_OVERFLOW   	0x0000
//#define  RECORD_PACKET_SIZE    0x60 //0x20(48x2, 16x2)
#define  DOWNLOAD_BIT_DEPTH_24  0x18
#define  DOWNLOAD_BIT_DEPTH_16  0x10

extern uint32 rd_record_buff_ptr;
extern int16 record_data_count;
extern uint32 wr_play_buff_ptr;
extern int16 play_data_count;
extern uint16 volume_value;
extern uint16 play_vol_ch0;
extern uint16 record_vol_ch0;
//extern uint16 current_volume_ch1;
//extern uint16 current_volume_ch2;
//extern uint8 pa_mute;
extern uint8 usb_state;
extern uint8 play_mute_ch0;
extern uint8 record_mute_ch0;
extern uint32 current_sample;
extern uint8 cur_selector;
extern device_requests_t setup_data;
extern device_audio_configuration_t All_Descriptors_of_Configuration;
extern uint8 hid_report;
extern uint8 ReportDescriptor[0x21];
extern device_descriptor_t Device_Descriptor;
extern device_audio_configuration_t All_Descriptors_of_Configuration;
extern serialnum_descriptor_t SerialNumber;
extern string_manufacturer_t manufacure_string;
extern string_product_t device_name_string;
extern string_language_id_t language_string;
extern uint32 usb_isr;
extern uint32 dma_adc_isr;
extern uint32 dma_dac_isr;
extern uint8 start_play_flag;
extern uint8 start_record_flag;
extern uint8 muram_addr;
extern uint8 hid_send_flag;
extern uint8 mcu_pll_ctl_bak;
extern uint8 cpuclkctl_bak;
extern uint8 pa_volume_bak;
extern uint8 g_record_packet_size;
extern uint8 cur_vol_index;
extern uint8 sample_rate_arr[4][3];
extern uint8 vol_change_flag;
extern void deal_setup(void);
extern void usound_isr(void);
//extern void descriptor_init(void);
extern void mdelay(uint8 mscount);
//extern void usdelay(uint8 uscount);
extern void usb_fifo_access(uint8 ep_select, uint8 *address, uint16 data_lenth);
extern void dma_reload_to_dac(void);
extern void dma_reload_from_adc(void);
extern void IRQ_ISR(void);
extern void dma_adc_half_isr(void);
extern void dma_dac_half_isr(void);

#endif

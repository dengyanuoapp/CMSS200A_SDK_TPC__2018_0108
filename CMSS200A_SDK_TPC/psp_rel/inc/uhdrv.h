/********************************************************************************
*                              AS212A
*                            Module: USB Host
*                 Copyright(c) 2001-2010 Actions Semiconductor,
*                            All Rights Reserved.
*
* History:
*      <author>    <time>           <version >             <desc>
*       xiaomaky   2009-12-4 13:54     1.0                build this file
********************************************************************************/



#ifndef _UHDRV_H
#define _UHDRV_H

#include "string.h"
#include "typeext.h"
#include "port.h"
#include "irq.h"
#include "message.h"
#include "uddrv.h"

/* error  meaning */
// #define GENERAL_OK                     0x00
// #define GENERAL_ERROR                  0x01
// #define DEVICE_NOT_IN                  0x02
// #define ENUMERATION_FAIL               0x03
// #define BULKSTART_FAIL                 0x04
// #define ERROR_NOT_SUPPORT_DEVICE       0X05
// #define ERROR_SETUP_OVERTIME           0X06
// #define ERROR_CONTROL_IN_DATA          0X07
// #define ERROR_STATUS_OVERTIME          0X08
// #define ERROR_NOT_SUPPORT_PROTOCOL     0X09
// #define ERROR_UNIT_READY_OVERTIME      0X0A
// #define ERROR_IN_DATA_WRONG            0X0B
// #define ERROR_LUN_BLOCK_SIZE           0X0C
// #define ERROR_BUS_RESET_OVERTIME       0X0D

// #define ERROR_TRANSFER_BUSY            0X80
// #define ERROR_IRQ_OCCUR                0X81
// #define ERROR_EP_BUFFER_NOT_EMPTY      0X82  /* not busy, but some endpoint sub-buffer is not empty */
// #define ERROR_OTG_STATE                0X83
// #define ERROR_EP_STALL                 0X84
// #define ERROR_NO_EP_HANDSHAKE		   0X85

// #define ERROR_PARAMETER_IN             0XF0
// #define ERROR_MIDIUM_NOT_PRESENT	   0XF1
// #define ERROR_MIDIA_CHANGED	   		   0XF2


/* function number  */
#define APINO_UH_Init                  APINO_UD_Init          //0x0000+API_STG_BaseNoB
#define APINO_UH_Exit                  APINO_UD_Exit          //0x0001+API_STG_BaseNoB
#define APINO_UH_SectorRead            APINO_UD_SectorRead    //0x00
#define APINO_UH_SectorWrite           APINO_UD_SectorWrite   //0x01
#define APINO_UH_Update                APINO_UD_Update        //0x04
#define APINO_UH_GetCap                APINO_UD_GetCap        //0x0002+API_STG_BaseNoB                   
#define APINO_UH_GetMaxLun             0x06
#define APINO_UH_GetCurrentLun         0x07
#define APINO_UH_GetSectorSize         APINO_UD_GET_SCTR_SIZE //0x1002+API_STG_BaseNoB



#ifdef __C251__


/*!
 *  structure of read or write info   \n
 *  \li SectorAddress: logical block address by 512BYTE.
 *  \li SectorNumber : sector number.
 *  \li RAMAddress   : RAM address.
 *
 */
/***********************************************************
�������ƣ�  UH_Init
�������ܣ�  uhost�����ĳ�ʼ����������DRV_Install��ʱ����ã�DRV_Install
            ֮��Ҳ������ʱ���ã�������ȡusb����״̬��ص���Ϣ
�������룺  mode:  �̷�
�������أ�  0x01       success;
            0xff    û�в���miniA��
            0xfe    ������miniA�ߣ�����û�в���u�̣�����u��û������
            0xfd    �豸��֧��
            //0xfc    ���ݴ������˵���������ܲ���----------��ʱ��ʵ��
            ����    resverd
*************************************************************/
//non_banked byte UH_Init(unsigned char mode);
#define UH_Init(mode) API_UH_Init(APINO_UD_Init,mode)
uint8 API_UH_Init(uint8 cmd, char mode);

/***********************************************************
�������ƣ�  UH_Exit
�������ܣ�  uhost�������˳���������DRV_UnInstall��ʱ�����
�������룺
�������أ�
*************************************************************/
#define UH_Exit() API_UH_Exit(APINO_UH_Exit)
void API_UH_Exit(uint8 cmd);

/***********************************************************
�������ƣ�  UH_SectorRead
�������ܣ�  ����������
�������룺  shro:   �μ�struct UD_RW
�������أ�  0       success;
            0xff    ��ַ��������粻���ڻ�Խ��ĵ�ַ
            0xfe    U���ϱ��˶�ʧ��
            ����    resverd
*************************************************************/
#define UH_SectorRead(shro) API_UH_SectorRead(APINO_UH_SectorRead,shro)
uint8 API_UH_SectorRead(uint8 cmd, UD_RW *shro);

/***********************************************************
�������ƣ�  UH_SectorWrite
�������ܣ�  ����д����
�������룺  shro:   �μ�struct UD_RW
�������أ�  0       success;
            0xff    ��ַ��������粻���ڻ�Խ��ĵ�ַ
            0xfe    U���ϱ���дʧ��
            ����    resverd
*************************************************************/
#define UH_SectorWrite(shro) API_UH_SectorWrite(APINO_UH_SectorWrite,shro)
uint8 API_UH_SectorWrite(uint8 cmd, UD_RW *shro);

/***********************************************************
�������ƣ�  UH_GetMaxLun
�������ܣ�  ��ȡ������Ŀ
�������룺
�������أ�  0��15   ������Ŀ������Э��涨ȡֵ��Χ��0��15��
                    ע������3������0��3�����ĸ���
            ����    resverd
*************************************************************/
#define UH_GetMaxLun() API_UH_GetMaxLun(APINO_UH_GetMaxLun)
uint8 API_UH_GetMaxLun(uint8 cmd);

/***********************************************************
�������ƣ�  UH_TestLunReady
�������ܣ�  ���Զ�Ӧ������û�м��ش���
�������룺  lun     �����Եķ�����
�������أ�  0       �÷��������˴���
            0xfa       �÷���û�м��ش���
            0xfc       �����lun������
            ����    resverd
*************************************************************/
//uint8 UH_TestLunReady(unsigned char lun);

/***********************************************************
�������ƣ�  UH_GetCap
�������ܣ�  ��ö�Ӧ���̵Ĵ�С
�������룺  lun     ��Ӧ���̵ķ�����
�������أ�  �����Ĵ�С����512 byteΪ��λ��һ��block����512��u�̣�uhost�����Ὣ��ת������512 byteΪ��λ
            0       error
*************************************************************/
#define UH_GetCap() API_UH_GetCap(APINO_UH_GetCap)
uint32 API_UH_GetCap(uint8 cmd);

/***********************************************************
�������ƣ�  UH_GetSectorSize
�������ܣ�  ���u��һ��Sector�Ĵ�С
�������룺  lun     ��Ӧ���̵ķ�����
�������أ�  u��һ��Sector�Ĵ�С���������n����һ��Sector�Ĵ�С��n*512 byte
            0       error
*************************************************************/
#define UH_GetSectorSize() API_UH_GetSectorSize(APINO_UH_GetSectorSize)
uint8 API_UH_GetSectorSize(uint8 cmd);

/***********************************************************
�������ƣ�  UH_TestUdiskReady
�������ܣ�  ���Ե�ǰ����״̬
�������룺
�������أ�  0       �ɹ���miniA���룬u��Ҳ���룬�����ӡ���ʱ���Խ��ж����õ������Ȳ���
            0xff    û�в���miniA��
            0xfe    ������miniA�ߣ�����û�в���u�̣�����u��û������
            0xfd    �豸��֧��
            ����    resverd
ע���Ὣ�����������UH_Init�ļ򻯰棬��Ҫͨ����ѯһЩ�����־λ�����ض�Ӧ��ֵ
*************************************************************/
//uint8 UH_TestUdiskReady(void);

/***********************************************************
�������ƣ�  UH_GetCurrentLun
�������ܣ�  ��ȡ��ǰ����Ĵ��̷�����
�������룺
�������أ�  0��15   ��ǰ����Ĵ��̷�����
            ����    resverd
*************************************************************/
#define UH_GetCurrentLun() API_UH_GetCurrentLun(APINO_UH_GetCurrentLun)
uint8 API_UH_GetCurrentLun(uint8 cmd);

/***********************************************************
�������ƣ�  UH_SetCurrentLun
�������ܣ�  �趨��ǰ����Ĵ��̷����ţ�������趨�Ļ���Ĭ��Ϊ0
�������룺  lun     ��Ӧ���̵ķ�����
�������أ�  0       success
            0xfc       �����lun������
            0xfd    �豸��֧��
            ����    resverd
*************************************************************/
//uint8 UH_SetCurrentLun(unsigned char lun);

/***********************************************************
�������ƣ�  UH_WordRead
�������ܣ�  ��ȡĳһ��lba��ĳ��ƫ������˫�ֽ�����
�������룺  UD_RW
            typedef struct
            {
                dword   lba;        //word���������ڵĵ�ַ
                char    reserv;     //δ��
                void    byteaddr;   //word�������ڵ��ֽ�ƫ��
                byte    reserv;     //δ��
            }UD_RW;
�������أ�  0       success
            1       0xff    ��ַ��������粻���ڻ�Խ��ĵ�ַ
            ����    resverd
*************************************************************/
//int16 UH_WordRead(UD_RW *rwaddr);

/***********************************************************
�������ƣ�  UH_DwordRead
�������ܣ�  ��ȡĳһ��lba��ĳ��ƫ���������ֽ�����
�������룺  UD_RW
            typedef struct
            {
                dword   lba;        //word���������ڵĵ�ַ
                char    reserv;     //δ��
                void    byteaddr;   //word�������ڵ��ֽ�ƫ��
                byte    reserv;     //δ��
            }UD_RW;
�������أ�  0       success
            1       0xff    ��ַ��������粻���ڻ�Խ��ĵ�ַ
            ����    resverd
*************************************************************/
//int16 UH_DwordRead(UD_RW *rwaddr);


/***********************************************************
�������ƣ�  UH_Update
�������ܣ�  �պ�����FS_FClose()�ر�һ���ļ�ʱ������
�������룺
�������أ�
*************************************************************/
#define UH_Update() API_UH_Update(APINO_UH_Update)
void API_UH_Update(uint8 cmd);


/*
message.h��
#define MSG_UH_IN               0xc0   //U�̲���
#define MSG_UH_OUT              0xc1   //U�̰γ�
#define MSG_UH_UDISK_OUT        0xc2   //�����еĿ��γ�
*/
#define     ISR_CHECK_MODE
extern uint8  sHOST_Glbal;
#define     g_uhost_state               sHOST_Glbal

/* function parameter for reset_usb_controller() */
#define     CONTROLLER_INIT             0x00
#define     CONTROLLER_ENABLE           0x01
#define     ISR_UHOST                   0x02

#define     old_isr                     (*(uint8 *)0x27a9)

/* OTG state machine */
#define FSM_A_HOST                     0x03 /* The USB FSM State: A_host */

#define GENERAL_OK                     0x00


#define ERROR_DEVICE_NOT_IN            0x02
#define ERROR_BUS_RESET_OVERTIME       0x06

#define     BULK_IN_EP_NUM                                                  2 // BULK IN �˵��,
#define     BULK_OUT_EP_NUM                                                 1 // BULK OUT �˵��

/* 1) �� ��U�Կ������£�ʹ��ȫ��ģʽ��URAM��512byte�����˵㻺������IN�˵��OUT�˵����256byte(= 64 * 4)
 * 2) ��U�Կ������£�ʹ�ø���ģʽ��PCMRAM��4KB�����˵㻺������IN�˵��OUT�˵����2KB(= 512byte * 4)
 */
#ifdef  __CARD_UDISK_TRANSFER__ // ��U�Կ��ĳ���, Bulk in/out����2KB

#define     EP_MAX_BUF_SIZE_BULK_IN                                         0x800 // 2KB
#define     EP_MAX_BUF_SIZE_BULK_OUT                                        0x800 // 2KB

#else

#define     EP_MAX_BUF_SIZE_BULK_IN                                         192 // 192 = 64 * 3
#define     EP_MAX_BUF_SIZE_BULK_OUT                                        128 // 128 = 64 * 2

#endif  // #ifdef  __CARD_UDISK_TRANSFER__

#ifdef  __CARD_UDISK_TRANSFER__ // ��U�Կ��ĳ���, Bulk in/out����2KB
#define     EP_BUF_STA_ADDR_BULK_IN     0x380 // 0x380 bytes�Ŀռ�ָ "URAM + FIR_RDS_RAM"
#define     EP_BUF_STA_ADDR_BULK_OUT    (EP_BUF_STA_ADDR_BULK_IN + EP_MAX_BUF_SIZE_BULK_IN)
#else
#define     EP_BUF_STA_ADDR_BULK_IN     192 // 192 bytes�Ŀռ�ָ EP0_IN��EP0_OUT��EP3�õĸ�64 bytes�Ĵ�С
#define     EP_BUF_STA_ADDR_BULK_OUT    (EP_BUF_STA_ADDR_BULK_IN + EP_MAX_BUF_SIZE_BULK_IN)
#endif  // #ifdef  __CARD_UDISK_TRANSFER__

#define WATCHDOG_CLEAR()                ClearWatchDog()
#define delay_ms(x)                     TM_DelayMS(x)

// enable external USB interrupt request(����USB�ж϶������ʹ��λ)
#define     ENABLE_EXT_USB_IRQ()                            (USBEIRQ = USBEIRQ | 0x08)
// disable external USB interrupt request(����USB�ж϶������ʹ��λ)
#define     DISABLE_EXT_USB_IRQ()                           (USBEIRQ = USBEIRQ & 0xf7)
/* close USB clock (����USB controller���ֿ���ʡ���ģ�DP&DM��ʱ��ԴӦ����AHBCLK�����Թرյ�clock��DP&DM��ʱ��Դ�޹�).
 * �ر�PLLEN����,������PLLEN, USBRESET �� GoSuspend�����ù�,����Phy_DALLUALLEN���ڲ��õ�,Ҳ�ɹر�
 */
#define     CLOSE_USB_CONTROLLER_CLOCK() \
            (USB_PHYCTRL = USB_PHYCTRL & 0x3f)
#define     OPEN_USB_CONTROLLER_CLOCK()  \
            (USB_PHYCTRL = USB_PHYCTRL | 0xc0)

/* enable Line status detect, enable 15Kohm DP/DM pull down, disable 500Kohm DP/DM pull up resistor */
#define     ENABLE_DEVICE_DETECT() \
            (DPDMCTRL = (DPDMCTRL & 0xf0) | 0x10)
#define     DISABLE_DEVICE_DETECT() \
            (DPDMCTRL = DPDMCTRL & 0xef)
#pragma ROM(HUGE)
void reset_usb_controller (uint8 call_from);
#endif /* __C251__ */

#endif /*_UHDRV_H */












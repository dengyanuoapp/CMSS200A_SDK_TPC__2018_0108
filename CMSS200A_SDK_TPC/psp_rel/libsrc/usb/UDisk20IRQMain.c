/*
 *********************************************************************************************************
 *                                       ACTOS GL3976
 *
 *
 *                           (c) Copyright 2002-2007, Actions Co,Ld.
 *                                   All Right Reserved
 *
 * File   : Udisk20IRQMain.msa
 * By     : zhouxl
 * Version: v1.02     first version     2004-12-13 12:45
 *********************************************************************************************************
 */

#include "udisklib.h"

STG_INFO edata StorageInfor _at_ 0x00E0;
#pragma name(RCODE)

#ifndef	UDISK_AP
#pragma asm
EXTRN EDATA(SP_Keeper)
#pragma endasm
#endif

#pragma ROM(HUGE)
void Reset_Recover(void);
void USB_Bus_Reset_ISR(void);
void VBus_Change_Handle(void);
void Suspend_ISR(void);
void Hs_mode_ISR(void);
void Other_ISR(void);
#pragma ROM(HUGE)

void ADFUHandle(void);
void sFWUpgOkDelay(uint8 fwup_direction);
void sDelete_FlashVram(uint8 sectorNum, uint32 lbaAddr);
void sState_0_Handle(void);
void sState_10_Handle(void);
void sState_20_Handle(void);
void sGoInState0(void);
void sSplitDiskEject(void);
void UdiskReady(void);
void UdiskUnReady(void);
static uint8 CheckTimer(uint8 *timerrecord, uint8 *countrecord, uint8 count);

#pragma renamecode(?PR?RCODE)
#pragma ROM(HUGE)

//�˶���Ϊ�������ε���ӡ��udiskӦ�ñ��볣���ζ���δʹ�õ��µı������
#ifndef _PRINT_DEBUG_
const uint8 ConstSeg3 = 'A';
#endif
/*
 *********************************************************************************************************
 *                                           U20_ISRMain
 *
 * Description: USB2.0 Interrupt Server Entry.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */

void U20_ISRMain(void) //interrupt 8
{
#ifndef	UDISK_AP
#pragma asm
    MOV SP_Keeper, DR60
#pragma endasm
#endif

    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_USB;

    ClearWatchdog();

    USBEIRQ = USBEIRQ | (uint8) 0x80;

    if ((SHORTPCKIRQ & 0x20) != 0x00)
    {
        Bulk_Out_Handle();
    }
    else if ((Usbirq_hcusbirq & 0x01) != 0x00)
    {
        Setup_Packet_Handle();
    }
    else if ((Usbirq_hcusbirq & 0x10) != 0x00)
    {
        USB_Bus_Reset_ISR();
    }

    else if ((Usbirq_hcusbirq & 0x80) != 0x00)
    {
        VBus_Change_Handle();
    }

    else if ((Usbirq_hcusbirq & 0x08) != 0x00)
    {
        Suspend_ISR();
    }

    else if ((Usbirq_hcusbirq & 0x20) != 0x00)
    {
        Hs_mode_ISR();
    }
    else
    {
        Other_ISR();
    }

    Reset_Recover();
}

void Reset_Recover(void)
{
    SFR_BANK = sfr_bak;
#ifndef	UDISK_AP
#pragma asm
    MOV DR60, SP_Keeper
#pragma endasm
#endif
}

/*
 *********************************************************************************************************
 *                                           IfIsConnect
 *
 * Description: �ж��Ƿ����߻���Timeout, ������쳣, ��ֱ���˳��жϷ������.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : ���ݴ���ʧ��ʱ,PC�ᷢ��Bus Reset��С��;���߶�д�����а���.
 *********************************************************************************************************
 */
void ifIsConnect(void)
{
    uint8 usbrstflag, disconnectflag;
    uint8 sfrbak;
    ClearWatchdog();

    sfrbak = SFR_BANK;
    usbrstflag = 0x00;
    disconnectflag = 0x00;
    SFR_BANK = BANK_USB;
    if ((Usbirq_hcusbirq & 0x10) != 0x00)
    {
        usbrstflag = 0x01;
    }
#ifndef _FPGA_VERTION_
    if ((DPDMCTRL & 0x40) == 0x00)
    {
        disconnectflag = 0x01;
    }
#endif

    //USB disconnect or usb reset
    if ((usbrstflag == 0x01) || (disconnectflag == 0x01))
    {
#ifdef _PRINT_DEBUG_
        prints("timeout\n");
#endif

        if (fifo_trans_mode == 0x01)
        {
            SFR_BANK = BANK_DMA_CARD;
        }
        else
        {
            SFR_BANK = BANK_DMA0;
        }
        //��DMAֹͣ
        DMAnCTL0 = DMA0CTL0 & (uint8) 0xfe;
        //FIFOֱͨģʽ������ر�ֱͨ���ã�����DMA�����ô���fifoֱͨģʽ,���º����洢���ʴ����쳣
        DMAnCTL1 = 0x00;
        fifo_trans_mode = 0x00;

        SFR_BANK = BANK_USB;
		UDMAM = 0x0c;
        //��FIFO��EPA�ж�Ϊ��һ��CBW׼��
        EPB_OutRdy_ShortPacketNAK();
    }

    if (usbrstflag == 0x01)
    {
        USB_Bus_Reset_ISR();
        Reset_Recover();
    }
    else if (disconnectflag == 0x01)
    {
        VBus_Change_Handle();
        Reset_Recover();
    }
    else
    {
    }
    SFR_BANK = sfrbak;
}

/*
 *********************************************************************************************************
 *                                           USB_Bus_Reset_ISR
 *
 * Description: Root Port Reset Interrupt handle.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void USB_Bus_Reset_ISR(void)
{
    Usbirq_hcusbirq |= 0x10;

    //����Udiskö��ֻ�ᷢ������Reset,֮���Reset��Ϊ�����PC���͵�
    usb_reset_enter_cnt++;

    //����bus reset��Բ��ֱ����������ã���������з���bus reset���˳�u�����⡣
    Suspend_Flag = 0x00;

    ConfigValue = 0;
    AlternateSetting = 0;
    DeviceStatus = 0;
    InterfaceStatus = 0;
    EndPoint1Status = 0;
    EndPoint2Status = 0;
}

/*
 *********************************************************************************************************
 *                                           VBus_Change_Handle
 *
 * Description: Connect/Disconnect Interrupt.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void VBus_Change_Handle(void)
{
    smDelay(3);

    PullLineFlag = 0x00;

#ifdef _PRINT_DEBUG_
    prints("L1\n");
#endif

    if ((DPDMCTRL & 0x40) == 0x00)
    {
#ifdef _PRINT_DEBUG_
        prints("L2\n");
#endif

        PullLineFlag = 0x01;
    }

    //clear VBus interrupt
    Usbirq_hcusbirq |= (uint8) 0x80;
}

/*
 *********************************************************************************************************
 *                                           Suspend_ISR
 *
 * Description: Suspend Interrupt handle.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Suspend_ISR(void)
{
    uint8 tmp;
    Usbirq_hcusbirq |= 0x08;

    //bit[7]:1 need process suspend,0 don't process suspend; bit[1:0]:0 no suspend; 1 recieve suspend; 2 has handle suspend
    if ((Suspend_Flag & 0x80) == 0x00)
    {
    }
    else if (detach_cmd_rcv != 0x00)
    {
        FWOpStatus = 1;
        IsAdfuCondition = 1;
    }
    else
    {
#ifdef _PRINT_DEBUG_
        prints("S1\n");
#endif
        //USB plug-in��־��ӳ��ʵ�����Ҫ�3ms�ӳ�ʱ��
        smDelay(3);

        //if plug off usb line,don't send suspend message to AP
        if ((DPDMCTRL & 0x40) != 0x00)
        {
            //ESD���Բ���suspend����ʱ1s����,��PC����bus reset,�򲻴���suspend
            tmp = 4;
            while ((tmp--) != 0x00)
            {
                if ((Usbirq_hcusbirq & 0x10) != 0x00)
                {
                    break;
                }
                TM_DelayMS(250);
                ClearWatchdog();
            }

            if ((Usbirq_hcusbirq & 0x10) == 0x00)
            {
                //������suspend״̬,Ӳ���ָ�Ĭ��״̬
                usb_pwr_suspnd_fnsh = 0;
                //��reset���,�����豸���׽���Full Speedģʽ
                usb_reset_enter_cnt = 0;
                //0:no suspend; 1:recieve suspend; 2:has handle suspend
                Suspend_Flag++;

                //enable high speed
                BKDOOR = (BKDOOR & 0x7f) | 0x10;
            }
            else
            {
#ifdef _PRINT_DEBUG_
                prints("S2\n");
#endif
            }
        }
        //�������д����,�����һ�δ�������д��Flash
        UD_Update();
    }
}

/*
 *********************************************************************************************************
 *                                           Hs_mode_ISR
 *
 * Description: Enter high speed mode interrupt occur.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : �豸���յ�Bus reset,�����Chirp J/K�ķ��ͺ�,�����High Speed�ж�.
 *********************************************************************************************************
 */
void Hs_mode_ISR(void)
{
    Usbirq_hcusbirq |= 0x20;

    //the first connect to PC,usb reset has more than twice,change to full speed
    //after setting config,don't change to full speed
    if (usb_pwr_suspnd_fnsh == 0x00)
    {
        if (usb_reset_enter_cnt >= 2)
        {
            //disable high speed,bit4��ƴ���,��λӦд1,��ÿ�ζ�������0
#ifdef _PRINT_DEBUG_
            prints("set fs \n");
#endif
            BKDOOR = BKDOOR | (uint8) 0x90;
        }
    }
}

/*
 *********************************************************************************************************
 *                                           Other_ISR
 *
 * Description: Other ISR handle.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void Other_ISR(void)
{
    Usbirq_hcusbirq = Usbirq_hcusbirq;
}
#pragma ROM(HUGE)

/*
 ********************************************************************************
 *                           OTG_PHY_Setting
 * Description : USB PHY����
 * Arguments   : d: address; e: value.
 * Returns     : none
 * Notes       : FPGA version, Just for GL6005 PHY setting.
 ********************************************************************************
 */
void OTG_PHY_Setting(unsigned char addr, unsigned char value)
{
    unsigned char tmp;

    VDSTAT = value; //set Value

    tmp = addr;
    VDCTRL = tmp & 0x0f; //set low addr
    VDCTRL = tmp | 0x10;

    tmp = (addr >> 4);
    VDCTRL = tmp & 0x0f; //set high addr
    VDCTRL = tmp | 0x10;
}

/*
 *********************************************************************************************************
 *                                           UDiskHandle
 *
 * Description: ��ѯ������UDisk״̬��������״̬����Ӧ����.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UDiskHandle(void)
{
    uint8 IE0bak, sfrbak, timecount;

    IE0bak = IE0;
    IE0 = IE0 & 0x7f;
    StatusCheckFlag = 0x01;
    IE0 = IE0bak;

    adfu_flag = USB_State;

    if (IsAdfuCondition == 0x00)
    {
        //set config ֮������usb_reset_enter_cntΪ0x00,���ٽ��г�����ж�
        if (usb_pwr_suspnd_fnsh == 0x00)
        {
            //Suspend_Flag 0:no suspend; 81:recieve suspend; 82:has handle suspend
            if ((usb_reset_enter_cnt == 0x00) && (Suspend_Flag == 0x00))
            {
                //�鶨ʱ,connect_idle_cnt:������ָ��  prv_connect_idle_tm:1Hz�仯��¼����  c-1:����ֵ,��λΪ1��

                timecount = 15;
                if (SplitDiskFlag != 0x00)
                {
                    timecount = 0x05;
                }
                if (CheckTimer(&connect_idle_cnt, &prv_connect_idle_tm, timecount) != 0x00)
                {
                    //������ǳ����,��ʱ��û��bus reset
#ifdef _PRINT_DEBUG_
                    prints("IsCharge?-");
#endif
                    USB_State = 0x70;
                    return;
                }
            }
        }
    }
    else
    {
        //ADFU status
        ADFUHandle();
        return;
    }

    //���߱�־
    if (PullLineFlag != 0x00)
    {
        //�����˳�״̬
        USB_State = 0x40;
        TransferFlag = 0x40;
        return;
    }
    else if ((Suspend_Flag & 0x7f) == 0x01)
    {
        //Suspend_Flag 0:no suspend; 81:recieve suspend; 82:has handle suspend
        Suspend_Flag++;
        USB_State = 0x80;
        return;
    }
    else if (SwitchToADFUFlag != 0x00)
    {
        SwitchToADFUFlag = 0x00;
        //�л���ADFU
        USB_State = 0x51;
        TransferFlag = 0x51;
        return;
    }
    //PC������־
    else if (PcEjectFlag != 0x00)
    {
        //�����˳�״̬
        USB_State = 0x30;
        return;
    }
    else if (PartiEjectFlag != 0x00)
    {
        PartiEjectFlag = 0x00;
        //��������˳�״̬
        USB_State = 0x60;
        return;
    }
    else
    {
    }

    switch (USB_State)
    {
    case 0x00:
        //����״̬
        sState_0_Handle();
        break;
    case 0x10:
        //�ϴ�״̬
        sState_10_Handle();
        break;
    case 0x20:
        //�´�״̬
        sState_20_Handle();
        break;
    case 0x51:
        //��״̬��Ҫ�����л���ADFU���յ��ĵ�һ�� CMD_TestUnitReady_����,�ָ��ж�
        UdiskReady();
        break;
    case 0x60:
        //������������
        sSplitDiskEject();
        break;
    case 0x80:
        USB_State = 0x90;
        break;
    default:
        sfrbak = SFR_BANK;
        SFR_BANK = BANK_USB;
        if ((DPDMCTRL & 0x40) == 0x00)
        {
            //�Ѱγ�
            USB_State = 0x40;
        }
        else
        {
            //����״̬
            USB_State = 0x00;
        }
        SFR_BANK = sfrbak;
        break;
    }
}

/*
 *********************************************************************************************************
 *                                           ADFUHandle
 *
 * Description: ��ѯ������ADFU״̬��������״̬����Ӧ����.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void ADFUHandle(void)
{
    uint32 callentryRetValue;

    //�Ƿ��Ѿ����� 55h��ʾ���ӣ�77h��ʾ������
    if (PullLineFlag == 0x00)
    {
        //FWOpStatus:���ػ������ز������ 01:���� 02:����
        if (FWOpStatus != 0x00)
        {
            //FW���ػ����������ʱ2��
            sFWUpgOkDelay(FWOpStatus);
        }
        else
        {
            //�Ƿ��й��ܵ�������
            if (CallingTaskFlag != 0x00)
            {
                CallingTaskFlag = 0x00;
                callentryRetValue = (*((uint32( *)()) (CallingTaskAdd)))(); //���ܵ�����ڵ�ַ
                CallingTaskResult = callentryRetValue >> 16; //״̬�ṹ�ĵ�ַ
                StatusLength = (uint16) callentryRetValue; //״̬�ṹ�ĳ���
            }
        }
        UdiskReady();
    }
    else
    {
        //prints("ADFU PullLine-");
        PullLineFlag = 0x00;
        FWOpStatus = 0x01;
        TransferFlag = 0x61; //ADFU��״̬
    }
}

/*
 *********************************************************************************************************
 *                                           sFWUpgOkDelay
 *
 * Description: FW���ػ����������ʱ2��.
 *
 * Arguments  : fwup_direction: ���ػ������ز������ 01:���� 02:����.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sFWUpgOkDelay(uint8 fwup_direction)
{
    if (fwup_direction == 0x01) //need reboot
    {
        //���꼶������Ӧ�ж�
        IE0 &= 0x7f;
        //��������UdiskReady���ж�
        StatusCheckFlag = 0x00;

        if (CheckTimer(&AdfuTimerCtl, &PrevRTCTimerValue1, USBIDLECOUNTERVALUE) != 0x00)
        {
            //prints("udisk exit -");
            ClearWatchdog();
            if (AccessFlash != 0x00)
            {
                UD_ClrVram();
            }
            UDiskExit();
            //UD_Exit();

            //LBA-NAND flash��֧��0xFF��������ش��޸�
            if (flash_type == 0x03)
            {
                /*fixme:need
                 reboot_flash();
                 */
            }
            //prints("watchdog-");
            IE0 &= 0x7f;

            SFR_BANK = BANK_RTC;

            WDCTL =  0x8a; //watchdogʹ�ܲ�����

            while (1)
            {
                ; //Watch Dog����������
            }
        }
    }
    else
    {
        //prints("go idle-");
        //��ʱ����,�ؿ���״̬
        if (CheckTimer(&AdfuTimerCtl, &PrevRTCTimerValue1, USBIDLECOUNTERVALUE * 2) != 0x00)
        {
            FWOpStatus = 0x00;
            TransferFlag = 0x00; //�ص�Ready״̬
        }
    }
}

/*
 *********************************************************************************************************
 *                                           sState_0_Handle
 *
 * Description: ����״̬������.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sState_0_Handle(void)
{
    //�Ƿ�ʼæ,UpOrDownLoad��δ����ʼ��,EP1������read,write��Ľ���,01:�ϴ�  02:�´�
    if ((USBBusy == 0x00) || (UpOrDownLoad == 0x00))
    {
        //Ϊ��һ����CBW׼��Ep1
        UdiskReady();
    }
    else
    {
        if (UpOrDownLoad == 0x01)
        {
            //�����ϴ�״̬
            USB_State = 0x10;
        }
        else if (UpOrDownLoad == 0x02)
        {
            //�����´�״̬
            USB_State = 0x20;
        }
        else
        {
        }
    }
}

/*
 *********************************************************************************************************
 *                                           sState_10_Handle
 *
 * Description: �ϴ�״̬.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sState_10_Handle(void)
{
    //01:�ϴ�  02:�´�
    if (UpOrDownLoad == 0x01)
    {
        //�鶨ʱ,UsbIdleCounter:������ָ��  PrevRTCTimerValue1:1Hz�仯��¼����  USBIDLECOUNTERVALUE:����ֵ,��λΪ1��
        if (CheckTimer(&UsbIdleCounter, &PrevRTCTimerValue1, USBIDLECOUNTERVALUE) == 0x00)
        {
            //Ϊ��һ����CBW׼��Ep1����ʾ��ռ��DMA��������U����Ӧһ�����
            UdiskReady();
        }
        else
        {
            //һ��ʱ���ղ���USB�ж�,���뵽����״̬
            sGoInState0();
        }
    }
    else
    {
        USB_State = 0x20;
    }
}

/*
 *********************************************************************************************************
 *                                           sState_20_Handle
 *
 * Description: �´�״̬������.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sState_20_Handle(void)
{
    if (UpOrDownLoad == 0x02)
    {
        //�Ƿ�������
        if (CheckTimer(&Counter_30s, &PrevRTCTimerValue2, COUNTERVALUE_30S) == 0x00)
        {
            //���˿ɰ�U������û��  0:���ɰ��ߣ�01h:�ɰ��ߡ����������ñ��������Ƿ����
            //��Ϊ��XP���ڴ������������а����(���ǵĹ������)Ҳ�ᷢ�ɰ���
            if ((Removable == 0x00) || (CheckTimer(&UsbIdleCounter, &PrevRTCTimerValue1, USBIDLECOUNTERVALUE) == 0x00))
            {
                UdiskReady();
            }
            else
            {
                //һ��ʱ���ղ���USB�ж�,���뵽����״̬
                sGoInState0();
            }
        }
        else
        {
            //һ��ʱ���ղ���USB�ж�,���뵽����״̬
            sGoInState0();
        }
    }
    else
    {
        USB_State = 0x10;
    }
}

/*
 *********************************************************************************************************
 *                                           sGoInState0
 *
 * Description: һ��ʱ���ղ���USB�ж�, ���뵽����״̬.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sGoInState0(void)
{
    //�������д����,�����һ�δ�������д��Flash
    UD_Update();

    //Ϊ�������״̬����ʾ������ӦUSB�ж�
    UdiskUnReady();
    USB_State = 0x00;
    USBBusy = 0x00;
    //�ӿ��е���д����ʾ���뷢�ڹ�USB�ж�������(for brom can't run at 48M clock)
    UpOrDownLoadBak = 0x00;
}

/*
 *********************************************************************************************************
 *                                           sCardCapCheck
 *
 * Description: Card Capacity Check
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : ������Ͽ��̷�,PC�ͻ������,��U-modeģʽ��,����������ܽ���,�ʱ��밲װ������.
 *********************************************************************************************************
 */
void sCardCapCheck(void)
{
    uint8 reg_bak, drv_install_flag;
    reg_bak = IE0;
    IE0 = IE0 & 0x7f;
    //single lun of card mode need install card driver
    if ((DiskAppearSelect != 0x03) && (ShowMultiDiskFlag == 0))
    {
        CardExistFlag = 0; //��װʧ��,�޿���־

    }
    else
    {
        if(StorageInfor.ce_info[0].type == 3)
        {
        	  drv_install_flag = (uint8) DRV_Install(FFCardBaseDrvName, 0);
        }
        else
        {
            if (DRV_CheckCard(1) == 2) //��⿨�Ƿ���ϺͿ�����:0:��������; 1:MS��; 2:SD/MMC��,��д����; 0x11:д������SD��.
            {
                //char DRV_Install(const char *filename, word param);; param=0:���� 01:����
                drv_install_flag = (uint8) DRV_Install(FFSDCardDrvName, 0);
            }
            else
            {
                drv_install_flag = 0;
            }
        }

        //0:ʧ��  01h:�ɹ�
        if (drv_install_flag == 0x01)
        {
            CardCapacity = (uint32) UD_SDCardGetCap(); //������Ϊ��λ
            CardExistFlag = 1; //�п���־
            RWCardError = 0; //��card��д�����־

            Formatcap_cmd_info_DiskCard.CurrentCap = CardCapacity;
            Formatcap_cmd_info_DiskCard.FomatableCap = CardCapacity;
            Cap_cmd_info_DiskCard.LastLogBlockAddr = CardCapacity - 1;
        }
        else
        {
            CardExistFlag = 0;
        }
    }
    IE0 = reg_bak;
}

/*
 *********************************************************************************************************
 *                                           UDiskGetDiskInfo
 *
 * Description: ȡ�ô�����Ŀ���,Ҫ�ڵ���UdiskInit�����.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
int8 UDiskGetDiskInfo(void)
{
    int8 cDiskInfo;

    cDiskInfo = 0;
    if (SplitDiskFlag != 0)
    {
        cDiskInfo = cDiskInfo | 0x80; //bit7:�Ƿ����
    }
    if (EncryptFlag != 0)
    {
        cDiskInfo = cDiskInfo | 0x40; //bit6:�Ƿ����
    }
    if (CardExistFlag != 0)
    {
        cDiskInfo = cDiskInfo | 0x20; //bit5:�Ƿ�忨
    }

    return cDiskInfo;
}

/*
 *********************************************************************************************************
 *                                           UDiskCheckWP
 *
 * Description: ���ô���д�������.
 *
 * Arguments  : e:Card Or Flash.
 *
 * Returns    : l:0 Not Write Protect.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskCheckWP(int8 letter)
{
    uint8 ret;

    ret = 0;
    if (letter == 'C')
    {
        ret = FlashWPFlag;
    }
    if (letter == 'H')
    {
        ret = CardWPFlag;
    }

    return ret;
}

/*
 *********************************************************************************************************
 *                                           UDiskGetStatus
 *
 * Description: Return the Udisk transfer status.
 *
 * Arguments  : None.
 *
 * Returns    : a: the status.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskGetStatus(void)
{
    uint8 ret;

    ret = USB_State;
    if (IsAdfuCondition != 0x00 && TransferFlag != 0)
    {
        ret = TransferFlag;
    }
    return ret;
}

/*
 *********************************************************************************************************
 *                                           DisableSuspend
 *
 * Description: ��ֹsuspend.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void DisableSuspend(void)
{
    DisableSuspendFlag = 0x01;
}

/*
 *********************************************************************************************************
 *                                           AlwaysShowCardDisk
 *
 * Description: ���߼���ʾ���̷�.
 *
 * Arguments  : 0: disable 1: enable.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void AlwaysShowCardDisk(uint8 para)
{
    ShowMultiDiskFlag = para;
}

/*
 *********************************************************************************************************
 *                                           UDiskGetCardStatus
 *
 * Description: ��ѯ���Ƿ����.
 *
 * Arguments  : 0: disable 1: enable.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskGetCardStatus(void)
{
    return CardExistFlag;
}

/*
 *********************************************************************************************************
 *                                           UDiskCardInitInternal
 *
 * Description: ���س�ʼ��.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UDiskCardInitInternal(void)
{
    sCardCapCheck();

    ClearWatchdog();

    CardWPCheck();
}

/*
 *********************************************************************************************************
 *                                           smDelay
 *
 * Description: Delay n ms.
 *
 * Arguments  : e: n.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void smDelay(uint8 mscount)
{
    long count;
    count = 0x00004000 * mscount;
    while (count != 0x00)
    {
        count = count - 1;
    }
}

/*
 ********************************************************************************
 *                           EndianReverse
 * Description : Endian reverse
 * Arguments   : none
 * Returns     : none
 * Notes       :
 ********************************************************************************
 */
void EndianReverse(uint8 *data_p, uint16 data_length)
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

/*
 *********************************************************************************************************
 *                                           CheckTimer
 *
 * Description: �鶨ʱ.
 *
 * Arguments  : hl: ������ָ��; de: 1Hz�仯��¼����; c-1: ����ֵ, ��λΪ1��.
 *
 * Returns    : a: ָ��ʱ���Ƿ��ѵ�: 1:yes; 0:no.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
static uint8 CheckTimer(uint8 *timerrecord, uint8 *countrecord, uint8 count)
{
    uint8 ret = 0;
    uint8 sfrbak;

    sfrbak = SFR_BANK;
    SFR_BANK = BANK_RTC;
    if (RTCTimeS == *timerrecord)
    {
        // not reach one second
        ret = 0;
    }
    else
    {
        *timerrecord = RTCTimeS;
        //�뵽�����ʱ����ֵ�Ƚ�
        if (*countrecord < count)
        {
            *countrecord += 1;
            ret = 0;
        }
        else
        {
            *countrecord = 0;
            ret = 1;
        }
    }
    SFR_BANK = sfrbak;
    return ret;
}

/*
 *********************************************************************************************************
 *                                           sSplitDiskEject
 *
 * Description: ��������ɺ�, ������������.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void sSplitDiskEject(void)
{
    if (EjectFlag == 0x00)
    {
        EjectFlag++;

        //�˳�ǰ�����ֳ��������
        UDiskExit();
        UDiskInitInternal();
        //�ָ�initʱ�����״̬
        USB_State = 0x60;
    }
    else
    {
        if (CheckTimer(&EjectTimeCtr, &PrevRTCTimerValue1, USBEJECTCTRVALUE) != 0x00)
        {
            EjectTimeCtr = 0x00;
            USB_State = 0x00;
            EjectFlag = 0x00;

            UDiskRun(0x00);
        }
    }
}

/*
 *********************************************************************************************************
 *                                           UdiskReady
 *
 * Description: For the next usb transfer.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UdiskReady(void)
{
    uint8 IE0bak;
    //Global Interrupt Disable
    IE0bak = IE0;
    IE0 = IE0 & 0x7f;

    //��״̬��ѯ���̷������жϣ�Ӧ��ȥ��һ����ʾ�����ٴ�USB����֤��ʾ��ʱ
    if (StatusCheckFlag != 0x00)
    {
        //�ܵ������µ�CBWʵ���Ѿ��յ�EPA FIFO��ֻ�迪�ж���Ӧ����,Enable USB Interrupt
        AIE = AIE | 0x01;
    }

    //Global Interrupt Enable
    IE0 = IE0bak;
}

/*
 *********************************************************************************************************
 *                                           UdiskReady
 *
 * Description: For the next usb transfer.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
void UdiskUnReady(void)
{
    //Ϊ�������״̬����ʾ������ӦUSB�ж�,Disable USB Interrupt
    AIE = AIE & (uint8) 0xfe;
}

/*
 *********************************************************************************************************
 *                                           UDiskPullCardOut
 *
 * Description: Handle the card plug out message.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 UDiskPullCardOut(void)
{
    uint8 IE0bak;
    IE0bak = IE0;
    IE0 = IE0 & 0x7f;

    //	mydisable_interrupt();

    //0: mac OSX system    1: windows system
    if (OSType == 0x00)
    {
        //usb disconnect,ʵ����ƻ��ϵͳ�¿��ε����̷���ʧЧ��
        UsbCTRL_STUS |= 0x40;
    }

    //��ID
    if (DRV_DetectUD(1) != 0x20)
    {
        CardExistFlag = 0x00; //��տ����ڱ�־

        //������umodeдʱ���ο������м���д�Ķ��������ж�ؿ��������ᵼ�º�����umodeд����ע��ж�ض���
        //DRV_UnInstall(DRV_SDFF); //ж�ؿ����������⿨��Դû�йرգ����²忨ʱС������
    }

    //��USB�ж�
    AIE = AIE | 0x01;

    //0: mac OSX system    1: windows system
    if (OSType == 0x00)
    {
        //D+ Pull high, ��ʱ��PC �ż�⵽��usb device ����
        UsbCTRL_STUS &= (uint8) 0xbf;
    }
    IE0 = IE0bak;

    //	myenable_interrupt();

    return 0x01;
}

/*
 *********************************************************************************************************
 *                                           CardWPCheck
 *
 * Description: Check is the sdcard is write protected.
 *
 * Arguments  : None.
 *
 * Returns    : None.
 *
 * Note(s)    : None.
 *********************************************************************************************************
 */
uint8 CardWPCheck(void)
{
    CardWPFlag = 0;
    //����������ھͲ����жϿ���д������־����ֹ��������û��װ�ɹ�ʱ������mUD_SDCardWPDetectʱ�����ܷ�
    if (CardExistFlag != 0x00)
    {
        //����������뿨д������־
        //        CardWPFlag = (uint8)UD_SDCardWPDetect();		// not support
    }

    return CardWPFlag;
}


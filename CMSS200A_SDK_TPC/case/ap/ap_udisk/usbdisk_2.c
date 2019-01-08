/*
 ********************************************************************************
 *                       ACTOS
 *                      usbfunctions
 *
 *                (c) Copyright 2002-2009, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : usbdisk_2.c
 * By     : Mars
 * Version: v1.00     first version     2009-11-10 14:05  by Mars
 ********************************************************************************
 */
#include <reg251s.h>
#include "Udisk.h"

#pragma name(UDISK_RC_2)

//extern char DisplayIndi;
//extern char *DispBuf;
//extern ADFU_SysInfo_t sysinfo;
extern uint8 UdiskStatus;
extern uint8 MenuKeyPress;
extern uint8 IsCardOut;
extern bool UDiskRdyFlag;

//ADFU_SysInfo_t *sysinfopt;
//BYTE HoldDispState;
//BYTE ChargeCheckCounter;
//WORD wUDiskDisp[3]={UDISKRDY,UDISKUPL,UDISKDL};

#pragma renamecode(UDISK_RC_2)

void MsgManager(void)
{
    char i;
    if (MenuKeyPress == 1)
    {
        return;
    }
    if ((i = GetSysMsg()) == Msg_KeyEQ)
    {
        if (UdiskStatus == 0)
        {
            MenuKeyPress = 1;
        }
        return;
    }
    else if (i == MSG_SD_OUT)
    {
        IsCardOut = 1;
        return;
    }
    //    else if(i == MSG_RTC2HZ)
    //    {
    //         if(ChargeCheckCounter >= 60)
    //         {
    //             ChargeCheckCounter = 0;
    //             sKY_ChargeGetandSet();
    //             return;
    //         }
    //         ChargeCheckCounter ++;
    //    }
}

void Display(char dispIndi)
{
    
}

bool UDiskInitAll(int para)
{
    //	int i;

    //��װud����
    res_fp = ResOpen("ui30.res");
    ClearScreen(NULL); //��ɰ���
    SetPenColor(Color_WHITE_def);
    //UpdateScreen(NULL);
    ResShowPic(UDISKRDY, 0, 0); //��ʾ���л���

    if (DRV_DetectUD(0x00) == 0xff)
    {
        return 0;
    }
    else
    {
        DRV_ActiveUD(0x00, MODE_FAST); //����ģʽud����
    }

    UDiskSetDiskNum(0);

    //U�̳�ʼ�ջ�,�������:callback����������...
    UDiskInit(0, 3);
    UDiskRun(para); //����USB����
    return 1;

    //	int i;
    //
    //	//��װud����
    //	res_fp = ResOpen("ui30.res");
    //	ClearScreen(NULL); //��ɰ���
    //	UpdateScreen(NULL);
    //	ResShowPic(UDISKRDY, 0, 0); //��ʾ���л���
    //
    //	if (DRV_DetectUD(0x00) == 0xff)
    //		return 0;
    //	else
    //		DRV_ActiveUD(0x00, MODE_FAST); //����ģʽud����
    //
    //	//read comval
    //	VMRead(&g_comval, VM_SYSTEM, sizeof(g_comval)); //��VRAM
    //
    //	//check if valid
    //	if (g_comval.magic != MAGIC_COMVAL) //ȡ����ģʽ�趨
    //		COMVAL_INIT(g_comval);
    //
    //	if (g_comval.LightTime != 0)
    //		sKY_OpenBacklight(); //������
    //
    //	//==================================================================================
    //	//**********************���º������ô����ܸĶ�***************************************
    //	//==================================================================================
    //	UDiskSetDiskNum(g_comval.OnlineMode);       //��������ģʽ
    //	//UDiskSetDiskNum(3);                         //���õ���������ģʽ
    //
    //	AlwaysShowCardDisk(g_comval.SuppCard);      //�����Ƿ���ʾ���̷�  //reagan modify 2009-1-6 13:39
    //
    //    //U�̳�ʼ��,�������:callback����,�Ƿ�֧��Udisk���к�(�����,ÿ��Udisk����һ��),�Ƿ���Udisk��֤ģʽ
    //	UDiskInit((void *) FlashLed, g_comval.udisk_setting);
    //
    //	if (sKY_HoldCheck()) //HOLD��ʾ��Ҫд����
    //	{
    //		UDiskSetWP('C');
    //		HoldDispState = 1;
    //		ResShowPic(UDLOCK_S, 115, 16);
    //	}
    //	/*    #ifdef SupportCard
    //	 if (CardWPCheck())            //sCardWPCheck����ֵ��1:�п����ҿ�д����  0:û����û��д����
    //	 { //�п�
    //	 UDiskSetWP('H');
    //	 HoldDispState=1;
    //	 ResShowPic(UDLOCK_S,115,16);
    //	 }
    //	 #endif */
    //
    //	UDiskRun(para); //����USB����
    //
    //	return 1;
}

//void FlashLed(void)
///{
//   memset(&sysinfo,0,sizeof(ADFU_SysInfo_t));
//}

/*
 ********************************************************************************
 *                       ACTOS
 *                 show logo when boot
 *
 *                (c) Copyright 2003-2009, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : welcome.c
 * By     : fiona.yang
 ********************************************************************************
 */
#include "AS260X.h"
//#include <intrins.h>
#include "typeext.h"
//#include "lcd.h"
#include "stringH.h"
#include "display.h"
#include "key.h"

#define WELCOME_DATA_ADD     0xFF2400
//#define GPIO_MODE
extern void _nop_(void);
//void Delay_ms(uint8 delay_ms);
uint8 GetUsbCableStatus(void);

extern void Hdrv_HardInit(void);
extern void Sdrv_LEDDisplay(uint8 api_no, uint8 addr, uint8 content, uint8 type);
extern void Sdrv_LEDPuts(uint8 api_no, uint8 addr, uint8 *str, uint8 type);
extern void sAdfuEnter(void);
void Delay_ms(uint8 delay_ms);

//#define SERIAL_LCM
void charge_pump_ctl(void);

void Show_welcome_logo(void);

void mcu_enter_sleep(void);
void uart_init(void);

#pragma name(CODEDISPSTART)
uint8 cesel;
uint8 com_num;

void DispStarting(void)
{
    uint8 sfrbak;

#ifdef EVB_KEY
    uint8 keyval;
#endif
    uint8 loopcnt;
    uint8 onoff_status;
    uint16 sec_cnt = 0;

    sfrbak = SFR_BANK;

	uart_init();

    SFR_BANK = BANK_GPIO;
    MFP_CTL2 |= 0x01; //SET UDI
    //检测硬开关是否关闭
    SFR_BANK = BANK_PMU;
    onoff_status = SYSTEM_ONOFF & 0x10;
    SFR_BANK = sfrbak;
    if(onoff_status != 0)
    {
        return;
    }
    //判断是否连接USB线
    if ((GetUsbCableStatus() & 0x40) == 0x40)
    {
        //开机长按mode键进ADFU
        SFR_BANK = BANK_GPIO;
		AD_Select  &= 0xCC;
		AD_Select  |= 0x11;/*gpio_c7 mapping to LRADC5, gpio_a6 mapping to LRADC4*/
		AD_Select1 |= 0x01;/*gpio_g2 mapping to LRADC1(rem_con)*/
        SFR_BANK = BANK_PMU;
        PMUADC_CTL |= 0xa0; //enable LRADC1,ADC FREQUENCY 128Hz
        //check key value
        loopcnt = 10;
        while (loopcnt)
        {
            SFR_BANK = BANK_PMU;
#ifdef EVB_KEY
            keyval = LRADC1_DATA & 0x3f;
            SFR_BANK = sfrbak;
            if ((keyval > 0x02) && (keyval < 0x09))
            {
                sAdfuEnter();
            }
#else
			if ((SYSTEM_ONOFF & 0x08) != 0)
		    {
		      	SFR_BANK = sfrbak; 				
				sAdfuEnter();
		    }

            SFR_BANK = sfrbak;
#endif
            Delay_ms(1);
            loopcnt--;
        }
    }
    SFR_BANK = sfrbak;
}
void charge_pump_ctl(void)
{
    uint8 sfr_bak;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_PMU;
    CP_CTL0 |= 0x81;
    CP_CTL1 |= 0x08;
    SFR_BANK = sfr_bak;
}

//mcu enter low frequent
void mcu_enter_sleep(void)
{
    SFR_BANK = 0x05;
    SYSTEM_CTL &= 0xfe;
}
void Show_welcome_logo(void)
{
    Hdrv_HardInit();
    Sdrv_LEDPuts(0, 0, "LOAD", 1);
}
uint8 GetUsbCableStatus(void)
{
    uint8 sfr_bak, dpdm_status;
    uint8 usb_status;
    sfr_bak = SFR_BANK;
    SFR_BANK = BANK_USB;
    dpdm_status = DPDMCTRL;
    DPDMCTRL = 0x1f;
    usb_status = 0x80;
    while(usb_status--)
    {
        ;
    }
    if((USBSTATUS & 0x18) != 0x18)
    {
        usb_status = 0xff;
        while(usb_status--)
        {
            ;
        }
        if((USBSTATUS & 0x18) != 0x18)
        {
            usb_status = 0x40;
        }
        else
        {
            usb_status = 0x00;
        }
    }
    else
    {
        usb_status = 0x00;
    }
    DPDMCTRL = dpdm_status;

    SFR_BANK = BANK_PMU;
    usb_status |= (TEST_CTL & 0x80);

    SFR_BANK = sfr_bak;
    return usb_status;

}
void Delay_ms(uint8 delay_ms)
{
    uint8 i, j;
    while (delay_ms != 0)
    {
        for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 255; j++)
            {
                _nop_();
            }
        }
        delay_ms--;
    }
}

void uart_init(void)
{
	uint8 sfr_bak;
	uint8 i=0;
	uint16 check_count=0;
	
	sfr_bak = SFR_BANK;
	SFR_BANK = BANK_CMU_RMU;
	MRCR1 &= 0xf7;
	MRCR1 |= 0x08;
	CLKENCTL1 |= 0x01;

	SFR_BANK = BANK_GPIO;
	GPIOCOUTEN &= 0xe7;
	GPIOCINEN &= 0xe7;
    GPIOCDAT &= 0xe7;
	MFP_CTL5 &= 0xf0;
	MFP_CTL5 |= 0x05;

	SFR_BANK = sfr_bak;
	return;
}


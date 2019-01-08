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
 * \file     usound_init.c
 * \brief    .
 * \author   dengtaiping
 * \par      GENERAL DESCRIPTION:
 *               usb、AUDIO等模块初始化
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 *
 *      Copyright(c) 2001-2012 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2012/10/23
 *******************************************************************************/
#include "uart.h"

#pragma name(UART_MSG)
#define max_send_byte 2

const uint8 uart_start_block[8] = 
	{0xFF, 0x55, 0x04, 0x00, 0x38, 0x03, 0xE9, 0xD8};

void uart_send_msg(uint8* msg_buf)
{
	uint8 sfr_temp = SFR_BANK;
	SFR_BANK = BANK_UART;
    while((*msg_buf)!=0)
    {        
        while((UART_STA&(0x01<<6))!=0)
        {
            
        }
        UART_TXDAT=*msg_buf;
        msg_buf++;
    }
	//UART_TXDAT = c;
	while (UART_STA & 0x08)
    {	// wait uart idle
        ;
    }   
	SFR_BANK = sfr_temp;
	return;
}

void uart_send_msg_hex(uint8* str)
{
    uint16 i;
    uint8 dat;
    uint8 remain_read;
    uint8 sfr_temp = SFR_BANK;
    SFR_BANK = BANK_UART;
    while((UART_STA&(0x01<<6))!=0)
    {   
    }
    if(uart_fifo.putP >= max_send_byte)
    {
        remain_read = max_send_byte;  
        uart_fifo.putP = uart_fifo.putP - max_send_byte;
    }
    else
    {
        remain_read = uart_fifo.putP;
        uart_fifo.putP = 0;
    }
    uart_fifo.used += remain_read;
    for (i = 0; i < remain_read; i++)
    {
        dat = *(str + i);
        UART_TXDAT = dat;
    }
    UART_DRQ |= 0x08;	// enable tx irq
	SFR_BANK = sfr_temp;
}

void send_first_msg(void)
{
	uint8 sfr_bak = SFR_BANK;
	uint8 i;
	SFR_BANK = BANK_UART;			
	while((UART_STA&(0x01<<6)) != 0)
	{	// wait until TX FIFO is not full.			  
		;
	}
	
	UART_TXDAT = 0xFF;	// trans the 1st uart data. 	
	while (UART_STA & 0x08) 
	{	// wait uart idle			
		;
	}		
	
	TM_DelayMS(20);
	for (i=0; i<8; i++)
	{
		UART_TXDAT = uart_start_block[i];
	}	
	while (UART_STA & 0x08) 
	{	//wait uart idle
		;
	}
	
	SFR_BANK = sfr_bak;
}

void uart_timer_count(void)
{
	uint8 sfr_temp = SFR_BANK;
	TM_KillTimer(uart_timer_id);
    SFR_BANK = BANK_UART;
	UART_DRQ |= 0x0a;	// enable TX IRQ
	SFR_BANK = sfr_temp;
}

void uart_charge_count(void)
{
	uint8 sfr_temp = SFR_BANK;
	TM_KillTimer(uart_timer_id);
    SFR_BANK = BANK_UART;
	UART_CTL |= 1<<6;	// enable RX
	SFR_BANK = sfr_temp;
}

void delay_timer_count(void)
{
	uint8 sfr_temp = SFR_BANK;
	TM_KillTimer(delay_timer_id);
    SFR_BANK = BANK_UART;
	UART_CTL |= 1<<6;	// enable RX
	SFR_BANK = sfr_temp;
}

void deal_send_msg(void)
{
	uint8 sfr_temp = SFR_BANK;
	
	SFR_BANK = BANK_UART;
	while (UART_STA & 0x08)
    {	// wait uart idle
    	;
    }
    UART_DRQ &= ~(1<<1);	// not TX IRQ pending
	
    /* transmit not completed */
    if(uart_fifo.putP!=0)
    {
        UART_DRQ &= 0xF7;	// disable TX IRQ
        uart_send_msg_hex(data_read_buf + uart_fifo.used);
    }
    else
    {	// transform finished.
		if(trans_last_data_flag == TRUE)
		{
			trans_last_data_flag = FALSE;			
			SFR_BANK = BANK_UART;
			UART_DRQ &= 0xF3;	// disable TX IRQ & RX IRQ
			UART_CTL &=~(1<<6);	// disable RX
			UART_STA |= 1<<1;	// reset the RX FIFO
			SFR_BANK = sfr_temp;

			get_uart_data(uart_fifo.rx_count);
			uart_timer_id = TM_SetTimer((uint32) uart_timer_count, uart_data->time_delay);
			uart_fifo.rx_count = 0xff;
			uart_fifo.flags = 2;
			return;
		}

        if(uart_fifo.rx_count != 0xff)
        {
        	UART_CTL |= 1<<6;	// enable RX
        }           
        uart_fifo.flags = 0;
        uart_fifo.used = 0;
        uart_fifo.time_count = 0;
        UART_DRQ &= 0xF7;	// disable TX IRQ
    }
	SFR_BANK = sfr_temp;
}


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

#pragma name(UART)

uint32 uart_isr_no;
uart_fifo_t uart_fifo;
SD_FILE *uart_data1_fp;
SD_FILE *uart_data2_fp;
uint8* head1_read_buf;
uint8* head2_read_buf;
uint8* data_read_buf;
uart_data_t* uart_data;
uint8 uart_timer_id;
uint8 delay_timer_id;
uint8 trans_1st_finish_flag;
uint8 trans_2nd_start_flag;
uint8 trans_last_data_flag;
uint8 tmp_count;
uint8 drop_flag;
void get_uart_data(uint8 index);
void init_uart_info(void);
void send_first_msg(void);

const uint8 uart_data1_str[] = "U_DATA1.BIN";
const uint8 uart_data2_str[] = "U_DATA2.BIN";

uint8 uart_init(void)
{
	uint8 sfr_temp = SFR_BANK;

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

	SFR_BANK = BANK_UART;
	UART_BR = 0x05;
	UART_MODE = 0x00;
	UART_CTL = 0xc0;	// enable TX & RX
	UART_DRQ = 0x05;	// disable TX IRQ, enable RX IRQ
	SFR_BANK = sfr_temp;

	send_first_msg();
	
	init_uart_info();
	memset(&uart_fifo, 0x0, UART_FIFO_CNT);
	trans_1st_finish_flag = FALSE;
	trans_2nd_start_flag  = FALSE;
	trans_last_data_flag  = FALSE;
	tmp_count = 0;
	drop_flag = FALSE;
	SFR_BANK = BANK_UART;
	IE0 &= 0x7f;
	uart_isr_no = IRQ_Intercept((uint32) uart_isr, IRQ_UART);
	IE0 = IE0 | 0x80;
	IE0 |= 0x10; //enable UART/IRC interrupt
	SFR_BANK = sfr_temp;

    return 1;
}

void uart_exit(void)
{
	IRQ_Release(uart_isr_no, IRQ_UART);
	SD_FClose(uart_data2_fp);
	SD_FClose(uart_data1_fp);
	return;
}

void init_uart_info(void)
{   
    head1_read_buf = (uint8*)0x1180;
    head2_read_buf = (uint8*)0x1280;
    data_read_buf = (uint8*)0x1380;
    uart_data1_fp = SD_FOpen(uart_data1_str, MODE_READ);
    SD_FRead(uart_data1_fp, head1_read_buf, 256);
    uart_data2_fp = SD_FOpen(uart_data2_str, MODE_READ);
    SD_FRead(uart_data2_fp, head2_read_buf, 256);
}

void get_uart_data(uint8 index)
{    
    if(index >= TOTAL_COUNT_NUM)
    {
       return;
    }
    
    if (uart_fifo.rx_count < SKIP_COUNT_NUM)
	{
		uart_data = (uart_data_t*)&head1_read_buf[index * sizeof(uart_data_t)];
		SD_FSeek(uart_data1_fp, SEEK_SET, (0x100 + uart_data->offset));
		memset(data_read_buf, 0x0, (uart_data->length + 2));
		SD_FRead(uart_data1_fp, data_read_buf, uart_data->length);
	}
	else
	{
		uart_data = (uart_data_t*)&head2_read_buf[(index-SKIP_COUNT_NUM) * sizeof(uart_data_t)];
	    SD_FSeek(uart_data2_fp, SEEK_SET, (0x100 + uart_data->offset));
	    memset(data_read_buf, 0x0, (uart_data->length + 2));
	    SD_FRead(uart_data2_fp, data_read_buf, uart_data->length);
	}
    uart_fifo.putP = uart_data->length; 
    uart_fifo.used = 0;
}

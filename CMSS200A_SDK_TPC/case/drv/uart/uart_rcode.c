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

#pragma name(UART_RCODE)
//const uint8 charge_command[]={0xFF,0x55,0x05,0x06,0x04};
const uint8 two_stage_command[]={0xFF,0x55,0x04,0x00,0x14};

void uart_isr(void)
{
	uint8 sfr_temp = SFR_BANK;
	uint8 need_clr;   

    SFR_BANK = BANK_UART;
    if(uart_fifo.flags == 2)
    {
		deal_send_msg();
        return;        
    }
 
 	need_clr = FALSE;
	
	drop_flag = FALSE;
    while ((UART_STA & 0x20) == 0)
	{	// RX fifo not empty.
		uart_fifo.buf[tmp_count] = UART_RXDAT;
		need_clr = TRUE;      
		
		if((trans_1st_finish_flag==TRUE) && (trans_2nd_start_flag==FALSE))
		{
			/* compare whether or not the 2nd stage 1st command */
			if(uart_fifo.buf[tmp_count] != two_stage_command[tmp_count])
			{	 
				drop_flag = TRUE;
				tmp_count = 5;
				break;
			}
			else
			{
				drop_flag = FALSE;
			}				

			tmp_count++;
			if(tmp_count >= 5)
	        {	// compare 5 bytes.
	        	if(drop_flag == FALSE)
	        	{
	        		trans_2nd_start_flag = TRUE;
	        	}
	            break;
	        }
		}
		else
		{
			drop_flag = FALSE;
			tmp_count = 5;
			break;
		}										      
	} 
	
	if(need_clr == TRUE)
	{
		if((trans_1st_finish_flag==TRUE) && (tmp_count<5))
		{	// not recieve enough RX data.
			SFR_BANK = sfr_temp; 
			return;
		}
		UART_CTL &= ~(1<<6);	// disable RX
	    UART_STA |= 1<<1;		// reset the RX FIFO
	    while ((UART_STA & 0x20) == 0)
	    {	// RX fifo not empty.							   
	        ;
	    }	

		if((drop_flag==TRUE) && (tmp_count>=5))
		{	// not transmit TX data.
			UART_DRQ &= 0xF7;	// disable TX IRQ
	        UART_DRQ |= 0x01;	// clear RX IRQ pending bit	
			memset(uart_fifo.buf, 0x0, 6);
			uart_fifo.flags = 0;
			tmp_count = 0;
			uart_timer_id = TM_SetTimer((uint32) uart_charge_count, 10);
			SFR_BANK = sfr_temp; 
			return;
		}
		uart_fifo.flags = 1;
	}	
	SFR_BANK = sfr_temp; 
	
    if(uart_fifo.flags == 1)
    {
		tmp_count = 0;
		drop_flag = FALSE;
		if ((uart_fifo.rx_count==SKIP_COUNT_NUM) && (trans_1st_finish_flag==FALSE))
		{
			SFR_BANK = BANK_UART;
			UART_DRQ &= 0xF7;	// disable TX IRQ
	        UART_DRQ |= 0x01;	// clear RX IRQ pending bit
	    	SFR_BANK = sfr_temp; 
			trans_1st_finish_flag = TRUE;
			uart_fifo.flags = 0;
			delay_timer_id = TM_SetTimer((uint32)delay_timer_count, 20);
        
			return;
		}
    	if(uart_fifo.rx_count == LAST_COUNT_NUM)
        {
        	trans_last_data_flag = TRUE;
        }
		
        SFR_BANK = BANK_UART;
        UART_DRQ |= 0x01;	// clear RX IRQ pending bit
    	SFR_BANK = sfr_temp; 
        get_uart_data(uart_fifo.rx_count);
		uart_timer_id = TM_SetTimer((uint32) uart_timer_count, uart_data->time_delay);
        uart_fifo.rx_count++;		
        uart_fifo.flags = 2;
    }	
}



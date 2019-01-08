#ifndef _UART_H
#define _UART_H

#include "actos.h"
#include "key.h"
#include "stringH.h"

#ifdef 	SUPPORT_CHARGE_2A
#define SKIP_COUNT_NUM	14
#define LAST_COUNT_NUM	21
#define TOTAL_COUNT_NUM	23
#else
#define SKIP_COUNT_NUM	11
#define LAST_COUNT_NUM	18
#define TOTAL_COUNT_NUM	20
#endif

#define UART_FIFO_CNT 10

typedef struct  
{  
     uint8 buf[UART_FIFO_CNT];  
     uint8 putP;
     uint8 flags;  
     uint8 used;
     uint16 rx_count;
     uint16 time_count;
} uart_fifo_t;
// 菜单头控件有关的定义
typedef struct
{
    uint16 offset;  //数据存放的偏移
    uint16 length;  //该段数据的长度
    uint16 time_delay;
} uart_data_t;


extern uint32 uart_isr_no;
extern uart_fifo_t uart_fifo;
extern SD_FILE *uart_data1_fp;
extern SD_FILE *uart_data2_fp;
extern uint8* head_read_buf;
extern uint8* data_read_buf;
extern uart_data_t* uart_data;
extern uint8 uart_timer_id;
extern uint8 delay_timer_id;
extern uint8 trans_1st_finish_flag;
extern uint8 trans_2nd_start_flag;
extern uint8 trans_last_data_flag;
extern uint8 tmp_count;
extern uint8 drop_flag;
void uart_charge_count(void);
void init_uart_info(void);
void uart_send_msg(uint8* msg_buf);
uint8 uart_init(void);
void uart_isr(void);
void get_uart_data(uint8 index);

void uart_send_msg_hex(uint8* str);
void uart_timer_count(void);
void delay_timer_count(void);
uint8 check_gpioa6(void);
void send_first_msg(void);
void deal_send_msg(void);


#endif /* _SDRV_S04311_H */


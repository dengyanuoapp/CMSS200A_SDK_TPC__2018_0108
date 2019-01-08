#ifndef _SDRV_KEY_H
#define _SDRV_KEY_H

#include "actos.h"
#include "key.h"
#include "time.h"
#include "stringH.h"

#ifdef __C251__
#define NO_KEY 0xff

//用于检测电池电量
#define BAT_CHECK_GPIO_INPUT GPIOAINEN
#define BAT_CHECK_GPIO_DAT GPIOADAT
#define BAT_CHECK_GPIO_NUM    6

//用于红色指示灯开关
#define RED_LIGHT_GPIO_OUTPUT GPIODOUTEN
#define RED_LIGHT_GPIO_DAT GPIODDAT
#define RED_LIGHT_GPIO_NUM    1

//用于蓝色指示灯开关
#define BLUE_LIGHT_GPIO_OUTPUT GPIOCOUTEN
#define BLUE_LIGHT_GPIO_DAT GPIOCDAT
#define BLUE_LIGHT_GPIO_NUM    3

#define linein_min_adcval   0x34    //2.6V
#define linein_max_adcval   0x3f   //3.3V

extern uint8 TheCharge; //充电标识，0,stop；1，start，charge full
extern uint8 oldkey;
extern uint8 tmpkey;
extern uint8 tmp_count;
extern uint8 hold_count;
extern uint8 wait_keyup;

extern uint8 const key_map[16];
extern uint8 IR_KEY;
extern uint8 Low_power_count;
extern uint8 adcdat;
extern uint8 onoff_flag;

// 函数表

void ADC_DAC_init(void);
extern void _nop_(void);
void ctc_isr(void);
void IR_Interrupt(void);
void init_ir(void);
void delay(void);
void key_scan(void);
void Delay10ms(void); //24Mhz，延时500ms
void ADC_DAC_loop(void);
void init_bat_gpio(void);
uint8 sKY_GetChargeStatus(uint8 api_no);
void open_red_light(uint8 flag);
void switch_red_light(uint8 flag);
void switch_blue_light(uint8 flag);
void init_ADSelect0(void);
void adjust_key_sequence();


#endif //__C251__
#endif /* _SDRV_S04311_H */


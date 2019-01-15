/*
 *  *********************************************
 *  Usage : replace the main() as following 
 *  *********************************************
 *
// defined "_useMyMain01" inside the following h file
#include    "ap_main_patch01.h"

#endif
 *
 */

#ifndef     _useMyMain01
#define     _useMyMain01

// gpioA5 -> uartCTS,(in) ; // gpioA6 -> uartRTS,(out) ;
#define _maskRTS     0x40
#define _set_to_RTS_0       GPIOADAT &= (~_maskRTS) ; // set to 0
#define _set_to_RTS_1       GPIOADAT |=   _maskRTS  ; // set to 1
#define _maskCTS     0x20
#define _set_to_CTS_0       GPIOADAT &= (~_maskCTS) ; // set to 0
#define _set_to_CTS_1       GPIOADAT |=   _maskCTS  ; // set to 1

#define _set_dir_CTS_i      { GPIOAOUTEN &= (~_maskCTS) ; GPIOAINEN  |=   _maskCTS  ; }
#define _set_dir_RTS_i      { GPIOAOUTEN &= (~_maskRTS) ; GPIOAINEN  |=   _maskRTS  ; }
#define _set_dir_CTS_o      { GPIOAOUTEN |=   _maskCTS  ; GPIOAINEN  &= (~_maskCTS) ; }
#define _set_dir_RTS_o      { GPIOAOUTEN |=   _maskRTS  ; GPIOAINEN  &= (~_maskRTS) ; }

// gpioC3 -> RX, gpioC4 -> TX 
#define _maskTX      0x10
#define _set_to_TX_0       GPIOCDAT &= (~_maskTX) ; // set to 0
#define _set_to_TX_1       GPIOCDAT |=   _maskTX  ; // set to 1
#define _maskRX      0x08

// I2S0 : bclk E0 , lrclk E1 , mclk E2
#define _maskI2Sbclk      0x01
#define _maskI2Slrclk     0x02
#define _maskI2Smclk      0x04
#define _set_to_I2Sbclk_0       GPIOEDAT &= (~_maskI2Sbclk   )  ; // set to 0
#define _set_to_I2Slrclk_0      GPIOEDAT &= (~_maskI2Slrclk   ) ; // set to 0
#define _set_to_I2Smclk_0       GPIOEDAT &= (~_maskI2Smclk   )  ; // set to 0
#define _set_to_I2Sbclk_1       GPIOEDAT |=   _maskI2Sbclk      ; // set to 1
#define _set_to_I2Slrclk_1      GPIOEDAT |=   _maskI2Slrclk     ; // set to 1
#define _set_to_I2Smclk_1       GPIOEDAT |=   _maskI2Smclk      ; // set to 1

#define _set_dir_I2Sbclk_o      { GPIOEOUTEN |=   _maskI2Sbclk      ; GPIOEINEN  &= (~_maskI2Sbclk    ) ; }
#define _set_dir_I2Slrclk_o     { GPIOEOUTEN |=   _maskI2Slrclk     ; GPIOEINEN  &= (~_maskI2Slrclk   ) ; }
#define _set_dir_I2Smclk_o      { GPIOEOUTEN |=   _maskI2Smclk      ; GPIOEINEN  &= (~_maskI2Smclk    ) ; }
#define _set_dir_I2Sbclk_i      { GPIOEOUTEN &= (~_maskI2Sbclk    ) ; GPIOEINEN  |=   _maskI2Sbclk      ; }
#define _set_dir_I2Slrclk_i     { GPIOEOUTEN &= (~_maskI2Slrclk   ) ; GPIOEINEN  |=   _maskI2Slrclk     ; }
#define _set_dir_I2Smclk_i      { GPIOEOUTEN &= (~_maskI2Smclk    ) ; GPIOEINEN  |=   _maskI2Smclk      ; }

#include "test01_uart_and_delay.h"
#include "test02_uart_as_start_detect.h"
#include "test05_combine_start_detect.h"

void _mPatch01( void ) {

    /*
    SetPLL(PLL_48MHZ);
    SetVDD(VDDSET_1_6V);
    ClearWatchDog();
    */

    while ( 1 ) {
        ClearWatchDog();
        //if ( _test01_uart_and_delay() ) return ;
        //if ( _test02_uartRX_as_start_detect() ) return ;
        //if ( _test02_uartTX_as_start_detect() ) return ;
        //if ( _test02_uartCTS_as_start_detect() ) return ;
        if ( _test05_combine_start_detect() ) return ;
    }
} // _mPatch01

#endif

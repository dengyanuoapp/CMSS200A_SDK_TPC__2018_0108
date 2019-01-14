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

#define _maskTX      0x10
#define _maskRX      0x08

#include "test01_uart_and_delay.h"
#include "test02_uart_as_start_detect.h"

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
        if ( _test02_uartTX_as_start_detect() ) return ;
    }
} // _mPatch01

#endif

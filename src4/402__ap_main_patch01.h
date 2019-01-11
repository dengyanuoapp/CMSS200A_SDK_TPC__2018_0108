/*
 *  *********************************************
 *  Usage : replace the main() as following 
 *  *********************************************
 *
// defined "_useMyMain01" inside the following h file
#include    "ap_main_patch01.h"

int16 main(int16 param)
{
#ifdef _useMyMain01
_mPatch01();
}
int16 main_fake(int16 param)
{
#endif
 *
 */

#ifndef     _useMyMain01
#define     _useMyMain01

#define _maskTX      0x10
#define _maskRX      0x08

#include "test01_uart_and_delay.h"
#include "test02_uart_as_start_detect.h"

int16 main_fake(int16 param);
void _mPatch01(void) {

    SetPLL(PLL_48MHZ);
    SetVDD(VDDSET_1_6V);
    ClearWatchDog();

    while ( 0 ) {
        ClearWatchDog();
        //if ( _test01_uart_and_delay() ) return ;
        if ( _test02_uart_as_start_detect() ) return ;
    }

    // call the real main loop
    main_fake(param);

} // _mPatch01

#endif

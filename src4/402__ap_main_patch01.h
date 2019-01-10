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

define _maskTX      0x10
define _maskRX      0x08
// test_uart
void _test_uart01(void)
{
    uint8 sfr_bak;

    sfr_bak = SFR_BANK;

    SFR_BANK = BANK_GPIO;
    GPIOCOUTEN |= _maskTX ;
    GPIOCINEN  |= _maskRX ;

    while ( 1 ) {
        if (GPIOCDAT & _maskRX )
        {
            GPIOCDAT   |=  _maskTX ;
        } else {
            GPIOCDAT &= (~ _maskTX);
        }
        ClearWatchDog();
    }
    SFR_BANK = sfr_bak;
} // _test_uart

void _mPatch01(void) {
    while ( 1 ) {
        ClearWatchDog();
        _test_uart01();
    }
} // _mPatch01

#endif

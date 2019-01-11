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
// test_uart
uint8 _test_uart01(void)
{
    uint8 __sfr_bak01;
    uint8 __gpiocOUTen01;
    uint8 __gpiocINen01;
    uint32 __tUartCnt01;

    __sfr_bak01    = SFR_BANK;
    __gpiocOUTen01 = GPIOCOUTEN ;
    __gpiocINen01  = GPIOCINEN  ;

    SFR_BANK = BANK_GPIO;
    GPIOCOUTEN |= _maskTX ;
    GPIOCINEN  |= _maskRX ;

    __tUartCnt01 = 0 ;
    while ( 1 ) {
#if 1
        if (GPIOCDAT & _maskRX )
        {
            //if ( __tUartCnt01 > 1000 ) { // 1000 : 1.63 ms , 61349 : 100 ms
            if ( __tUartCnt01 > 61349 ) { // 1000 : 1.63 ms , 61349 : 100 ms
                __tUartCnt01 = 0 ;
                GPIOCDAT   |=  _maskTX ;
            } else {
                __tUartCnt01 ++ ;
                GPIOCDAT &= (~ _maskTX);
            }
        } else {
            GPIOCDAT &= (~ _maskTX);
            __tUartCnt01 = 0 ;
        }
#else
        GPIOCDAT   ^=  _maskTX ;
#endif
        ClearWatchDog();
    }

    GPIOCOUTEN = __gpiocOUTen01 ;
    GPIOCINEN  = __gpiocINen01 ;
    SFR_BANK   = __sfr_bak01;
    return 0 ;
} // _test_uart01

void _mPatch01(void) {

    SetPLL(PLL_48MHZ);
    SetVDD(VDDSET_1_6V);
    ClearWatchDog();

    while ( 1 ) {
        ClearWatchDog();
        if ( _test_uart01() ) return ;
    }
} // _mPatch01

#endif

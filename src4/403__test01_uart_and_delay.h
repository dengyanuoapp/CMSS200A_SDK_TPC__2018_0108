
#ifndef     __test01_uart_and_delay_h
#define     __test01_uart_and_delay_h

// test_uart
uint8 _test01_uart_and_delay(void)
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
            // 1000 : 1.63 ms , 61349 : 100 ms
            if ( __tUartCnt01 > 61349 ) { 
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
} // _test01_uart_and_delay
#endif

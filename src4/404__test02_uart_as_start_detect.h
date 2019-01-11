
#ifndef     __test02_uart_as_start_detect_h
#define     __test02_uart_as_start_detect_h

// test_uart
uint8 _test02_uart_as_start_detect(void)
{
    uint8 __sfr_bak01;
    uint8 __gpiocINen01;
    uint32 __tUartCnt01;

    __sfr_bak01    = SFR_BANK;
    __gpiocINen01  = GPIOCINEN  ;

    SFR_BANK = BANK_GPIO;
    GPIOCINEN  |= _maskRX ;

    __tUartCnt01 = 0 ;
    while ( 1 ) {
        if (GPIOCDAT & _maskRX )
        {
            // 1000 : 1.63 ms , 61349 : 100 ms
            if ( __tUartCnt01 > 61349 ) { 
                //__tUartCnt01 = 0 ;
                break ;
            } else {
                __tUartCnt01 ++ ;
            }
        } else {
            __tUartCnt01 = 0 ;
        }
        ClearWatchDog();
    }

    GPIOCINEN  = __gpiocINen01 ;
    SFR_BANK   = __sfr_bak01;
    return 1 ;
} // _test02_uart_as_start_detect
#endif

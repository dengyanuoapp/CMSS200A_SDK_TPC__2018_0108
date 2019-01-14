
#ifndef     __test02_uart_as_start_detect_h
#define     __test02_uart_as_start_detect_h

_reset01( void ) {
    IE0 &= 0x7f;
    SFR_BANK = BANK_RTC;
    WDCTL =  0x8a; //watchdog使能并重启
    while (1) { }
} // _reset01

// test_uart
uint8 _test02_uartRX_as_start_detect(void)
{
    uint8 __sfr_bak01;
    uint8 __gpiocINen01;
    uint32 __tUartCnt01;

    __sfr_bak01    = SFR_BANK;
    __gpiocINen01  = GPIOCINEN  ;

    SFR_BANK = BANK_GPIO;
    GPIOCINEN  |= _maskRX ;

    if (GPIOCDAT & _maskRX ) {
        return 1 ; // skip , to run normal main.
    }

    __tUartCnt01 = 0 ;
    while ( 1 ) {
        if (GPIOCDAT & _maskRX )
        {
            // 1000 : 1.63 ms , 61349 : 100 ms
            if ( __tUartCnt01 > 61349 ) { 
                //__tUartCnt01 = 0 ;
                _reset01();
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
} // _test02_uartRX_as_start_detect

uint8 _test02_uartTX_as_start_detect(void)
{
    uint8 __sfr_bak02;
    uint8 __gpiocINen02;
    uint8 __gpiocOUTen02;
    uint32 __tUartCnt02;

    __sfr_bak02    = SFR_BANK;
    __gpiocINen02  = GPIOCINEN  ;
    __gpiocOUTen02 = GPIOCOUTEN ;

    SFR_BANK = BANK_GPIO;
    GPIOCOUTEN &= (~_maskTX) ;
    GPIOCINEN  |= _maskTX ;

    for ( __tUartCnt02 = 1000 ; __tUartCnt02  ; __tUartCnt02 --   ) { 
    }

    if (GPIOCDAT & _maskTX ) {
        return 1 ; // skip , to run normal main.
    }

    __tUartCnt02 = 0 ;
    while ( 1 ) {
        if (GPIOCDAT & _maskTX )
        {
            // 1000 : 1.63 ms , 61349 : 100 ms
            if ( __tUartCnt02 > 61349 ) { 
                //__tUartCnt02 = 0 ;
                _reset01();
                break ;
            } else {
                __tUartCnt02 ++ ;
            }
        } else {
            __tUartCnt02 = 0 ;
        }
        ClearWatchDog();
    }

    GPIOCOUTEN = __gpiocOUTen02 ;
    GPIOCINEN  = __gpiocINen02  ;
    SFR_BANK   = __sfr_bak02    ;
    return 1 ;
} // _test02_uartTX_as_start_detect

#endif

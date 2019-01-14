
#ifndef     __test05_combine_start_detect_h
#define     __test05_combine_start_detect_h

uint8 _test05_combine_start_detect(void)
{
    uint8 __sfr_bak05;
    uint8 __gpiocINen05;
    uint8 __gpiocOUTen05;
    uint32 __tUartCnt05;

    __sfr_bak05    = SFR_BANK;
    __gpiocINen05  = GPIOAINEN  ;
    __gpiocOUTen05 = GPIOAOUTEN ;

    SFR_BANK = BANK_GPIO;

    GPIOAOUTEN &= (~_maskCTS) ;
    GPIOAINEN  |= _maskCTS ;

    GPIOAOUTEN |= _maskRTS ;
    GPIOAINEN  &= (~_maskRTS) ;

    _set_to_RTS_1 ;

    for ( __tUartCnt05 = 10 ; __tUartCnt05  ; __tUartCnt05 --   ) { 
    }

    if (GPIOADAT & _maskCTS ) {
        _set_to_RTS_0 ;
        return 1 ; // skip , to run normal main.
    }

    __tUartCnt05 = 0 ;
    while ( 1 ) {
        if (GPIOADAT & _maskCTS )
        {
            // 1000 : 1.63 ms , 61349 : 100 ms
            if ( __tUartCnt05 > 613 ) { 
                //__tUartCnt05 = 0 ;
                _set_to_RTS_0 ;
                _reset01();
                break ;
            } else {
                __tUartCnt05 ++ ;
            }
        } else {
            __tUartCnt05 = 0 ;
        }
        ClearWatchDog();
    }

    GPIOAOUTEN = __gpiocOUTen05 ;
    GPIOAINEN  = __gpiocINen05  ;
    SFR_BANK   = __sfr_bak05    ;
    return 1 ;
} // _test05_combine_start_detect

#endif

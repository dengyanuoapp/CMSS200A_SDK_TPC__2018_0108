
#ifndef     __test03_combine_start_detect_h
#define     __test03_combine_start_detect_h

uint8 _test03_combine_start_detect(void)
{
    uint8 __sfr_bak03;
    uint8 __gpiocINen03;
    uint8 __gpiocOUTen03;
    uint32 __tUartCnt03;

    __sfr_bak03    = SFR_BANK;
    __gpiocINen03  = GPIOAINEN  ;
    __gpiocOUTen03 = GPIOAOUTEN ;

    SFR_BANK = BANK_GPIO;
    GPIOAOUTEN &= (~_maskCTS) ;
    GPIOAINEN  |= _maskCTS ;

    for ( __tUartCnt03 = 1000 ; __tUartCnt03  ; __tUartCnt03 --   ) { 
    }

    if (GPIOADAT & _maskCTS ) {
        return 1 ; // skip , to run normal main.
    }

    __tUartCnt03 = 0 ;
    while ( 1 ) {
        if (GPIOADAT & _maskCTS )
        {
            // 1000 : 1.63 ms , 61349 : 100 ms
            if ( __tUartCnt03 > 61349 ) { 
                //__tUartCnt03 = 0 ;
                _reset01();
                break ;
            } else {
                __tUartCnt03 ++ ;
            }
        } else {
            __tUartCnt03 = 0 ;
        }
        ClearWatchDog();
    }

    GPIOAOUTEN = __gpiocOUTen03 ;
    GPIOAINEN  = __gpiocINen03  ;
    SFR_BANK   = __sfr_bak03    ;
    return 1 ;
} // _test03_combine_start_detect

#endif

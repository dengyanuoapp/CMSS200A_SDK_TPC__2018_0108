
#ifndef     __test05_combine_start_detect_h
#define     __test05_combine_start_detect_h

uint8 _test05_combine_start_detect(void)
{
    uint8 __sfr_bak05;
    uint32 __tUartCnt05;

    uint8 __gpioaINen05;
    uint8 __gpioaOUTen05;
    uint8 __gpioeINen05;
    uint8 __gpioeOUTen05;

    __sfr_bak05    = SFR_BANK;
    __gpioaINen05  = GPIOAINEN  ;
    __gpioaOUTen05 = GPIOAOUTEN ;
    __gpioeINen05  = GPIOEINEN  ;
    __gpioeOUTen05 = GPIOEOUTEN ;

    SFR_BANK = BANK_GPIO;

    _set_dir_CTS_i      ;
    _set_dir_RTS_o      ;

    // RTS set to output
    _set_dir_I2Sbclk_o      ;
    _set_dir_I2Slrclk_o     ;
    _set_dir_I2Smclk_o      ;


    _set_to_RTS_1 ;

    // BCLK

    for ( __tUartCnt05 = 100 ; __tUartCnt05  ; __tUartCnt05 --   ) { 
    }

    if (GPIOADAT & _maskCTS ) {
        _set_to_RTS_0 ;

        _set_to_I2Sbclk_0       ;
        _set_to_I2Slrclk_0      ;
        _set_to_I2Smclk_0       ;
        for ( __tUartCnt05 = 100 ; __tUartCnt05  ; __tUartCnt05 --   ) { }
        _set_to_I2Sbclk_1       ;
        _set_to_I2Slrclk_1      ;
        _set_to_I2Smclk_1       ;

        return 1 ; // skip , to run normal main.
    }

    __tUartCnt05 = 0 ;
    while ( 1 ) {
        if (GPIOADAT & _maskCTS )
        {
            // 1000 : 1.63 ms , 61349 : 100 ms
            if ( __tUartCnt05 > 813 ) { 
                //__tUartCnt05 = 0 ;
                _set_to_RTS_0 ;

                _set_to_I2Sbclk_0       ;
                _set_to_I2Slrclk_0      ;
                _set_to_I2Smclk_0       ;
                for ( __tUartCnt05 = 100 ; __tUartCnt05  ; __tUartCnt05 --   ) { }
                _set_to_I2Sbclk_1       ;
                _set_to_I2Slrclk_1      ;
                _set_to_I2Smclk_1       ;

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

    GPIOAOUTEN = __gpioaOUTen05 ;
    GPIOAINEN  = __gpioaINen05  ;
    GPIOEOUTEN = __gpioeOUTen05 ;
    GPIOEINEN  = __gpioeINen05  ;
    SFR_BANK   = __sfr_bak05    ;
    return 1 ;
} // _test05_combine_start_detect

#endif

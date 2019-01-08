/********************************************************************************
 *                              AS211A
 *                            Module: USB MAIN
 *                 Copyright(c) 2001-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *      wanghaijing 2010-3-10         1.0                build this file
 ********************************************************************************/
/*!
 * \file     usbdisk.c
 * \brief
 * \author   wanghaijing
 * \par      GENERAL DESCRIPTION:
 *               这里对文件进行描述
 * \par      EXTERNALIZED FUNCTIONS:
 *               这里描述调用到外面的模块
 *
 *      Copyright(c) 2001-2010 Actions Semiconductor, All Rights Reserved.
 *
 * \version 1.0
 * \date  2010-3-10
 *******************************************************************************/

#include "utest.h"

#pragma name(AP_MAIN)

#pragma renamecode(?PR?AP_MAIN)

int16 main(void)
{
    UTestRun();

    UTestExit();

    return RESULT_NULL;
}


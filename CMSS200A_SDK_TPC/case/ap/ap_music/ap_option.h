/*
 ******************************************************************************* *                               AS211A
 *                              AS211A
 *                              Module: music ap
 *                 Copyright(c) 2002-2010 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * Descripton: the head file of music
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       xusong    2010-07-14 11:00     1.0             build this file
 *******************************************************************************
 */
//通过这个文件来配置一些功能,以适合不同的方案使用
#define SuPPORTKARAOKE          //是否支持卡拉OK功能
//#define OPT_SUP_ID3                   //是否支持ID3显示
//#define OPT_SUP_SCROLL            //是否支持滚屏显示
//#define  OPT_SUP_DIGSONG       //是否支持数字点歌功能

//#define OPT_Sup_FadeOut_fun//支持淡出功能
//#define OPT_Sup_FadeIn_fun//支持淡入功能

/* 收藏夹信息存于VRAM, 每种介质最大支持24首收藏歌曲 */
//#define SUPPORT_USERPL         //是否支持收藏夹功能


#define SCROLLTIME_DEF     260/20           //20ms * 13 = 260ms//滚屏频率分频系数
#define ABReplayMiniTime_def 400/20     //20ms * 10 = 200ms//AB复读的最小长度
//用于方案开发测试
//#define TEST_SONG_SWITCH_NEXT             /* 切歌速度测试 */


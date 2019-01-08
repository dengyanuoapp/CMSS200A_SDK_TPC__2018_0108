#ifndef RECORD_AP_H
#define RECORD_AP_H

//背景图
#define PIC_ID_B_BACKREC BACKREC
#define PIC_X_B_BACKREC	0
#define PIC_Y_B_BACKREC	0

//音乐类型
#define PIC_ID_MUSICTYPE RWAV32K
#define PIC_X_MUSICTYPE	34
#define PIC_Y_MUSICTYPE	5

//bitrate背景
#define PIC_ID_BBCK2   BBCK2
#define PIC_X_BBCK2	0
#define PIC_Y_BBCK2	0

//Bitrate的第一位数字
#define PIC_ID_F1BITRATE MBIT0
#define PIC_X_F1BITRATE	0
#define PIC_Y_F1BITRATE	4

//Bitrate的第一位数字
#define PIC_ID_F2BITRATE MBIT1
#define PIC_X_F2BITRATE	6
#define PIC_Y_F2BITRATE	4

//Bitrate的第一位数字
#define PIC_ID_F3BITRATE MBIT2
#define PIC_X_F3BITRATE	13
#define PIC_Y_F3BITRATE	4

//当前曲目第1位
#define PIC_ID_SONGNUM1  MNUM0
#define PIC_X_SONGNUM1	34
#define PIC_Y_SONGNUM1	147

//当前曲目第2位
#define PIC_ID_SONGNUM2  MNUM1
#define PIC_X_SONGNUM2	41
#define PIC_Y_SONGNUM2	147

//当前曲目第3位
#define PIC_ID_SONGNUM3  MNUM0
#define PIC_X_SONGNUM3	49
#define PIC_Y_SONGNUM3	147

//隔离线
#define PIC_ID_SONGLINE  SINGLINE
#define PIC_X_SONGLINE	59
#define PIC_Y_SONGLINE	148

//总曲目第1位
#define PIC_ID_TSONGNUM1  MNUM0
#define PIC_X_TSONGNUM1	68
#define PIC_Y_TSONGNUM1	147

//总曲目第2位
#define PIC_ID_TSONGNUM2  MNUM0
#define PIC_X_TSONGNUM2	75
#define PIC_Y_TSONGNUM2	147

//总曲目第3位
#define PIC_ID_TSONGNUM3  MNUM0
#define PIC_X_TSONGNUM3	82
#define PIC_Y_TSONGNUM3	147

//电池电量
#define PIC_ID_BATTERY    BATTERY1
#define PIC_X_BATTERY	107
#define PIC_Y_BATTERY	147

#ifndef PC
#define STR_ID_FOLDER 
#endif
#define STR_X_FOLDER	37
#define STR_Y_FOLDER	62

#ifndef PC
#define STR_ID_FILE 
#endif
#define STR_X_FILE	37
#define STR_Y_FILE	36

#ifndef PC
#define STR_ID_FILETIME 
#endif
#define STR_X_FILETIME	37
#define STR_Y_FILETIME	19

//卡标志
#define PIC_ID_CARDDISK   CARDREC
#define PIC_X_CARDDISK	1
#define PIC_Y_CARDDISK	147

//D?ê±
#define PIC_ID_TIMEHH     PRGNUM0
#define PIC_X_TIMEHH	32
#define PIC_Y_TIMEHH	104

//小时低位
#define PIC_ID_TIMEHL     PRGNUM0
#define PIC_X_TIMEHL	45
#define PIC_Y_TIMEHL	104
//分钟高位
#define PIC_ID_TIMEMH     PRGNUM0
#define PIC_X_TIMEMH	66
#define PIC_Y_TIMEMH	104

//分钟低位
#define PIC_ID_TIMEML     PRGNUM0
#define PIC_X_TIMEML	79
#define PIC_Y_TIMEML	104

//秒高位
#define PIC_ID_TIMESH     PRGNUM0
#define PIC_X_TIMESH	98
#define PIC_Y_TIMESH	104

//秒低位
#define PIC_ID_TIMESL     PRGNUM0
#define PIC_X_TIMESL	111
#define PIC_Y_TIMESL	104

//总小时高位
#define PIC_ID_TTIMEHH     TIMENUM0
#define PIC_X_TTIMEHH	66
#define PIC_Y_TTIMEHH	89

//总小时低位
#define PIC_ID_TTIMEHL     TIMENUM0
#define PIC_X_TTIMEHL	74
#define PIC_Y_TTIMEHL	89

//总分钟高位
#define PIC_ID_TTIMEMH     TIMENUM0
#define PIC_X_TTIMEMH	87
#define PIC_Y_TTIMEMH	89

//总分钟低位
#define PIC_ID_TTIMEML     TIMENUM0
#define PIC_X_TTIMEML	94
#define PIC_Y_TTIMEML	89

//总秒高位
#define PIC_ID_TTIMESH     TIMENUM0
#define PIC_X_TTIMESH	106
#define PIC_Y_TTIMESH	89

//总秒低位
#define PIC_ID_TTIMESL     TIMENUM0
#define PIC_X_TTIMESL	113
#define PIC_Y_TTIMESL	89

//背景图 /*背景图的ID ，X， Y定义
#define PIC_ID_ERROR_CARDLOCK CARDLOCK
#define PIC_X_ERROR_CARDLOCK	7
#define PIC_Y_ERROR_CARDLOCK	41


#define STR_ID_ERROR_STR MREADERR
#define STR_X_ERROR_STR	35
#define STR_Y_ERROR_STR	25

//背景图
#define PIC_ID_ERROR_MESSAGE MESSAGE
#define PIC_X_ERROR_MESSAGE	22
#define PIC_Y_ERROR_MESSAGE	38

//背景图 /*背景图的ID ，X， Y定义
#define PIC_ID_DLALL_SSBCK SSBCK
#define PIC_X_DLALL_SSBCK	0
#define PIC_Y_DLALL_SSBCK	0

//背景图 /*背景图的ID ，X， Y定义
#define PIC_ID_DLALL_YN DELNO
#define PIC_X_DLALL_YN	30
#define PIC_Y_DLALL_YN	59


#define STR_ID_DLALL_CDELALL CDELALL
#define STR_X_DLALL_CDELALL	36
#define STR_Y_DLALL_CDELALL	135

//背景图 /*背景图的ID ，X， Y定义
#define PIC_ID_DLING_MENUTWO MENUTWO
#define PIC_X_DLING_MENUTWO	0
#define PIC_Y_DLING_MENUTWO	0


#define STR_ID_DLING_CDELING CDELING
#define STR_X_DLING_CDELING	255
#define STR_Y_DLING_CDELING	80

//背景图
#define PIC_ID_UDLOCK_S UDLOCK_S
#define PIC_X_UDLOCK_S	49
#define PIC_Y_UDLOCK_S	48

#define PIC_ID_B_SSBCK SSBCK
#define PIC_X_B_SSBCK	0
#define PIC_Y_B_SSBCK	0

#define PIC_ID_F_SSLIDER SSLIDER1
#define PIC_X_F_SSLIDER	10
#define PIC_Y_F_SSLIDER	59

#define PIC_ID_F_SLIDPTR SLIDPTR
#define PIC_X_F_SLIDPTR	11
#define PIC_Y_F_SLIDPTR	60


#define STR_ID_T_SLIDE SMAIN12
#define STR_X_T_SLIDE	255
#define STR_Y_T_SLIDE	132

#define PIC_ID_F_POSL   FMNUM0
#define PIC_X_F_POSL	40
#define PIC_Y_F_POSL	80

#define PIC_ID_F_POSR   FMNUM0
#define PIC_X_F_POSR	63
#define PIC_Y_F_POSR	80

#define PIC_ID_F2_MENUNEG   MENUNEG
#define PIC_X_F2_MENUNEG	90
#define PIC_Y_F2_MENUNEG	83

#define PIC_ID_F2_POSR   FMNUM0
#define PIC_X_F2_POSR	40
#define PIC_Y_F2_POSR	80

#define PIC_ID_F1_POSM   FMNUM0
#define PIC_X_F1_POSM	50
#define PIC_Y_F1_POSM	80

#define PIC_ID_F3_POSL   FMNUM0
#define PIC_X_F3_POSL	40
#define PIC_Y_F3_POSL	80

#define PIC_ID_F3_POSM   FMNUM0
#define PIC_X_F3_POSM	63
#define PIC_Y_F3_POSM	80

#define PIC_ID_F3_POSR   FMNUM0
#define PIC_X_F3_POSR	86
#define PIC_Y_F3_POSR	80


#define PIC_ID_B_MENUTWO  MENUTWO
#define PIC_X_B_MENUTWO	0
#define PIC_Y_B_MENUTWO	0


#define PIC_ID_F_DHT  DHT
#define PIC_X_F_DHT	112             //194
#define PIC_Y_F_DHT	13             //8

#define PIC_ID_BASE    MENURARR1
#define PIC_X_BASE0	10
#define PIC_Y_BASE0	99

#define PIC_X_BASE1	10
#define PIC_Y_BASE1	78

#define PIC_X_BASE2	10
#define PIC_Y_BASE2	57

#define PIC_X_BASE3	10
#define PIC_Y_BASE3	36

#define PIC_X_BASE4	10
#define PIC_Y_BASE4	15

#define PIC_X_BASE5	10
#define PIC_Y_BASE5	14

/*菜单项目的字符串的偏移量坐标 */
#define POPMENU_ITM_X_ITEM	20         //22
#define POPMENU_ITM_Y_ITEM	0

/*滚屏区域*/
#define POPMENU_SCR_X_M_SCROLLZONE	30
#define POPMENU_SCR_Y_M_SCROLLZONE	15

#define POPMENU_SCR_W_M_SCROLLZONE	80
#define POPMENU_SCR_H_M_SCROLLZONE	16

/*激活项的显示,其坐标位置以激活位置决定显示*/
#define PIC_ID_F_MENUBCK  MENUBCK
#define PIC_X_F_MENUBCK	0
#define PIC_Y_F_MENUBCK	0
#define PIC_ID_F_MENURARR  MENURARR
#define PIC_X_F_MENURARR	0
#define PIC_Y_F_MENURARR	0
#define PIC_ID_F_DHTHK  DHTHK
#define PIC_X_F_DHTHK	102
#define PIC_Y_F_DHTHK	6
#define  POPMENU_MENU_TITLE_X 255 // 菜单标题坐标
#define  POPMENU_MENU_TITLE_Y 131//菜单标题坐标

#define PIC_ID_V_MENUTWO  MENUTWO
#define PIC_X_V_MENUTWO	0
#define PIC_Y_V_MENUTWO	0


#define PIC_ID_V_DHT  DHT
#define PIC_X_V_DHT	112
#define PIC_Y_V_DHT	13

#define PIC_ID_FINFO    MDIR
#define PIC_X_FINFO0	10
#define PIC_Y_FINFO0	99

#define PIC_X_FINFO1	10
#define PIC_Y_FINFO1	78

#define PIC_X_FINFO2	10
#define PIC_Y_FINFO2	57

#define PIC_X_FINFO3	10
#define PIC_Y_FINFO3	36

#define PIC_X_FINFO4	9
#define PIC_Y_FINFO4	15

#define PIC_X_FINFO5	9
#define PIC_Y_FINFO5	10

/*菜单项目的字符串的偏移量坐标 */
#define VARMENU_ITM_X_ITEM	22
#define VARMENU_ITM_Y_ITEM	1

/*滚屏区域*/
#define VARMENU_SCR_X_M_SCROLLZONE	30
#define VARMENU_SCR_Y_M_SCROLLZONE	10

#define VARMENU_SCR_W_M_SCROLLZONE	80
#define VARMENU_SCR_H_M_SCROLLZONE	16

/*激活项的显示,其坐标位置以激活位置决定显示*/
#define PIC_ID_V_ACTIVEBAR2  MENUBCK
#define PIC_X_V_ACTIVEBAR2	0
#define PIC_Y_V_ACTIVEBAR2	0
#define PIC_ID_V_ACTIVEBARSC  DHTHK
#define PIC_X_V_ACTIVEBARSC	102
#define PIC_Y_V_ACTIVEBARSC	6
#define PIC_ID_V_MFILE MFILE
#define PIC_X_V_MFILE	0
#define PIC_Y_V_MFILE	0
#define PIC_ID_F_FILEBCK  FILEBCK
#define PIC_X_F_FILEBCK	19
#define PIC_Y_F_FILEBCK	0
#define VARMENU_MENU_TITLE_X 255   //20 // 菜单标题坐标
#define VARMENU_MENU_TITLE_Y 131  //149 //菜单标题坐标

#define PIC_X_REPEAT	0
#define PIC_Y_REPEAT	0
#define PIC_X_EQ        0
#define PIC_Y_EQ        0
#define PIC_X_TIMEH     0
#define PIC_Y_TIMEH     0

//#define POSY_SCROLL		52

#endif //RECORD_AP_H

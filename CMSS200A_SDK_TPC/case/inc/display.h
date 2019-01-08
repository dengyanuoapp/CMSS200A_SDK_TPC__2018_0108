
#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "actapi.h"
#include "port.h"
#include "driver.h"
#include "sddrv.h"
#include "sdfs.h"
#include "irq.h"
#include "chip_cfg.h"
#include "lang_id.h"
#include "ap_cfg.h"
#include "simulatorpc.h"
#include <typeext.h>
//language define
#ifdef __C251__

#define  COLORREF uint32

#define  FONT_TYPE_LARGE          4

#define  FONT_TYPE_DEFAULT        FONT_TYPE_LARGE

//#define  DISPLAY_BUFFER_ADDR     0x5800

#define  Color_BLACK_def     0x00000000//黑 
#define  Color_WHITE_def     0x0000ffff//白 
#define  Color_RED_def       0x0000f800//红 
//#define  Color_YELLOW_def    0x0000fff2//黄
#define  Color_YELLOW_def    0x0000e748//黄 
#define  Color_GREEN_def     0x000007e0//绿 
#define  Color_BULE_def      0x0000001f//蓝 
#define  Color_BULE1_def     0x0000753E//深蓝 
#define  Color_BULE2_def     0x0000335f//浅蓝 
#define  Color_MENU_def      0x0000fcc0//橙 
#define  Color_CYAN_def      0x00008cc1//青 
#define  Color_SAVE_def      0x0000ed80//保存背景 

#define  Color_MUSIC_def     0x00007eaf //0x000014bf//音乐模式下的背景
#define  Color_MUSIC_def1    0x0000092d
#define  Color_MENU_BAR      0x0000a2a0
#define  Color_MENU_GRD      0x0000efbf
#define  Color_MENU_GRD1     0x00006c35
#define  Color_WAIT_GRD      0x00000410
#define  Color_TOP_BAR       0x0000A6BD

#define  Color_REC_def       0x00009BA9
#define  Color_REC_def1      0x0000C69C

#define  Color_FM_def1       0x0000D759


//下面根据 windows 画笔列出如下颜色表
// Color_tab0106_def 中的 01表示表中第1行，06表示第6列
// 可参照 windows 画笔的色表
#define  Color_tab0101_def   Color_BLACK_def
#define  Color_tab0102_def   0x00008410
#define  Color_tab0103_def   0x00008000
#define  Color_tab0104_def   0x00008400
#define  Color_tab0105_def   0x00000400
#define  Color_tab0106_def   0x00000410
#define  Color_tab0107_def   0x00000010
#define  Color_tab0108_def   0x00008010
#define  Color_tab0109_def   0x00008408
#define  Color_tab0110_def   0x00000208
#define  Color_tab0111_def   0x0000041f
#define  Color_tab0112_def   0x00000210
#define  Color_tab0113_def   0x0000401f
#define  Color_tab0114_def   0x00008200
#define  Color_tab0201_def   Color_WHITE_def
#define  Color_tab0202_def   0x0000c618
#define  Color_tab0203_def   Color_RED_def
#define  Color_tab0204_def   0x0000ffe0
#define  Color_tab0205_def   Color_GREEN_def
#define  Color_tab0206_def   0x000007ff
#define  Color_tab0207_def   Color_BULE_def
#define  Color_tab0208_def   0x0000f81f
#define  Color_tab0209_def   0x0000fff0
#define  Color_tab0210_def   0x000007f0
#define  Color_tab0211_def   0x000087ff
#define  Color_tab0212_def   0x0000841f
#define  Color_tab0213_def   0x0000f810
#define  Color_tab0214_def   0x0000fc08

/*菜单滚动背景颜色 */
#define Color_BULE_Scroll_def  0x0000001f//深蓝 

#define SCROLL_REGION_X        30 //45    //30
#define SCROLL_REGION_Y       110
#define SCROLL_REGION_WIDTH    87 //140   //87
#define SCROLL_REGION_HEIGHT   16

//#define FirstSectionCode    0xa1
#define DisplayBufLen       1024//显示缓冲区的大小 

#define ASCIIDISCODE        0x80
#define FirstSectionCode    0xa1
#define DisplaySuppPage     0x08
//lcd use
#define LCMRST_GIO_EN_REG        GPIOGOUTEN    //GPIO_G3 (Output)
#define LCMRST_GIO_DATA_REG      GPIOGDAT
#define LCMRST_GIO_EN_BIT        0x08
#define LCMRST_SET_BIT           0x08
#define LCMRST_CLR_BIT           0xF7

#define LCMBL_GIO_EN_REG        GPIOGOUTEN        //GPIO_G6 (Output)
#define LCMBL_GIO_DATA_REG      GPIOGDAT
#define LCMBL_GIO_EN_BIT        0x40
#define LCMBL_SET_BIT           0x40
#define LCMBL_CLR_BIT           0xBF

#define LCMRS_GIO_EN_REG        GPIOCOUTEN        //GPIO_C1 (Output) 
#define LCMRS_GIO_DATA_REG      GPIOCDAT
#define LCMRS_GIO_EN_BIT        0x02
#define LCMRS_GIO_DN_BIT        0xFD
#define LCMRS_SET_BIT           0x02
#define LCMRS_CLR_BIT           0xFD

#define CE_GIO_EN_REG           GPIOAOUTEN        //GPIO_A7 CS pin 
#define CE_GIO_DATA_REG         GPIOADAT
#define CE_EN_BIT               0x80      //ce enable
#define CE_DN_BIT               0x7F
#define CE_SET_BIT              0x80
#define CE_CLR_BIT              0x7F


#define Display_Length      128
#define Display_Height      64


#define Lcd_acturalwidth 132
#define Lcd_acturalheight 162

//for not actural size lcd
//Lcd_widthoffset = (Lcd_acturalwidth - Display_Length)/2
//Lcd_heightoffset (Lcd_acturalheight - Display_Height)/2

#define Lcd_widthoffset 1
#define Lcd_heightoffset 2

//定义三个刷屏相关buf的RAM标号
#define LCD_RAM_INDEX      0
#define JPEG_RAM_INDEX     1
#define MURAM1L_RAM_INDEX  2
#define MURAM1L_RAM_USB    3

#define JPEG_BUF_ADDR 	 0x3000
#define LCD_BUF_ADDR  	 0x2400
#define MURAM1L_BUF_ADDR 0x5000

#define JPEG_BUF_LEN  	0x2000	//8K
#define LCD_BUF_LEN   	0x400	//1K

#define DMA_RGB    0
#define DMA_YCBCR  1

//lcd cmd
#define LCD_CMD_SETX     0x2a
#define LCD_CMD_SETY     0x2b
#define LCD_CMD_WRITE    0x2c
#define LCD_CMD_READ     0x2e
#define LCD_CMD_SETMODE  0x36

//display mode

#define RGBFLAG     0x08
#define MXENDFLAG   0x01
#define MYENDFLAG   0x02
#define MVENDFLAG   0x03
#define MYREVERSE   0x04

//for lcd & led
#define S1 0
#define S2 1
#define S3 2
#define S4 3
#define S5 4
#define S6 5
#define S7 6
#define S8 7
#define S9 8
#define S10 9
#define S11 10
#define S12 11
#define S13 12
#define S14 13
#define S15 14
#define S16 15
#define S17 16
#define S18 17
#define S19 18
#define S20 19
#define S21 20
#define S22 21
#define S23 22
#define S24 23
#define S25 24

#if 1
#define LED_SD    S1   //"SD"---k4
#define LED_USB   S2  //"USB"---k3
#define LED_COL   S3  //":"-----k5 
#define LED_PLAY  S4  //""k1
#define LED_PAUSE S5 //k2
#define LED_AUX   S7   //k6
#define LED_FM    S6   //". fm"k7
#else


#define LED_IPOD    S1   //"IPOD"---k1
#define LED_DP1        S2   //"DP1"---k2
#define LED_DP2        S3   //"DP2"---k2
#define LED_REPLAY         S4   //"SD"---k5
#define LED_SD         S5   //"SD"---k2
#define LED_USB        S6   //"USB"---k3
#define LED_REC        S7   //"REC"---k4
#define LED_DP3        S8   //"DP3"---DP3
#define LED_BAT3        S9   //"bat3"---k6
#define LED_BAT2        S10   //"bat2"---k7
#define LED_BAT1        S11   //"bat1"---k8
#endif
#define NUM_A  10
#define NUM_B  11
#define NUM_C  12
#define NUM_D  13
#define NUM_E  14
#define NUM_F  15
#define NUM_G  16//无用
#define NUM_H  17
#define NUM_I  18
#define NUM_J  19
#define NUM_K  20//无用
#define NUM_L  21
#define NUM_M  22//无用
#define NUM_N  23
#define NUM_O  24
#define NUM_P  25
#define NUM_Q  26
#define NUM_R  27
#define NUM_S  28
#define NUM_T  29//无用
#define NUM_U  30
#define NUM_V  31//无用
#define NUM_W  32//无用
#define NUM_X  33//无用
#define NUM_Y  34

#define NUMBER1 S1
#define NUMBER2 S2
#define NUMBER3 S3
#define NUMBER4 S4

typedef struct
{
    COLORREF PenColor;      //画笔和字体颜色
    COLORREF BackgdColor;   //背景颜色 !!
} DC, *PDC;

typedef struct
{
    uchar x;         //列位置
    uchar y;         //行位置
    uchar width;     //宽度
    uchar height;    //高度
} region_t;



typedef struct

{
    uint8 width;
    uint8 height;
} lcm_size_t;
//UI_lancfg用于多国语言处理，其定义如下:
typedef struct
{
    uint8 lang_id;//语种
    uint8 fontaddrindex;//根据语种查找字库位置写入，供UI驱动调用
    uint8 MBTaddrindex;//根据语种查找字库码表位置写入，供UI驱动调用
} UI_lancfg;





/**************************出错消息********************************/
#define  RESULT_PUTCHAR_OK            0//正常输出 
#define  RESULT_PUTCHAR_X_ERR         1//X轴超出 
#define  RESULT_PUTCHAR_Y_ERR         2//Y轴超出 
#define  RESULT_PUTCHAR_OTHER_ERR     3//其他错 

/**************************数据传送模式定义********************************/
#define  NORTransMode_def  0    //用普通的传送模式 
#define  DMATransMode_def  1   //用DMA传送模式 

/**************************standby mode 定义********************************/
#define  StandbyOFF_def   0     //关显示屏 
#define  StandbyON_def    1    //开显示屏 
#define  UNICODELANGUAGE  0x7f    //字符资源为unicode编码模式  
#define  NOUNICODELANGUAGE 0x7E      //内码字符资源为unicode编码模式
#define UNICODEARABICDONTJOIN 3
#define ISNARABICDONTJOIN  4

//for arabic ligature
#define  ARABICHASJOIN    -2
#define  ARABICHASNOJOIN     -3      // arabic 没有连写处理的标志
#define  COMBOX_RES           0      //
#define  MB_RES               1      //

/**************************用图方向模式定义********************************/
#define  DrawModeH_def      0  //横向优先 
#define  DrawModeV_def      1  //纵向优先   
#define  DrawModeHPic_def   2  //看图横向模式 
#define  DrawModeVPic_def   3  //看图纵向模式 
#define  DrawModeAvi_def    4  //AVI模式 
#define  DrawModeJpg_def    5
#define  DrawModeBMP_def    6

/**************************滚屏模式的定义********************************/
//#define  SCMFull_def    0    //全屏滚模式
//#define  SCMUp_def      1    //上屏滚模式
//#define  SCMDown_def    2     //下屏滚模式
//#define  SCMMid_def     3    //中间滚模式

/**************************字符的点阵模式********************************/
//#ifdef   UNICODE_SUPPORT
#define  UnicodeMode      0    //16*16点阵模式 
#define  PicDisMode       1    //图形显示模式 


/**************************定义3d效果********************************/
#define  D3Normalframe_def  0 //只加边框 
#define  D3KeyUp_def        1 //突出效果 
#define  D3KeyDown_def      2 //凹进效果 

/**************************定义3d图标效果颜色********************************/
#define  FaceLightColor_def  Color_tab0202_def   //向光边的颜色 
#define  BackLightColor_def  Color_tab0102_def   //背光边的颜色 
#define  SelectedColor_def   Color_BLACK_def     //被选中图片的边框颜色 


/**************************定义UNICODE字符 SIZE*****************************/
#define UNICODE_HEIGHT_DEF     16
#define UNICODE_WIDTH_DEF      16
#define PUT_UNCHAR       1
#define CHAR_OUTRECT     2
#define GET_STRINGLENGTH 3



//api no for case


/***************************led驱动API接口******************************/
#define APINO_LEDInit     0x0
#define APINO_LEDExit    0x01
#define APINO_LEDDisplay 0x02
#define APINO_LEDClearScreen 0x03
#define APINO_LEDPuts      0x04

void API_LEDInit(uint8 api_no);
#define LEDInit()  API_LEDInit(APINO_LEDInit);

void API_LEDExit(uint8 api_no);
#define LEDExit()  API_LEDExit(APINO_LEDExit);

void API_LEDDisplay(uint8 api_no, uint8 addr, uint8 content, uint8 type);
#define LEDDisplay(addr,content,type)  API_LEDDisplay(APINO_LEDDisplay,addr,content,type);

void API_LEDClearScreen(uint8 api_no);
#define LEDClearScreen()  API_LEDClearScreen(APINO_LEDClearScreen);

void API_LEDPuts(uint8 api_no, uint8 addr, uint8 *strbuf, uint8 type);
#define LEDPuts(addr,strbuf,type)  API_LEDPuts(APINO_LEDPuts,addr,strbuf,type);

#endif  /*end of _C251_ */
#endif/*_DISPLAY_H*/
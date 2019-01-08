/*
 ********************************************************************************
 *                       common
 *               ap common head file
 *
 *                (c) Copyright 2002-2003, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : ap_common.h
 * By     : phchen
 * Version: 1> v1.00     first version     2004-08-20 11:35
 ********************************************************************************
 * $Id: common.h,v 1.1.1.1 2008/01/11 07:28:06 rowena Exp $
 ********************************************************************************
 */
#ifndef _COMMON_H
#define _COMMON_H

#include  "display.h"
#include  "enhanced.h"
#include  "ui_res.h"
#include "actos.h"

//定义错误信息框显示位置
#define PIC_MESSAGEBOX_X   9
#define PIC_MESSAGEBOX_Y  38

#define PIC_CARDERR_X      9
#define PIC_CARDERR_Y     32
// 定义每一种类型的最大文件扩展名的个数
#define  MUSIC_EXT_COUNT        4
#define  VOICE_EXT_COUNT        2
#define  DSC_EXT_COUNT            2
#define  NEW_MAX_FILE_NUM        999

// LCD屏有关的定义
// LCD分辨率的定义, 所有控件的布局由以下两个参数确定
// 定义屏幕的高度和宽度.
#define  LCD_WIDTH            Display_Length
#define  LCD_HIGH             Display_Height

// 字库,显示高度的定义
#define  CN_CHAR_WIDTH            16
//#define  LARGE_CHAR_HEIGHT        16
//#define  LARGE_CHAR_WIDTH        8
//#define  CHAR_HEIGHT            8
//#define  CHAR_WIDTH                 6
//#define        TCHAR_WIDTH        8

// 定义中英文行的高度
#define  ROW_EN_HEIGHT            10
#define  ROW_CN_HEIGHT            18

// 字体全透明
#define  CLARITY            0xff
#define  NOCLARITY            0x0

// 菜单头控件有关的定义
typedef struct
{
    uint16 title;
    uint16 headpic;
    uint8 menu_grade[3]; // "1-1-1"
} menuhead_t;

//下面参数用 define
#define  MENU_HEAD_HIGH        19          //菜单头控件的高度
#define  MENU_HEAD_WIDTH       LCD_WIDTH  //菜单头控件的宽度
#define  MENU_HEAD_S_POS_X     255        //字符串起始坐标
#define  MENU_HEAD_S_POS_Y     130
#define  MENU_HEAD_G_POS_Y     ((MENU_HEAD_HIGH-CHAR_HEIGHT-1)/2) // 菜单级别坐标
//错误报警框控件
#define  MSG_WIDTH            100    // 该值必须小于LCD_WIDTH, 大于80
#define  MSG_HIGH             60    //该值必须小于LCD_HIGH, 大于30
#define  MSG_POS_X            ((LCD_WIDTH - MSG_WIDTH)/2)
#define  MSG_POS_Y            ((LCD_HIGH - MSG_HIGH)/2)
#define  MSG_RIGHT            (MSG_POS_X + MSG_WIDTH)
#define  MSG_BOTTOM           (MSG_POS_Y + MSG_HIGH)
#define  MSG_STR_Y            ((LCD_HIGH - 12)/2)
//#define  menuhead_high_def        19        // 菜单头控件的高度
//#define  menuhead_width_def        LCD_WIDTH      // 菜单头控件的宽度
#define  MENUHEAD_S_POS_X_DEF        255        // 字符串起始坐标
#define  MENUHEAD_S_POS_Y_DEF        110
//#define  menuhead_g_pox_y_def        ((menuhead_high_def - CHAR_HEIGHT - 1) / 2)    // 菜单级别坐标
// 错误报警框控件

//#define  msg_width_def            100    // 该值必须小于LCD_WIDTH, 大于80
//#define  msg_high_def            60    //该值必须小于LCD_HIGH, 大于30
//#define  msg_pox_x_def            ((LCD_WIDTH - msg_width_def) / 2)
//#define  msg_pox_y_def            ((LCD_HIGH - msg_high_def) / 2)
//#define  msg_right_def            (msg_pox_x_def + msg_width_def)
//#define  msg_bottom_def            (msg_pox_y_def + msg_high_def)
//#define  msg_str_y_def            (LCD_HIGH-12) / 2

//普通菜单控件(一级)
/*
typedef struct
{
    uint16 total; //总共有几个item
    uint16 active; //活动状态的编号
    const uint16 *title; //字符串id 数组
    uint8 grade[3];
} function_menu_t;
*/
//下面参数用 define
#define  MENU_PTR_X_DEF  0
#define  MENU_STR_X_DEF       20
#define  MENU_BCK_X_DEF       0    //menu_str_x_def-4
#define  MENU_BCK_WIDTH         76
//#define  menu_ASCIIlength    12
#define  MENU_HEAD_X_DEF       0
#define  MENU_HEAD_Y_DEF       128
//下面参数用 define
#define  MENU_PTR_X         10
#define  MENU_STR_X         32
#define  MENU_BCK_X         9    //menu_str_x_def-4
//#define  MENU_BCK_WIDTH     76
#define  MENU_ASCII_LENGTH  12

//进度条控件

typedef struct
{
    //menuhead_t *menuhead;
    uint16 min;
    uint16 max;
    uint16 value;
} progress_t;

//下面参数用 define
#define  PROGRESS_HEIGHT     10      //滑动杆的高度象素个数
#define  PROGRESS_WIDTH      (LCD_WIDTH / 4 * 3)
#define  PROGRESS_POS_Y      6

//滑动杆控件
typedef struct
{
    int16 min;
    int16 max;
    int16 value;
    uint16 step; //步长,每次+/-的值
    uint8 check_disk; //是否需要处理卡或u盘消息标记
} slider_t;

//横滑动杆下面参数用 define
//#define  slider_high_def              19          //定义滑动杆的高度象素个数
#define  SLIDER_WIDTH_DEF           120-28
#define  SLIDER_POX_X_DEF           14
#define  SLIDER_POX_Y_DEF           44
#define  SLIDER_CURSOR_Y_DEF      44

// 竖滑动杆
//#define  slider_pic_width_def        76
//#define  slider_pic_height_def       60

//下面参数用 define
//#define  list_str_x_def       4
//#define  list_str_y_def        (LCD_HIGH-18)

// 文件浏缆器
//#define  explorer_head_str_x_def        2
//#define  explorer_head_str_y_def    112
#define  EXPLORER_PIC_X_DEF             0
#define  EXPLORER_STR_X_DEF             16
//#define  explorer_row_high_def        15
#define  EXPLORER_ROW_COUNT_DEF         5
//#define  explorer_str_len_def        ((LCD_WIDTH - explorer_str_x_def) / (LARGE_CHAR_WIDTH * 2) * 2)

// 文件浏缆删除器的类型
#define     EXPLORE_ALL            1    // 含有指定文件类型的一级目录和文件.
#define     EXPLORE_DIR            2    // 含有指定文件类型的一级目录
#define     EXPLORE_FILE        3    // 根目录下指定文件类型的文件
#define     SEL_DIR_ONLY        4    // 所有一级目录
#define            DEL_JPG        8        // 删除图象文件
#define            DEL_MUSIC        9        // 删除音乐文件
#define            DEL_VOICE        10        // 删除录音文件
#define            DEL_ALL            11    // 删除所有文件.
#define        DEL_TXT        12
#define        DEL_AMV        13    //delete amv file
// Picture def change

// pop up list
typedef struct
{
    //region_t region;
    const uint16 *strid;
    uint16 total; //菜单选项个数
    uint16 active; //活动项
    uint16 headid; //菜单标题id
} pop_up_list_t;

uint16 ui_popup_list(pop_up_list_t *list,  void(*callback)(uint16 value));
//成功返回0
// 错误或忽略返回 RESULT_IGNORE
// 其它值是热键

uint16 ui_err_msg(uint16 msgID);//显示后待5秒或按任意键退出
uint16 ui_show_msg(uint16 msgID);//显示信息后马上退出
//成功返回0
// 其它值是热键

//uint16 ui_menu(function_menu_t *menu , const void(*callback)(uint16 value));
//成功返回0
// 错误或忽略返回 RESULT_IGNORE
// 其它值是热键
//uint16 ui_menu2(function_menu_t *menu , const void(*callback)(uint16 value));


extern uint16 ui_show_progress(const progress_t *progress);
//extern void ui_show_guage(uint16 step, uint8 para);

//int ui_filelist(file_location_t *location, int type, int string_id);
//int ui_explore(file_location_t *location, int type, int string_id);
// 文件路径直接放在 location里面
// 如有热键，则返回热键的值
// 成功返回0
// 错误或忽略返回 RESULT_IGNORE


// 下面是全局变量的定义
//extern uint8 file_name[12];
//extern uint8 dir_name[30];
//滚屏参数
//extern uint8 *str_pt;        //字符串当前指针
//extern uint8 *strhead_pt;   //字串首址记录
//extern uint8 scroll_cnt;


// 下面是各个控件的子函数的定义
uint16 rm_dir_from_buf(uint16 n);
extern uint8 read_idm_data(uint8 *name, uint16 count);
extern uint8 write_idm_data(uint8 *name, uint16 count);
//extern void init_dir_buf();
//extern uint8 get_dir_name(uint8 *file, uint16 num);
//extern void get_dir_name2(uint8 *file, uint16 num);
//extern uint16 init_sel_dir_buf(void);
//extern uint8 del_all_my_file(uint16 type, uint8 string_id);
extern void reinit_idm_buf(uint16 top, uint16 bottom, uint16 total_dir, uint16 string_id);
void show_active_list(uint16 old_active, uint16 active, uint16 top);
//uint8 show_popup_list(uint16 string_id);
//int ui_explore_sub(file_location_t *location, int type, int string_id);
//uint16 init_active(uint16 string_id);
//int init_active_dironly(file_location_t  *location);
//void reset_cur_list_bg_color(int i, int tmp);
//void get_dir_long_name(file_location_t *location, int string_id);
void redraw_window(void);
void Clearexplore(void);
void ui_show_data(uint8 *buf, uint8 x, uint8 y);
void MYDUMP(char *string, int value);

#endif

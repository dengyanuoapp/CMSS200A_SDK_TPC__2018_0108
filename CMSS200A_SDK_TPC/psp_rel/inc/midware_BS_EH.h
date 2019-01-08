/*
*******************************************************************************
*                ACTOS AP
*        AG decoder header file
*
*        (c) Copyright, Actions Co,Ld.
*             All Right Reserved
*
*******************************************************************************
*/

#ifndef         _MIDWARE_BS_EH_H
#define         _MIDWARE_BS_EH_H
#include        <actos.h>

#define ENABLE_BANKING() MemBankCtl |= BANK_ENABLE
#define DISABLE_BANKING() MemBankCtl &= BANK_DISABLE
//#define CLEAR_WATCHDOG_MIDWARE() do{}while(0)
//#define ClearWatchDog_MW() do{}while(0)

//#define TEST_BASAL_BANK
//#define MW_PRINT
//#define TEST_CARD    1
#define FLUSH_BANK_AP_BANK1 0x01
#define FLUSH_BANK_AP_BANK2 0x02
#define FLUSH_BANK_BASAL 0x04
#define FLUSH_BANK_CODEC 0x08

#define BYTES_OF_ONE_PAGE 512
#define OBJECT_ID_LENGHT 16
#define OBJECT_SIZE_LENGHT 8
#define PLAY_DURATION_LENGHT 8
#define PREROLL_LENGHT 8
#define NUMBER_OF_HEADER_LENGHT 4
#define FIRST_HEADER_OBJECT_POS 30
#define DATA_OBJECT_HEADER_LENGHT 0x32

///////////typeCheck
#define CHECK_PASS 0
#define CHECK_NOPASS 1
#define BSPAGESize 512
#define ID3tag (0x494433)
#define MP3HeadORa (0xFE0C0F)
#define BSPAGEMaxErrorNum (300)
#define Sync_Head_MaxCount 48//32
#define Sync_Head_MaxSameCount 4
#define BUFFER_ADDRESS (0x0d80)//(0x1800)
#define ID3V2_HEADER_LEN 10
#define MP3_FRAME_HEAD 3   //4-1


#define AB_FLAG_DEAL_AB_PROCESS 0xff
#define AB_FLAG_SET_A 0x22
#define AB_FLAG_SET_B 0xAA

#define BYTE2_OF(x) ((unsigned char)((((unsigned long)((void far*)x))&0xff0000) >>16))


//lrc

typedef enum
{
    CharType_NUM,    //数字（0-9）
    CharType_UPER,    //大写字母（A-Z）
    CharType_DNER,    //小写字母（a-z）
    CharType_SPACE,    //空格 (" ")
    CharType_OTHER    //其他值
} CharType_e;



#define PAGE_BYTE 512
#define NEXT_ROW 0x0a
#define NEXT_SCREEN 0x0d
#define MINUTE_VALID 2
#define SECOND_VALID 4
#define MAX_MINUTE 99
#define ERROR_EXPLAIN TRUE
#define RIGHT_EXPLAIN FALSE
#define LAST_TIME_BETWEEN 3

//#define CLEAR_WATCHDOG_MIDWARE() do{}while(0)

typedef struct timeFormat
{
    uchar min;
    uchar sec;
    uchar p1s;
} timeFormat_t;

typedef enum languageLrc
{
    English_LRC = 1,
    Chinese_LRC
} languageLrc_t;



typedef enum returnDealString
{
    READ_BUFFER_AGAIN = 2,
    READ_FILE_AGAIN,
    STRING_BUFFER_OVERFLOW,
} returnDealString_t;

typedef enum returnReadFile
{
    READ_FILE_NORMAL = 0,
    READ_FILE_END,
    READ_FILE_ERROR,
} returnReadFile_t;

typedef enum returnReadData
{
    READ_DATA_ZERO = 0,
    READ_DATA_DONE,
    READ_DATA_FILE_END,
    READ_DATA_FILE_ERROR,
    READ_DATA_NOT_USE_DATA,
} returnReadData_t;

typedef enum returnReadString
{
    READ_STRING_ZERO = 0,
    READ_STRING_DONE,
    READ_STRING_FILE_END,
    READ_STRING_FILE_ERROR,
    READ_STRING_REREAD_FILE,
    READ_STRING_LRC_OVERFLOW,
    READ_STRING_TIME_START,
} returnReadString_t;

#endif  /* _MIDWARE_H */

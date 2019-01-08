/********************************************************************************
 *                              USDK 5102
 *                            Module: POWER MANAGER
 *                 Copyright(c) 2001-2009 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *    wuyueqian     2009-9-07 9:42     1.0             build this file
 ********************************************************************************/
/*!
 * file
 * brief
 * Author   wuyueqian
 * par
 *
 *      Copyright(c) 2001-2007 Actions Semiconductor, All Rights Reserved.
 *
 * version 1.0
 * date  2008/6/12
 *******************************************************************************/

#include <basal.h>
#include <decC.h>
#include <tts_lib.h>

#pragma name(MW_BS_AD_CMDR_TTS)

SD_FILE *m_de_TTS_Fp;
SD_FILE *info_TTS_Fp;
//Open_param_t *m_deTTSOpenparam;
play_status_t  TTSplay_status;
BYTE *m_deTTSDataBuffer;//[512];//解码时用的BUFFER

uint32  m_deTTSOldDspIntAddrSav;
uint32  m_deTTSOldDmaIntAddrSav;
uint8  m_deTTScTimer_Sav;
uint8  m_deTTSInitFlag;
unsigned char  prev_next_flag;
WORD  deTTSSecCount;
WORD  deTTSSecOffset;

unsigned char *str_buff;
unsigned unsigned char str_ptr;
unsigned char  tts_lib_flag;
tts_header_t  tts_info_head;

const char tts_lib[] = "Package.dat";
const char tts_info[] = "INFO.POS";

#define READ_SECTOR_BYTE_NUMBER 512
#define MOVE_BACKWARD_ONE_PAGE_FLAG  0x22
#define MOVE_FORWARD_ONE_PAGE_FLAG 0x11
#define RANDOM_CHOOSE_ONE_PAGE_FLAG 0x33

bool tts_info_set(void);
void tts_data_deal(void);

#ifndef PC
#pragma ROM(large)
//void GetSDErrorTYPE(void);
#pragma ROM(huge)
#else
//void GetSDErrorTYPE(void);
//decBreakPointInfor_t g_decBreakPointInfor;
#endif

#pragma renamecode(MW_BS_AD_CMDR_TTS)

#pragma ROM(huge)
uint8 m_deTTSRead_Data(uchar readFileDirection, uint32 position)
{
    bool result = TRUE;
    uint32 hook_read_bytes;

    position = position;
    readFileDirection = readFileDirection;

    //断点记录的是解码过的信息
    //    if ((TTSplay_status.status == PLAYING_REACH_END) || (TTSplay_status.status == PLAYING_ERROR) ||
    //            (TTSplay_status.status == PLAYING_REACH_HEAD))
    //    {
    //        return 0xff;
    //    }
    if(TTSplay_status.status == PLAYING_ERROR)
    {
        return FALSE;
    }

    if (deTTSSecCount == 0)
    {
        if(tts_info_set() != TRUE)
        {
            return FALSE;
        }
    }

    if((deTTSSecCount == 1) && (deTTSSecOffset != 0))
    {
        hook_read_bytes = deTTSSecOffset;
    }
    else
    {
        hook_read_bytes = 512;
    }

    SD_FRead(m_de_TTS_Fp, m_deTTSDataBuffer, hook_read_bytes);
    tts_data_deal();
    if(hook_read_bytes != 512)
    {
        memset(m_deTTSDataBuffer + hook_read_bytes, 0x00,
               512 - hook_read_bytes);
        hook_read_bytes = 512;
    }
    deTTSSecCount--;

    return result;
}

void swap_int_byte(int *a)
{
    char tmp;
    char *p = (char *)a;
    tmp = *p;
    *p = *(p + 1);
    *(p + 1) = tmp;
}

void swap_long_byte(long *a)
{
    char tmp;
    char *p = (char *)a;

    tmp = *p;
    *p = *(p + 3);
    *(p + 3) = tmp;

    tmp = *(p + 1);
    *(p + 1) = *(p + 2);
    *(p + 2) = tmp;
}

void fix_head_data(tts_header_t *tts_info_head)
{
    swap_int_byte(&tts_info_head->wordcnt);
    swap_int_byte(&tts_info_head->bytes1);
    swap_int_byte(&tts_info_head->multi_cnt);
    swap_int_byte(&tts_info_head->bytes2);
    swap_int_byte(&tts_info_head->citiaocnt);
    swap_int_byte(&tts_info_head->bytes3);
}

void fix_pos_data(tts_pos_t *pos)
{
    swap_long_byte(&pos->start_offset);
    swap_int_byte(&pos->str_length);
    swap_int_byte(&pos->multi_offset);
}

bool tts_info_set(void)
{
    tts_pos_t tts_pos_tmp;
    tts_multitone_t tts_multitone_item;
    char pos_tag, find_multitone_tag, qu, wei;
    uint16 offset;
    long deTTSApointerSave;
    unsigned char *pbuf = &str_buff[str_ptr];
    uint16 citiao_offset;

    pos_tag = TTS_MULTI_PREV;
    if(*pbuf == 0x00)
    {
        TTSplay_status.status = PLAYING_ERROR;
        return FALSE;
    }
    citiao_offset = 0x20 + (tts_info_head.wordcnt + tts_info_head.multi_cnt) * 8;

    find_multitone_tag = 0;

    if ((*pbuf >= (unsigned char)0x80) && (tts_lib_flag != 0))
    {
        //每个汉字机内码的第一个字节范围介于0xB0~ 0xF7之间就OK了，转换成区位码就是0~71（十进制）
        if((*pbuf > (unsigned char)0xAF) && (*pbuf < (unsigned char)0xF8))
        {
            qu = (str_buff[str_ptr] & (unsigned char)0x7F) - 0x30;
            wei = (str_buff[str_ptr + 1] & (unsigned char)0x7F) - 32 - 1;
            //TTS索引表头数据结构,占据32byte,reserved 24byte
            offset = 0x20 + (qu * 94 + wei) * sizeof(tts_pos_t);
            str_ptr += 2;
            find_multitone_tag = 1;
            if(prev_next_flag)
            {
                pos_tag = TTS_MULTI_NEXT;
            }
            else
            {
                pos_tag = TTS_MULTI_PREV;
                prev_next_flag = 1;
            }
        }
        else
        {
            str_ptr += 2;
            prev_next_flag = 0;
            //relocate to space mute data
            offset = citiao_offset + (sizeof(tts_pos_t) - 2) * 0x20;   //ascii char offset
        }
    }
    else
    {
        prev_next_flag = 0;
        if(*pbuf > (tts_info_head.citiaocnt - 1))
        {
            *pbuf = 0x20;
        }
        offset = citiao_offset + (sizeof(tts_pos_t) - 2) * (*pbuf);   //ascii char offset
        str_ptr++;
    }
re_get_struct:
    SD_FSeek(info_TTS_Fp, SEEK_SET, offset);
    SD_FRead(info_TTS_Fp, &tts_pos_tmp, sizeof(tts_pos_t));

    fix_pos_data(&tts_pos_tmp);

    if((tts_pos_tmp.start_offset == 0) && (tts_pos_tmp.str_length == 0))
    {
        offset = citiao_offset + (sizeof(tts_pos_t) - 2) * 0x20;   //ascii char offset
        goto re_get_struct;
    }

    //多音字处理
    if((find_multitone_tag == 1) && (tts_pos_tmp.multi_offset != 0xffff))
    {
        deTTSApointerSave = citiao_offset + (sizeof(tts_pos_t) - 2) * \
                            tts_info_head.citiaocnt + tts_pos_tmp.multi_offset;
        SD_FSeek(info_TTS_Fp, SEEK_SET, deTTSApointerSave);

        while(find_multitone_tag)
        {
            SD_FRead(info_TTS_Fp, &tts_multitone_item, sizeof(tts_multitone_item));

            switch(tts_multitone_item.multitone_tag)
            {
            case    TTS_MULTI_CON:
                qu = tts_multitone_item.qu;
                wei = tts_multitone_item.wei;
                break;

            case    TTS_MULTI_PREV:
                if(pos_tag != TTS_MULTI_PREV)
                {
                    if(tts_multitone_item.qu == str_buff[str_ptr - 4] &&
                            tts_multitone_item.wei == str_buff[str_ptr - 3])
                    {
                        find_multitone_tag = 0;
                    }
                }
                break;

            case    TTS_MULTI_NEXT:
                if(tts_multitone_item.qu == str_buff[str_ptr] &&
                        tts_multitone_item.wei == str_buff[str_ptr + 1])
                {
                    find_multitone_tag = 0;
                }

                break;

            case    TTS_MULTI_END:
            default:
                find_multitone_tag = 0;
                break;
            }
        }

        offset = 0x20 + (qu * 94 + wei) * sizeof(tts_pos_t);

        SD_FSeek(info_TTS_Fp, SEEK_SET, offset);
        SD_FRead(info_TTS_Fp, &tts_pos_tmp, sizeof(tts_pos_t));
        fix_pos_data(&tts_pos_tmp);
    }

    deTTSApointerSave = tts_pos_tmp.start_offset;
    deTTSSecOffset = tts_pos_tmp.str_length % 512;
    deTTSSecCount = tts_pos_tmp.str_length / 512;
    if(deTTSSecOffset != 0)
    {
        deTTSSecCount++;
    }
    SD_FSeek(m_de_TTS_Fp, 0, deTTSApointerSave);

    return TRUE;
}


void tts_data_deal(void)
{
    uint16 i;
    for(i = 0; i < 512; i++)
    {
        m_deTTSDataBuffer[i] ^= (unsigned char)0x35;
    }
}

#ifndef PC
#pragma ROM(huge)
#endif

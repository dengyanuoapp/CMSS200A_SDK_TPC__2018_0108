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

#include <actos.h>
#include <enhanced.h>
#include <stringH.h>
#include <lang_id.h>

#pragma name(MW_EH_ID3)

#pragma renamecode(MW_EH_ID3)

#define ID3_BUFFER 0x0d80//0x1800
extern char *TempBuf; //read file buffer

extern uint8 karaoke_flag;
extern uint8 actrec_flag;

char far TagBuf[128];
bool FectchID3Info(const char *filename, ID3Info_t *ID3Info);
extern BOOL FetchID3_V2info(handle TagFileHandle, ID3Info_t *ID3Info);
void DealStringEnd(uint8 *str, int string_id);
void DealStrEndSpace(char *str);

/*
 ********************************************************************************
 *             mp3GetTiltle
 * Description :
 * Arguments   :  string_id is language id(0:简体 1 英文 2 繁体),用于对最后的不完整字符进行处理。
 因为最多取30个字符。而有些时候汉字里面有可能混放英文的。因此会出现最后字符只是汉字一半的情况
 * Returns     : ID3ExistFlag = 0 表示没有ID3信息，1表示有
 * Notes       :
 ********************************************************************************
 */
#ifndef PC
bool mp3GetID3Info(const char *filename, ID3Info_t *ID3Info, int string_id)
#else
bool mp3GetID3Info(const char *filename, ID3Info_t *ID3Info, int string_id)
#endif
{
    bool ID3ExistFlag = FALSE;

    ClearWatchDog();

    karaoke_flag = 0;
    actrec_flag = 0;

    if (!FectchID3Info(filename, ID3Info))
    {
        ClearWatchDog();
        return FALSE;
    }

    //下面处理得出的 信息的一些特殊情况
    if (ID3Info->TIT2_length != 0)
    {
        DealStringEnd(ID3Info->TIT2_buffer, string_id);
        DealStrEndSpace(ID3Info->TIT2_buffer);
        if (ID3Info->TIT2_buffer[0] != 0)
        {
            ID3ExistFlag = TRUE;
        }
    }

    if (ID3Info->TPE1_length != 0)
    {
        DealStringEnd(ID3Info->TPE1_buffer, string_id);
        DealStrEndSpace(ID3Info->TPE1_buffer);
        if (ID3Info->TPE1_buffer[0] != 0)
        {
            ID3ExistFlag = TRUE;
        }
    }

    if (ID3Info->TALB_length != 0)
    {
        DealStringEnd(ID3Info->TALB_buffer, string_id);
        DealStrEndSpace(ID3Info->TALB_buffer);
        if (ID3Info->TALB_buffer[0] != 0)
        {
            ID3ExistFlag = TRUE;
        }
    }

    if (ID3Info->TENC_length != 0)
    {
        DealStringEnd(ID3Info->TENC_buffer, string_id);
        DealStrEndSpace(ID3Info->TENC_buffer);
        if (ID3Info->TENC_buffer[0] != 0)
        {
            ID3ExistFlag = TRUE;
        }
    }

    ClearWatchDog();
    return ID3ExistFlag;

}

/*
 ********************************************************************************
 *             DealStringEnd
 *
 * Description :  处理定符串尾的不完整汉字问题
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void DealStringEnd(uint8 *str, int16 string_id)
{
    BYTE i;
    bool half;

    if ((string_id != CHINESE_SIMPLIFIED) && (string_id != CHINESE_TRADITIONAL))
    {
        return;
    }

    if (*((uint16 *)str) == 0xfffe)
    {
        return;
    }
    //如是空串不用处理
    if (str[0] == 0) //0 means '\0'
    {
        return;
    }

    for (i = 0; i < 255; i++)
    {
        if (str[i] >= 0x80)
        {
            half = TRUE;
        }
        else
        {
            half = FALSE;
        }

        //是否将结束
        if (str[i + 1] == 0)
        {
            if (FALSE != half)
            {
                str[i] = ' ';
            }
            return;
        }

        if (FALSE != half)
        {
            i++;
            if (str[i + 1] == 0) //刚好是完整的
            {
                return;
            }

            half = FALSE;
        }
    }//end for

}
/*
 ********************************************************************************
 *             DealStringEnd
 *
 * Description :  处理字符串后的空格的问题
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void DealStrEndSpace(char *str)
{
    BYTE i, j;
    /*统计字符串尾部的空格符号*/
    BYTE space_num = 0;
    /*处理unicode编码big endian的情况,去掉全是空格的问题*/
    if (*((WORD *) &str[0]) == 0xfffe)
    {
        WORD codeChar;
        for (i = 0, j = 0; i < 253; i += 2)
        {
            codeChar = *((WORD *) &str[i + 2]);
            if (codeChar == 0x0000)
            {
                break;
            }
            /*只去掉字符尾部的空格符*/
            if (codeChar != 0x2000)
            {
                j = i;
                space_num = 0;
            }
            else
            {
                space_num++;
            }
        }
        /*处理字符全是空格的情况*/
        if (j == 0)
        {
            *((WORD *) &str[0]) = 0x0000;
        }
        /*去掉字符串尾部的空格*/
        else if (j == (i - (space_num + 1) * 2))
        {
            /*尾部空格数不为0才处理*/
            if (space_num != 0)
            {
                *((WORD *) &str[j + 2]) = 0x0000;
            }
        }
        else
        {
        }
    }
    else
    {

        for (i = 0, j = 0; i < 255; i++)
        {
            if (str[i] == 0)
            {
                break;
            }
            /*只去掉字符尾部的空格符*/
            if (str[i] != ' ')
            {
                j = i;
                space_num = 0;
            }
            else
            {
                space_num++;
            }
        }
        if (j == 0)
        {
            str[j] = 0;
        }
        /*去掉字符串尾部的空格符号*/
        else if (j == (i - (space_num + 1)))
        {
            /*尾部空格数不为0才处理*/
            if (space_num != 0)
            {
                str[j + 1] = 0;
            }
        }
        else
        {
        }
    }
}
/*
 ********************************************************************************
 *             FectchID3Info
 *
 * Description :
 *
 * Arguments   :  获取fileName歌曲的ID3信息，放到ID3Info_t数据结构中
 *
 * Returns     : ID3ExistFlag = 0 表示没有ID3信息，1表示有
 *
 * Notes       :
 *
 ********************************************************************************
 */

bool FectchID3Info(const char *filename, ID3Info_t *ID3Info)
{
    handle TagFileHandle;//读文件指针
    int LastPageLen; //最后一页的有效节数
    long FileLen;
    bool Fectch_ret;
    char TAGString[] = "TAG"; //ID3V1的头应该是"tag"

    ClearWatchDog();
    // TempBuf = ID3_BUFFER;
    TempBuf = GET_REAL_ADDR(ID3_BUFFER);
    TagFileHandle = FS_FOpen(filename, FS_OPEN_NORMAL);//打开文件
    if (TagFileHandle == NULL)
    {
        return FALSE;
    }

    Fectch_ret = FetchID3_V2info(TagFileHandle, ID3Info);
    if(actrec_flag)
    {
        FS_FClose(TagFileHandle);
        return 0;
    }
    if (Fectch_ret == TRUE)
    {
        FS_FClose(TagFileHandle);
        return 1;
    }

    ClearWatchDog();
    //LastPageLen是文件最后一个簇的字节数
    LastPageLen = FS_GetUnalignedLen(TagFileHandle);
    //FileLen是文件的总簇数，每个簇512字节
    FileLen = FS_GetFileLen(TagFileHandle);
    FS_FSeek(FileLen - 2, 0, TagFileHandle); //先读一个PAGE
    FS_FRead(TempBuf, 1, TagFileHandle); //

    if(LastPageLen == 0)
    {
        LastPageLen = 512;
    }

    //这段是可以优化的，先判断是否小于128再读。如果最后一个扇区大于128，则只读一次
    if (LastPageLen < 128)
    {
        //取128bytes放入 128bytes的BUFFER中
        memcpy(TagBuf, TempBuf + 512 - 128 + LastPageLen, ((uint16)(128 - LastPageLen)));
        FS_FRead(TempBuf, 1, TagFileHandle);
        memcpy((TagBuf + 128 - LastPageLen), TempBuf, ((uint16)LastPageLen));
    }
    else
    {
        FS_FRead(TempBuf, 1, TagFileHandle);
        memcpy(TagBuf, TempBuf + LastPageLen - 128, 128);
    }
    FS_FClose(TagFileHandle);

    if (memcmp(TagBuf, TAGString, 3) == 0)
    {
        memcpy(ID3Info->TIT2_buffer, (char *) &TagBuf[3], ID3Info->TIT2_length - 2);
        *((BYTE *) (ID3Info->TIT2_buffer) + ID3Info->TIT2_length - 2) = 0x00; //这里补字符串结束


        memcpy(ID3Info->TPE1_buffer, (char *) &TagBuf[33], ID3Info->TPE1_length - 2);
        *((BYTE *) (ID3Info->TPE1_buffer) + ID3Info->TPE1_length - 2) = 0x00; //这里补字符串结束


        memcpy(ID3Info->TALB_buffer, (char *) &TagBuf[63], ID3Info->TALB_length - 2);
        *((BYTE *) (ID3Info->TALB_buffer) + ID3Info->TALB_length - 2) = 0x00; //这里补字符串结束


        return 1;
    }
    else
    {
        return 0;
    }

}

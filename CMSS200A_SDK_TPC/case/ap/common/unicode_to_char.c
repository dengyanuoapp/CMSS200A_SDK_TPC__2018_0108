/*
 *******************************************************************************
 *                ACTOS AP
 *        enhanced module impletementation
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *******************************************************************************
 */
#include "actos.h"
//nclude "lang_id.h"
#include "stringH.h"

#if defined(MAIN_AP) || defined(MUSIC_AP) || defined(RADIO_AP) || defined(RECORD_AP) || defined(LINEIN_AP)
#pragma name(UNI_TO_CHAR)
#define LAN_ID_SCHINESE 0
const uint8 UniToMBTab[][12] =
{
    "FTBL_GB.$$$", //��������codepage table
    "FTBL_B5.$$$", //��������codepage table
    "CP1252.L", //Ӣ��,��ŷ��ϵ codepage table
    "CP932.L", //���� codepage table
    "CP949.L", //���� codepage table
    "CP1257.L", //���޵غ���ϵ
    "CP1251.L", //������ϵ
    "CP1254.L", //��������ϵ
    "CP1255.L", //ϣ������
    "CP874.L", //̩��
    "CP1250.L", //��ŷ����ŷ��ϵ
    "CP1256.L", //��������ϵ
    "CP1253.L" //ϣ��
};

uint8 far TwoByteCode = 0;
uint32 far UniToMBTabAddr = 0;
uint16 unicodetochar(uint16 code_rename, uint8 LanguageID);
uint8 string_unitochar(uint8 *str, uint8 len, uint8 lang_id);

static void swap_int_byte(uint16 *a)
{
    uint8 tmp;
    uint8 *p = (uint8 *)a;
    tmp = *p;
    *p = *(p + 1);
    *(p + 1) = tmp;
}
/*
 ********************************************************************************
 *
 *
 * Description :
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
char GetLangIdex(unsigned char lang_id)
{
    unsigned char table_index = 0;
    TwoByteCode = 0;
    table_index = 0;
    TwoByteCode = 1;
    lang_id = lang_id;
#if 0
    switch(lang_id)
    {
    case LAN_ID_SCHINESE:
        table_index = 0;
        TwoByteCode = 1;
        break;
    case LAN_ID_TCHINESE:
        table_index = 1;
        TwoByteCode = 1;
        break;
    case LAN_ID_ENGLISH:
    case LAN_ID_FRENCH:
    case LAN_ID_GERMANY:
    case LAN_ID_ITALIAN:
    case LAN_ID_DUTCH:
    case LAN_ID_PORTUGUESE:
    case LAN_ID_SPANISH:
    case LAN_ID_SWEDISH:
    case LAN_ID_FINNISH:
    case LAN_ID_DENISH:
        table_index = 2;
        break;
    case LAN_ID_JAPANESE:
        table_index = 3;
        TwoByteCode = 1;
        break;
    case LAN_ID_KOREAN:
        table_index = 4;
        TwoByteCode = 1;
        break;
    case LAN_ID_CROATIA:
    case LAN_ID_SLOVAKIAN:
        table_index = 5;
        break;
    case LAN_ID_RUSSIAN:
        table_index = 6;
        break;
    case LAN_ID_TURKISH:
    case LAN_ID_INDONESIAN:
        table_index = 7;
        break;
    case LAN_ID_HEBREW:
        table_index = 8;
        break;
    case LAN_ID_THAI:
        table_index = 9;
        break;
    case LAN_ID_HUNGARIAN:
    case LAN_ID_ROMANIAN:
    case LAN_ID_CZECHISH:
    case LAN_ID_POLAND:
        table_index = 10;
        break;
    case LAN_ID_RABBINIC:
        table_index = 11;
        break;
    case LAN_ID_GREEK:
        table_index = 12;
        break;
    default:
        table_index = 0;
        break;
    }
#endif
    return table_index;
}

uint16 unicodetochar(uint16 code_rename, uint8 LanguageID)
{
    uint16 CharCode;
    uint32 FontCode;
    SD_RW sdrw;
    sdrw.srampage = 0x00;
    sdrw.sramaddr = (uint16) & CharCode;
    CharCode = code_rename;
    swap_int_byte((uint16 *)&code_rename);
    if (code_rename > 127)
    {

        FontCode = code_rename;
        if (LanguageID == LAN_ID_SCHINESE)
        {
            FontCode = FontCode << 1;
            if (FontCode < 0x1FE00 && FontCode >= 0x13F50)
            {
                CharCode = 0x2000; //ע���С��ת��
                return CharCode;
            }
            else if (FontCode >= 0x1FE00)
                FontCode = FontCode - 0xBEB0;
            sdrw.faddr = UniToMBTabAddr + FontCode;
        }
        else
        {
            sdrw.faddr = UniToMBTabAddr + (((uint32) FontCode) << 1);
        }
        SD_Read((SD_RW *) &sdrw, 2);
        if (CharCode == 0)
            CharCode = '?';
    }
    return CharCode;//TRUE;
}

uint8 string_unitochar(uint8 *str, uint8 len, uint8 lang_id)
{
    uint16 s_cnt;
    uint16 d_cnt;
    uint16 FontCode;
    uint16 CharCode;
    if (len < 0)
        len = 127;

    for (d_cnt = 0, s_cnt = 0; (s_cnt < len) && ((str[s_cnt] != 0) || (str[s_cnt + 1] != 0));)
    {
        FontCode = *((uint16 *) &str[s_cnt]);

        if (FontCode == 0x0000)
            break;
        FontCode = unicodetochar(FontCode, lang_id);
        CharCode = FontCode;
        swap_int_byte((uint16 *) &CharCode);

        if (TwoByteCode == 1 && CharCode > 0x7f) //���ֽ������ת��

        {
            str[d_cnt++] = *((uint8 *) (&CharCode) + 1); //�Ѹ�λ�Ƶ���λȥ
            str[d_cnt++] = *((uint8 *) (&CharCode)); //�Ѹ�λ�Ƶ���λȥ
        }
        else
        {
            str[d_cnt++] = *((uint8 *) (&CharCode) + 1); //���˵�λ(&code_rename)+1);
        }
        s_cnt += 2;
    }
    str[d_cnt] = 0;

    return d_cnt;
}

bool unitochar(char *str, int len, unsigned char lang_id)
{
    SD_FILE *fp;
    unsigned char table_index = 0;
    uint8 result = 0;

    table_index = GetLangIdex(lang_id);
    ///��unicode to char ת����
    fp = SD_FOpen(UniToMBTab[table_index], MODE_READ);
    if (fp == NULL)
    {
        return FALSE;
    }
    UniToMBTabAddr = fp->startaddr;
    SD_FClose(fp);
    result = string_unitochar(str, len, lang_id);

    return result;
}
#endif

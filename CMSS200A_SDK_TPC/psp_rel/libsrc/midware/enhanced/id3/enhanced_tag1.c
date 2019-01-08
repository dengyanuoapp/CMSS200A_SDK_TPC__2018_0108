/*
 *******************************************************************************
 *                ACTOS AP
 *        enhanced module impletementation
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 * File   : enhanced_tag2.c
 * By     : ouyuping
 * Version: v1.00     first version     3/23/2009
 *******************************************************************************
 */

#include <actos.h>
#include <enhanced.h>
#include <stringH.h>
//#include "debug.h"

#pragma name(MW_EH_ID31)

#pragma renamecode(MW_EH_ID31)

#define ID3_COUNT 3 //需要查找的FrameID数(包括标题，作者，专辑)
/*
 *********************************************************
 1) "RTENC"         :编码者      2) "TRCK"        :音轨
 3) "TPE1"          :演唱者      4) "TIT2"        :标题
 5) "TALB"          :专辑        6) "TYER"        :年代
 7) "TMED"          :媒体类型    8) "TCOM"        :作曲家
 9) "COMM"          :注释       10) "CON"         :流派
 11) "TOPE"          :原创者     12) "TCOP"        :版权
 13) "WXXX"          :URL
 *********************************************************
 */

typedef struct
{
    char Header[3];/*必须为"ID3"否则认为标签不存在*/
    BYTE Ver;/*版本号;ID3V2.3就记录03,ID3V2.4就记录04*/
    BYTE Revision;/*副版本号;此版本记录为00*/
    char Flag;/*存放标志的字节，这个版本只定义了三位*/
    char Size[4];/*标签大小，包括标签头的10个字节和所有的标签帧的大小*/
} ID3_10_header;//ID3V2.*标签头(前10byte)

typedef struct
{
    BYTE bytes_of_header;//不同ID3版本Frame Header所占字节数
    BYTE bytes_of_title;//不同ID3版本Frame Header中FrameID所占字节数
    BYTE bytes_of_framesize;//不同ID3版本Frame Header中帧长所占字节数
    char *id3table;//不同ID3版本对应的需要查找的FrameID Table
    BYTE id3tablesize;//对应FrameID Table的尺寸
} ID3_Version_Info;//

typedef struct
{
    char FrameID[4]; /*用四个字符标识一个帧，说明其内容，稍后有常用的标识对照表*/
    char Size[4]; /*帧内容的大小，不包括帧头，不得小于1*/
    char Flags[2];/*存放标志，只定义了6位*/
    //char encodetype;//frame  编码类别
} ID3_2_3_HEADER;

typedef struct
{
    char FrameID[3]; /*用四个字符标识一个帧，说明其内容*/
    char Size[3]; /*帧内容的大小，不包括帧头，不得小于1*/
    //    char encodetype; //frame  编码类别
} ID3_2_2_HEADER;

typedef struct
{
    uchar savelen; // FrameID保存的最长长度
    char *saveaddr; // FrameID保存地址
} ID3SaveItem;

extern char *TempBuf;
uint8 karaoke_flag;
uint8 actrec_flag;
const char ID3TitleTable_v2_3[] = "TIT2TPE1TALB";// ID3V2.3需要查找的FrameID Table
const char ID3TitleTable_v2_2[] = "TT2TP1TAL"; // ID3V2.2需要查找的FrameID Table

const char COMM_NAME[] = "COMM";
const char k123_name[] = "k123";
const char a123_name[] = "A123";
const ID3_Version_Info ID3ver[2] =
{
    {
        sizeof(ID3_2_3_HEADER),
        4,//size of ID3_2_3_HEADER's FrameID
        4,//size of ID3_2_3_HEADER's Size
        (char *) ID3TitleTable_v2_3,
        sizeof(ID3TitleTable_v2_3)
    },
    {
        sizeof(ID3_2_2_HEADER),
        3,//size of ID3_2_2_HEADER's FrameID
        3,//size of ID3_2_2_HEADER's Size
        (char *) ID3TitleTable_v2_2,
        sizeof(ID3TitleTable_v2_2)
    }
};
ID3SaveItem *pId3Save;//FrameID存储位置
ID3Info_t *pID3InfoStart;//ap层提供的存储结构
ID3_10_header far id3h;
int far bufindex;//当前TempBuf未处理字符的索引
int far findcount;//成功查找到的FrameID数
dword far filepos;//当前文件的精确指针
static dword far framepos;//当前Frame的地址
static dword far framesize;//当前Frame的size(不包括编码类型位)
HANDLE far TagHandle;
BYTE far AvailFlag[ID3_COUNT];//有效FrameID查找登记表
BYTE far curversion;//当前ID3版本
char *TempBuf; //read file buffer

/********************************************************************************
 * Description : 检查是否是有效的待查找frameID(title)
 *
 * Arguments  :
 *             pid3header : Frame Header 其头四个字节是frameId，检测它是否 我们所要找的id
 要的ID是  ID3TitleTable_v2_3[] = "TIT2TPE1TALB"，每个四个字节bytes_of_title
 AvailFlag[0]对应着"TIT2"
 * Returns     :
 *            TRUE: 有效,
 *            FALSE: 无效.
 * Notes       :
 *
 ********************************************************************************/

void Check_Comm_Item(char *pid3header);

BOOL IsAvailFrameID(char *pid3header)
{
    int i = 0, j = 0, index;

    ClearWatchDog();
    while ((i < ID3ver[curversion].id3tablesize) && (j < ID3ver[curversion].bytes_of_title))
    {
        if (pid3header[j] == ID3ver[curversion].id3table[i])
        {
            j++;
        }
        else
        {
            j = 0;
        }
        i++;
    }

    //没找到我们所要的ID
    if ((j < ID3ver[curversion].bytes_of_title) || ((i % ID3ver[curversion].bytes_of_title) != 0))
    {
        return 0;
    }

    index = (i / ID3ver[curversion].bytes_of_title) - 1;
    if (AvailFlag[index] == 1)//之前已经有了，还出现一条，不记录
    {
        return 0;
    }

    AvailFlag[index] = 1;//记录找到的标签

    pId3Save = (ID3SaveItem *) pID3InfoStart + index; //设置将要保存的地址

    return TRUE;
}

/********************************************************************************
 * Description : 处理Frame Header，计算Frame size
 *
 * Arguments  :
 *             pid3header : Frame Header
 * Returns     :
 *            TRUE: 成功,
 *            FALSE: 失败.
 * Notes       :
 *
 ********************************************************************************/

BOOL GetFrameSize(char *pid3header)
{
    int i = 0;
    uint8 *pfrmsaddr = (uint8 *)(pid3header + ID3ver[curversion].bytes_of_title);
    dword size = 0;

    if(id3h.Ver == 4)	//ID3v2.4
    {
        while(i < 4)
        {
            size <<= 7;
            size |= (dword)(pfrmsaddr[i] & 0x7f);
            i++;
        }
    }
    else
    {
        while (i < ID3ver[curversion].bytes_of_framesize)
        {
            size <<= 8;
            size += (dword) pfrmsaddr[i];
            i++;
        }
    }
    framesize = size;
    return TRUE;
}

/********************************************************************************
 * Description : 精确文件读取初始化
 *
 * Arguments  :
 *
 * Returns     :
 *            TRUE: 成功,
 *            FALSE: 失败.
 * Notes       :
 *
 ********************************************************************************/

bool FI_FInit(void)
{
    bufindex = 0;
    filepos = 0;

    FS_FRead(TempBuf, 1, TagHandle); //读一PAGE数据
    return TRUE;
}

/********************************************************************************
 * Description : 精确文件读取SEEK
 *
 * Arguments  :
 *             fpos:从文件头seek的字节偏移量
 * Returns     :
 *            TRUE: 成功,
 *            FALSE: 失败.
 * Notes       :
 *
 ********************************************************************************/

BOOL FI_FSeek(dword fpos)
{
    BOOL result;
    int32 fpagepos = (int32)(fpos >> 9);
    int32 curpagepos = (int32)(filepos >> 9);

    filepos = fpos;
    bufindex = (int16)(fpos & 0x1ff);

    if (fpagepos != curpagepos)
    {
        result = FS_FSeek(fpagepos, 0, TagHandle);
        if (result == 0)
        {
            return FALSE;
        }

        result = FS_FRead(TempBuf, 1, TagHandle);
        if (result == 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/********************************************************************************
 * Description : 精确文件读取READ
 *
 * Arguments  :
 *             databuf:读取文件数据的buf
 *             length:读取的长度，字节为单位
 * Returns     :
 *            TRUE: 成功,
 *            FALSE: 失败.
 * Notes       :
 *
 ********************************************************************************/

BOOL FI_FRead(char *databuf, int length)
{
    int i = 0;
    BOOL result;

    //一个一个字节复制，可以进行优化
    ClearWatchDog();
    while ((bufindex < 512) && (i < length))
    {
        databuf[i] = TempBuf[bufindex];
        i++;
        bufindex++;
        if (bufindex == 512)
        {
            //            result = FS_FSeek(1, 1, TagHandle); //当前位置先后偏移一页 512byte
            //            if (result == 0)
            //            {
            //                return FALSE;
            //            }

            result = FS_FRead(TempBuf, 1, TagHandle);
            if (result == 0)
            {
                return FALSE;
            }

            bufindex = 0;
        }
    }

    filepos += (uint32)length;
    return TRUE;
}

/********************************************************************************
 * Description : 保存当前Frame
 *
 * Arguments  :
 *             len: Frame的有效长度  pId3Save->savelen是30，之前已经设置了的
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************/

void SaveTagInfo(dword len)
{
    dword readlen = len;
    if (readlen == 0) //may be is 0
    {
        pId3Save->saveaddr[0] = '\0';
        return;
    }
    FI_FRead(pId3Save->saveaddr, 1);
    if (((pId3Save->saveaddr[0]) == 0) || ((pId3Save->saveaddr[0]) == 1))
    {
        if ((readlen - 1) > (pId3Save->savelen - 2))
        {
            readlen = pId3Save->savelen - 2;
        }
        else
        {
            readlen -= 1;//framesize include the encodetype
        }
        FI_FRead(pId3Save->saveaddr, (int16)readlen);
        //pId3Save->saveaddr[readlen] = '\0';
        //pId3Save->saveaddr[readlen + 1] = '\0';
    }
    else   //no encode type
    {
        if ((readlen) > (pId3Save->savelen - 2))
        {
            readlen = pId3Save->savelen - 2;
        }
        FI_FRead(pId3Save->saveaddr + 1, ((int16)(readlen - 1)));
    }
    pId3Save->saveaddr[readlen] = '\0';
    pId3Save->saveaddr[readlen + 1] = '\0';
}

/********************************************************************************
 * Description : Frame解析
 *
 * Arguments  :
 *
 * Returns     :
 *            Frame的整长
 * Notes       :
 *
 ********************************************************************************/

dword ParseFrame(void)
{
    char id3header[12];

    //获取一个头到id3header
    FI_FRead(id3header, ID3ver[curversion].bytes_of_header);
    if ((id3header[0] == 0) && (id3header[1] == 0) && (id3header[2] == 0))
    {
        return 0;
    }

    //处理一个头，获得该信息帧的长度
    GetFrameSize(id3header);

    if (FALSE != IsAvailFrameID(id3header))
    {
        SaveTagInfo(framesize);	 //framesize include the encode type
        findcount++;
    }
    else
    {
        Check_Comm_Item(id3header);
    }
    return framesize + ID3ver[curversion].bytes_of_header; //帧长加帧头长
}

/********************************************************************************
 * Description : 读取V2版本ID3 Tag
 *
 * Arguments  :
 *             TagFileHandle:待处理文件句柄
 *             pID3Info:ID3 Tag保存结构体指针
 * Returns     :
 *            TRUE: 成功,
 *            FALSE: 失败.
 * Notes       :
 *
 ********************************************************************************/

BOOL FetchID3_V2info(handle TagFileHandle, ID3Info_t *pID3Info)
{
    dword ID3Size;
    dword whole_framesize;

    TagHandle = TagFileHandle;
    pID3InfoStart = pID3Info;//信息隐藏，pID3Info是传进来的参数，而pID3InfoStart等同，但其在TAG模块中是全局访问的

    findcount = 0;
    memset(AvailFlag, 0, sizeof(AvailFlag));
    curversion = 0;//默认ID3v2.3版本

    ClearWatchDog();

    FI_FInit();//读取512byte到 TempBuf
    FI_FRead((char *) &id3h, sizeof(id3h));

    if ((id3h.Header[0] != 'I') || (id3h.Header[1] != 'D') || (id3h.Header[2] != '3'))
    {
        return FALSE;
    }

    if (id3h.Ver < 3)
    {
        curversion = 1;//ID3v2.2版本
    }

    //ID3Size是整个ID3V2的长度
    ID3Size = ((dword) (id3h.Size[0] & 0x7F) << 21) + ((dword) (id3h.Size[1] & 0x7F) << 14) + ((dword) (id3h.Size[2]
              & 0x7F) << 7) + (dword) (id3h.Size[3] & 0x7F) + sizeof(id3h);

    framepos = sizeof(id3h);

    while ((framepos < ID3Size) && (findcount < ID3_COUNT))
    {
        ClearWatchDog();

        //whole_framesize一帧的长度
        whole_framesize = ParseFrame();
        if (whole_framesize == 0) //deal with the padding byte
        {
            break;
        }
        framepos += whole_framesize;

        //SaveTagInfo 里使用的len(最大是30)不一定是frameSize ，因此要重新对齐
        if (!FI_FSeek(framepos))
        {
            break;
        }
    }

    if (findcount > 0)
    {
        return TRUE;
    }
    return FALSE;
}


void Check_Comm_Item(char *pid3header)
{
    int i;
    char tmpbuf[9];
    for(i = 0; i < 4; i++)
    {
        if(pid3header[i] != COMM_NAME[i])
        {
            return;
        }
    }

    FI_FRead(tmpbuf, 9);
    if(!memcmp(&tmpbuf[5], k123_name, 4))
    {
        //karaoke_flag |= 0x2;
        karaoke_flag = 0x2;
    }
    else if(!memcmp(&tmpbuf[5], a123_name, 4))
    {
        //karaoke_flag |= 0x2;
        actrec_flag = 0x1;
    }
}

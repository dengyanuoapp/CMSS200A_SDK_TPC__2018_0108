/*
 *******************************************************************************
 *                File selector
 *            enhanced library
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *******************************************************************************
 */
#include <actos.h>
#include <enhanced.h>
#include <stringH.h>
#include <sysdef.h>
#include <rand.h>

#define DIR_NEXT 0
#define DIR_PREV 1

#pragma name(MW_EH_FSEL_NEXT)

extern uint8 fselMode; //Mode of select, Sequence, repeat one ...
extern uint8 fselPLOrCommonDir; //playlist or common operation
extern uint8 fselType; //Type of select,  Music or voice.
extern uint8 fselError; // store error id;
extern int8 fselDiskLetter;
extern int8 fselDirLayer; //dir  layer
extern bool select_file_begin_flag;

extern uint16 fselFileNoInDisk;// 文件在全盘的序号
extern uint16 fselTotalInDisk;// 全盘文件总数
extern uint16 CurDirFileNum;
extern uint16 fselDirNoInDisk;
extern uint16 fselTotalDir;
extern uint32 CurDirClusNo;
extern uint8 dir_list_flag;
extern bool fselGetPrev(uint8 *strFile);
extern uint16 shuffle_next_file(void);
extern bool init_shuffle(uint16 num_total);

#ifdef _ACTIVE_DIRL
extern uint8  support_fav;
dir_location_t Dirlocation[DIR_PRE_SEC];
#endif

const uint8 ExtMusicNext[][4] =
{ "MP3", "WMA", "WAV", "MP1", "MP2" };
//const uint8 ExtVoiceNext[][4] =
//{ "WAV", "MP3" };
const uint8 IgnoreExtNext[4] =
{ "*  " };
const uint8 ExtAllNext[][4] =
{ "*  " };

extern uint16 GetFileTotal(uint8 *strDir, bool bCheckValidDir);
extern bool SelectPrev_8Layer(uint8 *strFile);

bool fselGetFileByNoInDiskC (char *strFile, const uint16 Num);

#ifdef _ACTIVE_DIRL
static bool Fast_SelectNext_8Layer(uint8 *strFile);
static bool ReadDir();
#endif

#pragma renamecode(MW_EH_FSEL_NEXT)

static uint8 GetExtSize(void)
{
    if (fselType == FSEL_TYPE_MUSIC)
    {
        return sizeof(ExtMusicNext) / 4;
    }
    //    else if (fselType == FSEL_TYPE_VOICE)
    //    {
    //        return sizeof(ExtVoiceNext) / 4;
    //    }
    else
    {
        return 0;
    }
    return 0;
}

/********************************************************************************
 * Description : Get name buffer of current extend
 *
 * Arguments  :  获得歌曲扩展名
 * Returns     :  the buffer of current file extend name
 * Notes       :     None
 *
 ********************************************************************************/
static const uint8 *GetExtBuf(void)
{
    ClearWatchDog();
    if (fselType == FSEL_TYPE_MUSIC)
    {
        return ExtMusicNext[0];
    }
    //    else if (fselType == FSEL_TYPE_VOICE)
    //    {
    //        return ExtVoiceNext[0];
    //    }
    else if (fselType == DIRECTORY_ALL)
    {
        return ExtAllNext[0];
    }
    else
    {
        return 0;
    }
}
/*
 *******************************************************************************
 * Description : select the next file and get the file name
 *
 * Arguments   :
 * Returns     :文件初始化时 fselFileNo = 0；调用SelectNext获得第一个文件后，fselFileNo = 1；因此fselFileNo
 * Notes       : 表示将要读的那个文件序号(从0开始)，每调用一次select，fselFileNo都会加1
 *
 *******************************************************************************
 */
bool SelectNext(uint8 *strFile)
{
    BYTE select_type;
    if (FALSE != select_file_begin_flag)
    {
        select_type = FS_DIR_FILE;  //从目录首开始
    }
    else
    {
        select_type = FS_DIR_FILE_CONT;
    }
    select_file_begin_flag = FALSE;
    return FS_Dir(strFile, GetExtBuf(), select_type, GetExtSize());
}

/*
 *******************************************************************************
 * Description : Select the next directory of current select directory
 * 选择上下文件夹的时候 fselDirno[fselDirlayer] != 0	,其他情况是等于0的
 * Arguments  : 进入下一个目录，并修改fselDirNo[fselDirLayer]
 *                 strDir: Dos 8.3 directory name of the next.
 *
 * Returns     :
 *    TRUE:  success.
 *    FALSE: fail, detail info store in fselError.
 * Notes       :  None
 *这里的实现是查找下一个目录时根据目录数，每次都从第一个开始dir，dir到所需的目录序号，太慢了
 *******************************************************************************
 */
bool fselNextDir(uint8 *strDir)
{
    return FS_Dir(strDir, IgnoreExtNext, FS_DIR_DIR_CONT, 1);
}

/********************************************************************************
 * Description : 获得当前目录的某种类型文件的总数
 * Arguments  :
 *            strDir: Dos 8.3 directory name
 *            bCheckValidDir: if check valid directory only
 * Returns     :
 *            The file total of current directory when bCheckValidDir = FALSE
 or Vaild BOOL flag when bCheckValidDir = TRUE
 * Notes       : None
 *
 ********************************************************************************/
static uint16 ScanCurDir_FileNum(uint8 *strFile)
{
    int32 ClusterNo;
    int32 DirEntry;
    uint16 FileNum = 1;
    uint8 dir_buf[74];

    FS_GetCurDir_layer(dir_buf);
    ClusterNo = FS_GetCurDir();
    DirEntry = FS_GetCurDirEntry();

    FS_Dir(strFile, GetExtBuf(), FS_DIR_FILE, GetExtSize());
    while(FS_Dir(strFile, GetExtBuf(), FS_DIR_FILE_CONT, GetExtSize()))
    {
        FileNum++;
    }

    FS_SetCurDir(ClusterNo);
    FS_SetCurDirEntry(DirEntry);
    FS_SetCurDir_layer(dir_buf);

    return FileNum;
}

/********************************************************************************
 * Description : Seek to the next file
 *
 * Arguments  :
 strFile:output file name
 * Returns     :
 TRUE: Successful
 FALSE: fail.
 * Notes       :  目录树查找,先检查父目录再检查子目录(子目录1 -- 子目录99)
 *
 ********************************************************************************/
bool SelectNext_8Layer(uint8 *strFile)
{
    //在当前目录向下找文件
    char m_szTemp[12];
    fselError = 0;

    if (!SelectNext(strFile))
    {
CurrentDiragain:
        ClearWatchDog();

        //寻找该目录的第一个子文件夹
        if (FALSE != FS_Dir(m_szTemp, IgnoreExtNext, FS_DIR_DIR, 1))
        {
            if(fselDirLayer < TOTALDIRLAYER)
            {
                if (FALSE != FS_CD(FS_CD_SON))
                {
                    //进入子文件夹，获得该子文件夹的该种文件类型数
                    fselDirLayer++;
                    select_file_begin_flag = TRUE;
                    // 检查当前目录是否有合适的文件.
                    if (FALSE != SelectNext(strFile))
                    {
                        //fselError = 0;
                        fselDirNoInDisk++;
                        return TRUE;
                    }
                    else //子目录下的子目录循环查找
                    {
                        goto CurrentDiragain;
                    }
                }
                else //出错了
                {
                    fselError = FSEL_ERR_OUTOF;
                    return FALSE;
                }
            }
            else
            {
                goto ParentDiragainNext;
            }
        }
        else //如果没有子文件夹，则向上跳一层，寻找当前目录的下一个目录（平级寻找）
        {
ParentDiragainNext:

            ClearWatchDog();
            if (FALSE != FS_DirParNext())//回到父目录的下一个
            {
                fselDirLayer--;
                if (FALSE != fselNextDir(m_szTemp))
                {

                    FS_CD(FS_CD_SON);
                    fselDirLayer++;
                    select_file_begin_flag = TRUE;
                    if (FALSE != SelectNext(strFile))
                    {
                        //fselError = 0;
                        fselDirNoInDisk++;
                        return TRUE;
                    }
                    // 检查子目录.
                    goto CurrentDiragain;
                }
                else //平级没有下一级目录，到上级目录
                {
                    goto ParentDiragainNext;
                }
            }
            else
            {
                fselError = FSEL_ERR_OUTOF;
                return FALSE;
            }
        }
    }
    return TRUE;
}
/********************************************************************************
 * Description : Select the next file according to type & mode, then get the name of file.
 *
 * Arguments  :
 *            strFile:  msdos 8.3  file name of next file
 * Returns     :true表示成功，false表示失败
 *            TRUE: Successful
 *            FALSE: Fail, detail information in fselError
 * Notes       :  None
 *
 ********************************************************************************/

bool fselGetNext(uint8 *strFile)
{
    uint16 num;
    uint8 fileNameTmp[12];
    bool result;
    uint32 TempClusNo;

    ClearWatchDog();

    if((fselTotalInDisk == 0) && (fselMode != FSEL_ALL_SEQUENCE)) //非初始化遍历时
    {
        return FALSE;
    }
    switch (fselMode)
    {
        //重复当前一个文件
    case FSEL_ALL_REPEATONE:
    {
        if (!FS_GetName(strFile, 0))
        {
            return FALSE;
        }
        return TRUE;
    }

    //当前目录下的随机播放
    case FSEL_ALL_RANDOM:
    {
#ifdef _ACTIVE_SHUFFLE
        num = shuffle_next_file();
        if(num == 0)
        {
            init_shuffle(fselTotalInDisk);
        }
#else
        num = (rand() % fselTotalInDisk) + 1;
#endif
        fselMode = FSEL_ALL_REPEATALL;
        result = fselGetFileByNoInDiskC(strFile, num);
        fselMode = FSEL_ALL_RANDOM;
        return result;
    }

    //所有文件顺序读取，不重复
    case FSEL_ALL_SEQUENCE:
    {
        if (FALSE != SelectNext_8Layer(strFile))
        {
            fselFileNoInDisk++;
            if((fselTotalInDisk != 0) && (fselFileNoInDisk > fselTotalInDisk))
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    //所有文件顺序读取，重复
    case FSEL_ALL_REPEATALL:
    {
        if(dir_list_flag == 0)
        {
            result = SelectNext_8Layer(strFile);
        }
#ifdef _ACTIVE_DIRL
        else if(dir_list_flag == 1)
        {
            result = Fast_SelectNext_8Layer(strFile);
        }
#endif
        else
        {
            return FALSE;
        }

        if (FALSE != result)
        {
            fselFileNoInDisk++;
            if(fselFileNoInDisk > fselTotalInDisk)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            fselError = 0;
            fselFileNoInDisk = 0;

            if(dir_list_flag == 0)
            {
                if(FALSE == FS_CD(":"))
                {
                    return FALSE;
                }
                if(FS_Dir(fileNameTmp, GetExtBuf(), FS_DIR_FILE, GetExtSize()))
                {
                    fselDirNoInDisk = 1;    //根目录有文件，目录序号先加1
                }
                else
                {
                    fselDirNoInDisk = 0;
                }
            }
#ifdef _ACTIVE_DIRL
            else if(dir_list_flag == 1)
            {
                fselDirNoInDisk = 1;
                if(ReadDir() == 0)
                {
                    return FALSE;
                }
            }
#endif
            else
            {
                return FALSE;
            }

            select_file_begin_flag = TRUE;
            if (FALSE != SelectNext_8Layer(strFile))
            {
                fselFileNoInDisk++;
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }

    //当前目录重复播放
    case FSEL_DIR_REPEATALL:
    {
        TempClusNo = FS_GetCurDir();
        if(CurDirClusNo != TempClusNo)
        {
            CurDirClusNo = TempClusNo;
            CurDirFileNum = ScanCurDir_FileNum(strFile);
        }
        if(FS_Dir(strFile, GetExtBuf(), FS_DIR_FILE_CONT, GetExtSize()))
        {
            fselFileNoInDisk++;
        }
        else    //已经到当前目录最后一首歌，返回目录首开始
        {
            if(!FS_Dir(strFile, GetExtBuf(), FS_DIR_FILE, GetExtSize()))
            {
                return FALSE;
            }
            fselFileNoInDisk = fselFileNoInDisk - CurDirFileNum + 1;
        }
        return TRUE;
    }

    default:
        return FALSE;
    }
}

bool fselGetFileByNoInDiskC (char *strFile, const uint16 Num)
{
    uint16 dir_num;
    bool dir_flag;
    uint8 fileNameTmp[12];
    if ((Num == 0) || (Num > fselTotalInDisk))
    {
        return FALSE;
    }

    if (fselFileNoInDisk < Num)
    {
        dir_num = Num - fselFileNoInDisk;
        dir_flag = DIR_NEXT;
    }
    else if (fselFileNoInDisk > Num)
    {
        //该条件是为了解决数字点歌时，如果目录过多，VRAM存放不了那么多文件信息后
        //选择通过往前往后dir的方式选择选择时，往前切换文件慢的问题。
        if(dir_list_flag == 0)
        {
            fselDirLayer = 0;
            fselFileNoInDisk = 0;
            fselDirNoInDisk = 0;
            CurDirClusNo = 0xfffffff0;  //初始化文件不可能使用的簇号
            CurDirFileNum = 0;   //当前目录循环播放时使用
            select_file_begin_flag = TRUE;
            if(FALSE == FS_CD(":"))
            {
                return FALSE;
            }
            if(FS_Dir(fileNameTmp, GetExtBuf(), FS_DIR_FILE, GetExtSize()))
            {
                fselDirNoInDisk = 1;    //根目录有文件，目录序号先加1
            }
            else
            {
                fselDirNoInDisk = 0;
            }
            dir_num = Num - fselFileNoInDisk;
            dir_flag = DIR_NEXT;
        }
        else
        {
            dir_num = fselFileNoInDisk - Num;
            dir_flag = DIR_PREV;
        }

    }
    else
    {
        return FS_GetName(strFile, 0);
    }

    while(dir_num > 0)
    {
        if (dir_flag == DIR_NEXT)
        {
            if(FALSE == fselGetNext(strFile))
            {
                return FALSE;
            }
        }
        else
        {
            if(FALSE == fselGetPrev(strFile))
            {
                return FALSE;
            }
        }
        dir_num--;
    }
    fselFileNoInDisk = Num;
    return TRUE;
}
void get_total_num_from_disk(file_location_t *location)
{
    uint8 fileNameTmp[12];
    memset(fileNameTmp, 0x0, 12);
    while(FALSE != SelectNext_8Layer(fileNameTmp))
    {
        fselTotalInDisk++;
        if(fselTotalInDisk == location->fselFileNo)
        {
            if(memcmp(fileNameTmp, location->filename, 12) != 0)
            {
                location->fselTotal = 0;
            }
        }
        memset(fileNameTmp, 0x0, 12);
    }
    fselTotalDir = fselDirNoInDisk;
}
#ifdef _ACTIVE_DIRL
void get_total_num_from_list(file_location_t *location)
{
    uint8 fileNameTmp[12];
    uint8 dir_buf[74];
    uint16 TempfselDirNoInDisk = 0;
    uint16 VramPage = 0;
    memset(fileNameTmp, 0x0, 12);
    memset(Dirlocation, 0, 512);
    memset(dir_buf, 0, 74);
    while(FALSE != SelectNext_8Layer(fileNameTmp))
    {
        if((fselDirNoInDisk != TempfselDirNoInDisk))
        {
            fselTotalDir++;
            Dirlocation[fselDirNoInDisk - 1].ClusterNo = FS_GetCurDir();
            Dirlocation[fselDirNoInDisk - 1].dirLayer = fselDirLayer;
            FS_GetCurDir_layer(dir_buf);
            memcpy(Dirlocation[fselDirNoInDisk - 1].MulLayer, dir_buf, 74);
            if(fselDirNoInDisk % DIR_PRE_SEC == 0)
            {
                if(VramPage < 0x1400)
                {
                    VMWrite((uint8 *)Dirlocation, VM_DIR_LIST + VramPage, 512);
                    VramPage += 512;
                }
                else
                {
                    dir_list_flag = 0;
                }

                fselDirNoInDisk = 0;
            }
            TempfselDirNoInDisk = fselDirNoInDisk;
        }
        fselTotalInDisk++;
        if(fselTotalInDisk == location->fselFileNo)
        {
            if(memcmp(fileNameTmp, location->filename, 12) != 0)
            {
                location->fselTotal = 0;
            }
        }
    }
    if(VramPage < 0x1400)
    {
        VMWrite((uint8 *)Dirlocation, VM_DIR_LIST + VramPage, 512);
    }
}
#endif

/********************************************************************************
 * Description : Select the next folder file
 *
 * 切换到其他存在歌曲文件的目录，可能是平级目录或父目录或子目录
 ********************************************************************************/
bool fselGetNextFolder (uint8 *strFile)
{
    bool result;
    uint8 mode = fselMode;
    fselMode = FSEL_ALL_REPEATALL;

    while(1)
    {
        if (FALSE != FS_Dir(strFile, GetExtBuf(), FS_DIR_FILE_CONT, GetExtSize()))
        {
            fselFileNoInDisk++;
        }
        else
        {
            break;
        }
    }
    result = fselGetNext(strFile);
    fselMode = mode;
    return result;
}

#ifdef _ACTIVE_DIRL
bool Fast_SelectNext_8Layer(uint8 *strFile)
{
    if(FALSE != SelectNext(strFile))
    {
        return TRUE;
    }
    if(fselDirNoInDisk == fselTotalDir)
    {
        return FALSE;
    }
    fselDirNoInDisk++;

    ReadDir();
    if(FALSE != SelectNext(strFile))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool ReadDir()
{
    uint16 fselDirNoInDiskTemp;
    fselDirNoInDiskTemp = fselDirNoInDisk - 1;

    VMRead((uint8 *)Dirlocation, VM_DIR_LIST + (512 * (fselDirNoInDiskTemp / DIR_PRE_SEC)), 512);

    FS_SetCurDir_layer(Dirlocation[fselDirNoInDiskTemp % DIR_PRE_SEC].MulLayer);
    FS_SetCurDir(Dirlocation[fselDirNoInDiskTemp % DIR_PRE_SEC].ClusterNo);
    FS_SetCurDirEntry(0);
    fselDirLayer = Dirlocation[fselDirNoInDiskTemp % DIR_PRE_SEC].dirLayer;
    return 1;
}
#endif

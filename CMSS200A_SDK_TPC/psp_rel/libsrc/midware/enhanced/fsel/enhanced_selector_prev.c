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

#pragma name(MW_EH_FSEL_PREV)
#ifdef PC
#define fselGetByNO fselGetByNO
#endif
extern uint8 fselMode; //Mode of select, Sequence, repeat one ...
extern uint8 fselPLOrCommonDir; //playlist or common operation
extern uint8 fselType; //Type of select,  Music or voice.
extern uint8 fselError; // store error id;
extern int8 fselDiskLetter;
extern int8 fselDirLayer; //dir  layer
extern uint16 fselFileNoInDisk;// 文件在全盘的序号
extern uint16 fselTotalInDisk;// 全盘文件总数
extern uint16 CurDirFileNum;
extern uint16 fselDirNoInDisk;
extern uint16 fselTotalDir;
extern uint32 CurDirClusNo;
extern uint16 shuffle_prev_file(void);
extern uint8 dir_list_flag;

#ifdef _ACTIVE_DIRL
extern dir_location_t Dirlocation[DIR_PRE_SEC];
#endif

const int8 ExtMusicPrev[][4] =
{ "MP3", "WMA", "WAV", "MP1", "MP2" };
const int8 ExtVoicePrev[][4] =
{ "WAV", "MP3" };
const int8 IgnoreExtPrev[4] =
{ "*  " };
const int8 ExtAll1[][4] =
{ "*  " };
static const uint8 DirlistName[] = {"LIST    LIB"};

extern uint16 GetFileTotal(uint8 *strDir, BOOL bCheckValidDir);


extern bool fselGetByNO(uint8 *strFile, const uint16 Num);
extern bool fselGetNext(uint8 *strFile);
extern uint8 fselSetLocationC(file_location_t *location);
extern bool fselGetLocationC(file_location_t *location);
extern bool init_shuffle(uint16 num_total);
extern bool fselGetFileByNoInDiskC (char *strFile, const uint16 Num);

bool SelLastDir(uint8 *strDir);
bool SelectPrevFile(uint8 *strFile);

#ifdef _ACTIVE_DIRL
static bool Fast_SelectPrev_8Layer(uint8 *strFile);
static bool ReadDir();
#endif

#pragma renamecode(MW_EH_FSEL_PREV)

/********************************************************************************
 * Description : PREV操作获取后缀名BUFFER的地址
 *
 * Arguments  :  None
 * Returns     :  the buffer of current file extend name
 * Notes       :     None
 *
 ********************************************************************************/
const uint8 *GetExtBuf_Prev(void)
{
    ClearWatchDog();

    if (fselType == FSEL_TYPE_MUSIC)
    {
        return ExtMusicPrev[0];
    }
    else if (fselType == FSEL_TYPE_VOICE)
    {
        return ExtVoicePrev[0];
    }
    else
    {
        return ExtAll1[0];
    }
}

/********************************************************************************
 * Description : PREV操作获取后缀名的个数，例如MUSIC支持WMA MP3 WAV一起DIR
 *
 * Arguments  :  None
 * Returns     :  the buffer of current file extend name
 * Notes       :     None
 *
 ********************************************************************************/

uint8 GetExtSize_Prev(void)
{
    if (fselType == FSEL_TYPE_MUSIC)
    {
        return sizeof(ExtMusicPrev) / 4;
    }
    else if (fselType == FSEL_TYPE_VOICE)
    {
        return sizeof(ExtVoicePrev) / 4;
    }
    else
    {
        return 0;
    }
    return 0;
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

    FS_Dir(strFile, GetExtBuf_Prev(), FS_DIR_FILE, GetExtSize_Prev());
    while(FS_Dir(strFile, GetExtBuf_Prev(), FS_DIR_FILE_CONT, GetExtSize_Prev()))
    {
        FileNum++;
    }

    FS_SetCurDir(ClusterNo);
    FS_SetCurDirEntry(DirEntry);
    FS_SetCurDir_layer(dir_buf);

    return FileNum;
}


/********************************************************************************
 * Description : select the next file and get the file name
 *向目录首方向获取上一个文件
 ********************************************************************************/
bool SelectPrevFile(uint8 *strFile)
{
    uint8 dir_type = FS_DIR_FILE_CONT;
    return FS_DirPrev(strFile, GetExtBuf_Prev(), dir_type, GetExtSize_Prev());
}

/********************************************************************************
 * Description : 选择上一个文件夹
 *
 * Arguments  : none
 *
 *
 * Returns     :
 *    TRUE:  success.
 *    FALSE: fail, detail info store in fselError.
 * Notes       : rewind to the last valid directory when fselDir == 0;
 *
 ********************************************************************************/
bool fselPrevDir(uint8 *strDir)
{
    return FS_DirPrev(strDir, IgnoreExtPrev, FS_DIR_DIR_CONT, 1);
}

/********************************************************************************
 * Description : 一般PREV操作进入到一个子目录， 就需要定位到最后的文件夹
 *
 * Arguments  : 	因为事先不知道总共几个目录，所以需要从头开始DIR
 strDir:output Dir name
 * Returns    :
 TRUE: Successful
 FALSE: fail.
 * Notes       :  None
 *
 ********************************************************************************/
bool SelLastDir(uint8 *strDir)
{
    if (fselDirLayer >= TOTALDIRLAYER)
    {
        return FALSE;
    }
    if (FALSE == FS_DirLast())
    {
        return FALSE;
    }
    return FS_DirPrev(strDir, IgnoreExtPrev, FS_DIR_DIR_CONT, 1);
}

/********************************************************************************
 *选择上一个文件
 ********************************************************************************/

bool SelectPrev_8Layer(uint8 *strFile)
{
    char strDir[12];
    fselError = 0;

    if (!SelectPrevFile(strFile))
    {
        //检查父目录.
ParentDiragainNext:
        ClearWatchDog();
        if (FALSE != FS_DirParPrev())
        {
            fselDirLayer--;
            if (FALSE != fselPrevDir(strDir)) //平级目录的前一个目录
            {
                goto CurrentDiragainNext;
                //平级过去保证LAYER不会大于7
            }
            else
            {
                //当前目录下有效的文件
                if (FALSE == FS_DirLast())
                {
                    return FALSE;
                }
                if (FALSE != SelectPrevFile(strFile))
                {
                    fselError = 0;
                    fselDirNoInDisk--;
                    return TRUE;
                }
                //  当前目录分支没有合适的文件,退回上级目录
                goto ParentDiragainNext;
            }
        }
        else
        {
            fselError = FSEL_ERR_OUTOF;
            return FALSE;
        }
        //寻找当前目录下的所有目录
CurrentDiragainNext:
        ClearWatchDog();
        FS_CD(FS_CD_SON);
        fselDirLayer++;

        if (FALSE != SelLastDir(strDir)) //向下查找目录
        {
            goto CurrentDiragainNext;
            //一直找到尽头
        }
        else
        {
            if (FALSE == FS_DirLast())
            {
                return FALSE;
            }
            if (FALSE != SelectPrevFile(strFile))
            {
                fselError = 0;
                fselDirNoInDisk--;
                return TRUE;
            }
            //  当前目录分支没有合适的文件,退回上级目录
            goto ParentDiragainNext;
        }
    }
    return TRUE;
}
/********************************************************************************
 * Description : Select the Prev file according to type & mode, then get the name of file.
 *
 * Arguments  :
 strFile: Dos 8.3 file name of selected file.
 * Returns     :
 TRUE: Successful.
 FALSE: Fail, detail information in fselError
 * Notes       :  None
 *
 ********************************************************************************/
bool fselGetPrev(uint8 *strFile)
{
    char temp[12];
    bool result;
    bool root_flag = 0;
    uint16 num;
    uint32 TempClusNo;

    ClearWatchDog();

    if(fselTotalInDisk == 0)
    {
        return FALSE;
    }

    switch (fselMode)
    {
    case FSEL_ALL_REPEATONE:
    {
        if (!FS_GetName(strFile, 0))
        {
            return FALSE;
        }
        return TRUE;
    }

    case FSEL_ALL_RANDOM:
    {
#ifdef _ACTIVE_SHUFFLE
        num = shuffle_prev_file();
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

    case FSEL_ALL_SEQUENCE:
    {
        if (FALSE != SelectPrev_8Layer(strFile))
        {
            fselFileNoInDisk--;
            if(fselFileNoInDisk == 0)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }

        }
        return FALSE;
    }


    case FSEL_ALL_REPEATALL:
    {
        if(dir_list_flag == 0)
        {
            result = SelectPrev_8Layer(strFile);
        }
#ifdef _ACTIVE_DIRL
        else if(dir_list_flag == 1)
        {
            result = Fast_SelectPrev_8Layer(strFile);
        }
#endif
        else
        {
            return FALSE;
        }

        if (FALSE != result)
        {
            fselFileNoInDisk--;
            if(fselFileNoInDisk == 0)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
        else   //需要从第一首切换到最后一首
        {
            if(dir_list_flag == 0)
            {
                fselDirLayer = 0;
                if(FALSE == FS_CD(":"))
                {
                    return FALSE;
                }
                while (FALSE != SelLastDir(temp))
                {
                    FS_CD(temp);
                    fselDirLayer++;
                    ClearWatchDog();
                    root_flag = 1;
                }
                fselDirNoInDisk = fselTotalDir + 1;
                if(root_flag == 1)
                {
                    fselDirNoInDisk--;
                }
            }
#ifdef _ACTIVE_DIRL
            else if(dir_list_flag == 1)
            {
                fselDirNoInDisk = fselTotalDir;
                ReadDir();
            }
#endif
            else
            {
                return FALSE;
            }

            if (FALSE == FS_DirLast())
            {
                return FALSE;
            }

            fselFileNoInDisk = fselTotalInDisk;

            if (FALSE != SelectPrev_8Layer(strFile))
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }

    case FSEL_DIR_REPEATALL:
    {
        TempClusNo = FS_GetCurDir();
        if(CurDirClusNo != TempClusNo)
        {
            CurDirClusNo = TempClusNo;
            CurDirFileNum = ScanCurDir_FileNum(strFile);
        }
        if(FS_DirPrev(strFile, GetExtBuf_Prev(), FS_DIR_FILE_CONT, GetExtSize_Prev()))
        {
            fselFileNoInDisk--;
        }
        else    //已经到当前目录第一首歌，返回目录尾开始
        {
            FS_DirLast();
            if(!FS_DirPrev(strFile, GetExtBuf_Prev(), FS_DIR_FILE_CONT, GetExtSize_Prev()))
            {
                return FALSE;
            }
            fselFileNoInDisk = fselFileNoInDisk + CurDirFileNum - 1;
        }
        return TRUE;
    }

    default:
        return FALSE;
    }

}

/********************************************************************************
 * Description : Select the last folder file
 *
 * 只需换文件夹就可以了
 ********************************************************************************/
bool fselGetPrevFolder (uint8 *strFile)
{
    uint16 num = 0, mode = fselMode;
    fselMode = FSEL_ALL_REPEATALL;

    while(1)
    {
        if (FALSE != FS_DirPrev(strFile, GetExtBuf_Prev(), FS_DIR_FILE_CONT, GetExtSize_Prev()))
        {
            fselFileNoInDisk--;
        }
        else
        {
            break;
        }
    }
    if(!fselGetPrev(strFile))   //切换到上一目录最后一个文件
    {
        fselMode = mode;
        return FALSE;
    }
    num = ScanCurDir_FileNum(strFile);
    FS_Dir(strFile, GetExtBuf_Prev(), FS_DIR_FILE, GetExtSize_Prev());
    fselFileNoInDisk = fselFileNoInDisk - num + 1;
    fselMode = mode;
    return TRUE;
}

#ifdef _ACTIVE_DIRL
bool Fast_SelectPrev_8Layer(uint8 *strFile)
{
    if (FALSE != SelectPrevFile(strFile))
    {
        return TRUE;
    }
    if(fselDirNoInDisk == 1)
    {
        return FALSE;
    }
    fselDirNoInDisk--;

    ReadDir();
    FS_DirLast();
    if (FALSE != SelectPrevFile(strFile))
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

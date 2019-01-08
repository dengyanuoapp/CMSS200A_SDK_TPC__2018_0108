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
extern uint16 fselFileNoInDisk;// �ļ���ȫ�̵����
extern uint16 fselTotalInDisk;// ȫ���ļ�����
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
 * Description : PREV������ȡ��׺��BUFFER�ĵ�ַ
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
 * Description : PREV������ȡ��׺���ĸ���������MUSIC֧��WMA MP3 WAVһ��DIR
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
 * Description : ��õ�ǰĿ¼��ĳ�������ļ�������
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
 *��Ŀ¼�׷����ȡ��һ���ļ�
 ********************************************************************************/
bool SelectPrevFile(uint8 *strFile)
{
    uint8 dir_type = FS_DIR_FILE_CONT;
    return FS_DirPrev(strFile, GetExtBuf_Prev(), dir_type, GetExtSize_Prev());
}

/********************************************************************************
 * Description : ѡ����һ���ļ���
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
 * Description : һ��PREV�������뵽һ����Ŀ¼�� ����Ҫ��λ�������ļ���
 *
 * Arguments  : 	��Ϊ���Ȳ�֪���ܹ�����Ŀ¼��������Ҫ��ͷ��ʼDIR
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
 *ѡ����һ���ļ�
 ********************************************************************************/

bool SelectPrev_8Layer(uint8 *strFile)
{
    char strDir[12];
    fselError = 0;

    if (!SelectPrevFile(strFile))
    {
        //��鸸Ŀ¼.
ParentDiragainNext:
        ClearWatchDog();
        if (FALSE != FS_DirParPrev())
        {
            fselDirLayer--;
            if (FALSE != fselPrevDir(strDir)) //ƽ��Ŀ¼��ǰһ��Ŀ¼
            {
                goto CurrentDiragainNext;
                //ƽ����ȥ��֤LAYER�������7
            }
            else
            {
                //��ǰĿ¼����Ч���ļ�
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
                //  ��ǰĿ¼��֧û�к��ʵ��ļ�,�˻��ϼ�Ŀ¼
                goto ParentDiragainNext;
            }
        }
        else
        {
            fselError = FSEL_ERR_OUTOF;
            return FALSE;
        }
        //Ѱ�ҵ�ǰĿ¼�µ�����Ŀ¼
CurrentDiragainNext:
        ClearWatchDog();
        FS_CD(FS_CD_SON);
        fselDirLayer++;

        if (FALSE != SelLastDir(strDir)) //���²���Ŀ¼
        {
            goto CurrentDiragainNext;
            //һֱ�ҵ���ͷ
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
            //  ��ǰĿ¼��֧û�к��ʵ��ļ�,�˻��ϼ�Ŀ¼
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
        else   //��Ҫ�ӵ�һ���л������һ��
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
        else    //�Ѿ�����ǰĿ¼��һ�׸裬����Ŀ¼β��ʼ
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
 * ֻ�軻�ļ��оͿ�����
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
    if(!fselGetPrev(strFile))   //�л�����һĿ¼���һ���ļ�
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

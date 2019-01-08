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
#include <irq.h>

#define RecSpare_addr 0x5a0

#pragma name(MW_EH_FSEL_API)

/*外部变量和函数*/
extern bool fselGetNext(uint8 *strFile);
extern bool fselGetPrev(uint8 *strFile);
extern bool GetFilebyItem(uint8 *filename, uint16 ItemNum);
extern bool fselUserPLEnt(file_location_t *location);
extern uint8 fselSetLocationC(file_location_t *location);
extern bool fselGetLocationC(file_location_t *location);
extern bool fselGetLocationC_Switch(file_location_t *location);
extern bool fselGetFileByNoInDiskC(char *strFile, const uint16 Num);
extern uint8 fselScanDiskInfo(file_location_t *location, uint8 fsel_mode);
extern bool fselGetNextFolder(uint8 *strFile);
extern bool fselGetPrevFolder(uint8 *strFile);
extern void get_total_num_from_list(file_location_t *location);
extern void get_total_num_from_disk(file_location_t *location);
extern bool fselGetLocationPL(file_location_t *location);
extern uint8 fselSetLocationPL(file_location_t *location);
extern bool UserPL_next(char *filename);
extern bool UserPL_prev(char *filename);
extern bool init_shuffle(uint16 num_total);

uint8 fselMode; //Mode of select, Sequence, repeat one ...
uint8 fselPLOrCommonDir; //playlist or common operation
uint8 fselType; //Type of select,  Music or voice.
uint8 fselError; // store error id;
int8 fselDiskLetter;
int8 fselDirLayer; //dir  layer
uint16 fselFileNoInDisk;// 文件在全盘的序号
uint16 fselTotalInDisk;// 全盘文件总数
bool select_file_begin_flag;
uint16 CurDirFileNum; //当前目录有效歌曲数量
uint32 CurDirClusNo; //当前目录起始簇号，用于当前目录循环模式
uint16 fselDirNoInDisk; //当前所在目录的序号
uint16 fselTotalDir;
uint8 dir_list_flag;

#ifdef FILE_PLAY_LIST
//for playlist
uint16 shuffle_plist_total;
uint8 far read_buf[512];
static const uint8 plistName[] = {"ASDKMM  LIB"};
#endif
const uint8 ExtMusic[][4] =
{ "MP3", "WMA", "WAV", "MP1", "MP2" };
const uint8 ExtVoice[][4] =
{ "WAV", "MP3" };
const uint8 ExtAll[][4] =
{ "*  " };

#ifdef _ACTIVE_USERPL
extern uint8 User_ItemTotal;
extern uint8 User_ItemCur;
extern uint8 support_fav;
#endif

#pragma renamecode(MW_EH_FSEL_API)

static uint8 GetExtSize(void)
{
    if (fselType == FSEL_TYPE_MUSIC)
    {
        return sizeof(ExtMusic) / 4;
    }
    else if (fselType == FSEL_TYPE_VOICE)
    {
        return sizeof(ExtVoice) / 4;
    }
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
        return ExtMusic[0];
    }
    else if (fselType == FSEL_TYPE_VOICE)
    {
        return ExtVoice[0];
    }
    else if (fselType == DIRECTORY_ALL)
    {
        return ExtAll[0];
    }
    else
    {
        return 0;
    }
}

#ifdef FILE_PLAY_LIST
void get_total_num_from_plist(file_location_t *location)
{
    handle temp_list_fp;
    //uint8 read_buf[512];
    FS_CD(":");
    temp_list_fp = FS_FOpen(plistName, FS_OPEN_NORMAL);
    if(temp_list_fp == NULL)
    {
        get_total_num_from_disk(location);
    }
    else
    {
        FS_FRead(read_buf, 1, temp_list_fp);
        memcpy(&shuffle_plist_total, &read_buf[16], 2);
        FS_FClose(temp_list_fp);
    }
}
#endif
/********************************************************************************
 * Description : Initialize the FSEL module
 *
 * Arguments  :
 type:   Select type,  is FSEL_TYPE_MUSIC or FSEL_TYPE_VOICE
 *            mode: Select mode, is FSEL_ALL_SEQUENCE or  FSEL_ALL_REPEATONE ...
 *            DirLevel: Directory deep level, 0 or 1 now.
 *
 * Returns     :
 *            TRUE: Successful,
 *            FALSE: Fail, detail information in fselError.
 * Notes       :Must call it before call other fsel Module functions
 *
 ********************************************************************************/
void fsel_init_val(uint8 type, uint8 mode)
{
    fselType = type;
    fselMode = mode;
    fselDirLayer = 0;
    fselFileNoInDisk = 0;
    fselDirNoInDisk = 0;
    CurDirClusNo = 0xfffffff0; //初始化文件不可能使用的簇号
    CurDirFileNum = 0; //当前目录循环播放时使用
    select_file_begin_flag = TRUE;
}
//uint8 fselScanDiskInfo(file_location_t *location)
uint8 fselScanDiskInfo(file_location_t *location, uint8 fsel_mode)
{
    //    uint8 fileNameTmp[12];
#ifdef _ACTIVE_DIRL
    //    uint8 dir_buf[74];
    //    uint16 TempfselDirNoInDisk = 0;
    //   uint16 VramPage = 0;
#endif

    dir_list_flag = 0;

    if (FALSE == FS_CD(":"))
    {
        return FALSE;
    }

#ifdef _ACTIVE_DIRL
    //    memset(Dirlocation, 0, 512);
    //    memset(dir_buf, 0, 74);
    if(support_fav == 0)
    {
        dir_list_flag = 1;
    }
#endif
    if (dir_list_flag == 0)
    {


#ifdef FILE_PLAY_LIST
        if(fsel_mode == FSEL_TYPE_PLAYLIST)
        {
            get_total_num_from_plist(location);
        }
        else
#endif
        {
            get_total_num_from_disk(location);
        }

    }
#ifdef _ACTIVE_DIRL
    else
    {
        get_total_num_from_list(location);

    }
#endif

    if (FALSE == FS_CD(":"))
    {
        return FALSE;
    }
    select_file_begin_flag = TRUE;

    return TRUE;

}
bool fselInit(uint8 type, uint8 mode, uint8 PlayListOrCommonDir, file_location_t *location, uint8 ScanFlag)
{
    uint8 fileNameTmp[12];
    uint16 fselDirNoInDisk_bak;
    uint32 *RecSpare = RecSpare_addr;

    fselDiskLetter = (int8) location->disk;

    if (ScanFlag == 1)
    {
        *RecSpare = 0xc00;
    }

    if (!DRV_ActiveDrive(fselDiskLetter, 1))
    {
        fselError = FSEL_ERR_DISK;
        return FALSE;
    }

    fsel_init_val(type, mode);

    if (FALSE == FS_CD(":"))
    {
        return FALSE;
    }
    if (FS_Dir(fileNameTmp, GetExtBuf(), FS_DIR_FILE, GetExtSize()))
    {
        fselDirNoInDisk = 1; //根目录有文件，目录序号先加1
    }
    else
    {
        fselDirNoInDisk = 0;
    }

    if (ScanFlag == 1)
    {
        fselTotalInDisk = 0;
        fselTotalDir = 0;
        fselDirNoInDisk_bak = fselDirNoInDisk;
        if (FALSE == fselScanDiskInfo(location, PlayListOrCommonDir)) //遍历全盘
        {
            return FALSE;
        }
        fselDirNoInDisk = fselDirNoInDisk_bak;
    }

    if (PlayListOrCommonDir == FSEL_TYPE_COMMONDIR)
    {
        fselPLOrCommonDir = FSEL_TYPE_COMMONDIR;
#ifdef _ACTIVE_SHUFFLE
        init_shuffle(fselTotalInDisk);
#endif
        //return TRUE;
    }
#ifdef _ACTIVE_USERPL
    else if (PlayListOrCommonDir == FSEL_TYPE_USERPL)
    {
        fselPLOrCommonDir = FSEL_TYPE_USERPL;
        fselUserPLEnt(location);
    }
#endif
#ifdef FILE_PLAY_LIST
    else if(PlayListOrCommonDir == FSEL_TYPE_PLAYLIST)
    {
        fselPLOrCommonDir = FSEL_TYPE_PLAYLIST;

#ifdef _ACTIVE_SHUFFLE
        init_shuffle(shuffle_plist_total);
#endif

    }
#endif
    else
    {
        return FALSE;
    }

    return TRUE;
}

/********************************************************************************
 * Description : Set the select mode.
 *
 * Arguments  :
 mode:  the new mode
 * Returns     :  Successful or Fail
 * Notes       : None
 *
 ********************************************************************************/
bool fselSetMode(uint8 mode)
{
    if (mode >= FSEL_DIR_END)
    {
        return FALSE;
    }
    fselMode = mode;
    return TRUE;
}

/********************************************************************************
 * Description : get the next file in playlist or common file system
 *
 * Arguments  :
 *             filename : output the next filename
 * Returns     :
 *            TRUE: Successful,
 *            FALSE: Fail, detail information in fselError.
 * Notes       :in common type, it call fselGetNext
 *
 ********************************************************************************/
bool fselGetNextFile(uint8 *filename)
{
    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR) //comon file system
    {
        return fselGetNext(filename);
    }
#ifdef _ACTIVE_USERPL
    else if (fselPLOrCommonDir == FSEL_TYPE_USERPL)
    {
        return UserPL_next(filename);
    }
#endif
    else
    {
        return FALSE;
    }
}

/********************************************************************************
 * Description : get the prev file in playlist or common file system
 *
 * Arguments  :
 *             filename : output the prev filename
 * Returns     :
 *            TRUE: Successful,
 *            FALSE: Fail, detail information in fselError.
 * Notes       :in common type, it call fselGetPrev
 *
 ********************************************************************************/
bool fselGetPrevFile(uint8 *filename)
{

    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR) //comon file system
    {
        return fselGetPrev(filename);
    }
#ifdef _ACTIVE_USERPL
    else if (fselPLOrCommonDir == FSEL_TYPE_USERPL)
    {
        return UserPL_prev(filename);
    }
#endif
    else
    {
        return FALSE;
    }
}
/********************************************************************************
 * Description : throgh the keeped values set the file selector parament
 *
 * Arguments  :location :the keeped vlaues
 *
 * Returns     :
 *            TRUE: Successful,
 *            FALSE: Fail, detail information in fselError.
 * Notes       :
 *
 ********************************************************************************/
uint8 fselSetLocation(file_location_t *location)
{
#ifdef _ACTIVE_USERPL
    handle fp_list;
#endif

    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR) //common file system
    {
        return fselSetLocationC(location);
    }

#ifdef FILE_PLAY_LIST
    if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST) //comond file system

    {
        return fselSetLocationC(location);
    }
#endif

#ifdef _ACTIVE_USERPL
    else if (fselPLOrCommonDir == FSEL_TYPE_USERPL)
    {

        if(!fselUserPLEnt(location))
        {
            return FALSE;
        }
        GetFilebyItem(location->filename, location->PL_Cur);
        fp_list = FS_FOpen(location->filename, FS_OPEN_NORMAL); //检查当前列表文件是否存在
        if(fp_list == NULL)
        {
            return FALSE;
        }
        FS_FClose(fp_list);
        return TRUE;
    }
#endif
    else
    {
        return FALSE;
    }
}

/********************************************************************************
 * Description : keep values of the file selector parament
 *
 * Arguments  :location :the keeped vlaues
 *
 * Returns     :
 *            TRUE: Successful,
 *            FALSE: Fail, detail information in fselError.
 * Notes       :
 *
 ********************************************************************************/
bool fselGetLocation(file_location_t *location)
{

    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR) //comond file system
    {
        return fselGetLocationC(location);
    }

#ifdef FILE_PLAY_LIST
    if (fselPLOrCommonDir == FSEL_TYPE_PLAYLIST) //comond file system

    {
        return fselGetLocationC(location);
    }
#endif

#ifdef _ACTIVE_USERPL
    else if (fselPLOrCommonDir == FSEL_TYPE_USERPL)
    {
        return fselGetLocationPL(location);
    }
#endif
    else
    {
        return FALSE;
    }
}

/********************************************************************************
 * Description : keep values of the file selector parament
 *
 * Arguments  :location :the keeped vlaues
 *
 * Returns     :
 *            TRUE: Successful,
 *            FALSE: Fail, detail information in fselError.
 * Notes       :
 *
 ********************************************************************************/
bool fselGetLocation_Switch(file_location_t *location)
{
    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR) //comond file system
    {

        return fselGetLocationC_Switch(location);
    }
#ifdef _ACTIVE_USERPL
    else if (fselPLOrCommonDir == FSEL_TYPE_USERPL)
    {
        return fselGetLocationPL(location);
    }
#endif
    else
    {
        return FALSE;
    }
}

/********************************************************************************
 * Description : Get the total of valid files according to select type and mode
 *
 * Arguments  : None
 * Returns     : Total of valid files
 * Notes       : None
 *
 ********************************************************************************/
uint16 fselGetTotal(void)
{
    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)
    {
        return fselTotalInDisk;
    }
#ifdef _ACTIVE_USERPL
    else if (fselPLOrCommonDir == FSEL_TYPE_USERPL)
    {
        return User_ItemTotal;
    }
#endif
    else
    {
        return 0;
    }
}

/********************************************************************************
 * Description : Get the No. of current selected file.
 *
 * Arguments  : None
 * Returns     : Number of current selected file.
 * Notes       : None
 *
 ********************************************************************************/
uint16 fselGetNo(void)
{
    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)
    {
        return fselFileNoInDisk;
    }
#ifdef _ACTIVE_USERPL
    else if (fselPLOrCommonDir == FSEL_TYPE_USERPL)
    {
        if(User_ItemCur)
            return User_ItemCur;
        else
            return 1;
    }
#endif
    else
    {
        return 0;
    }
}

/********************************************************************************
 * Description : Get the Error id.
 *
 * Arguments  : None
 * Returns     : Error id
 * Notes       :  None
 *
 ********************************************************************************/
int8 fselGetErrID(void)
{
    return fselError;
}

/********************************************************************************
 * Description : 数字点歌
 *
 * Arguments  : None
 * Returns     : Number of current selected file.
 * Notes       : None
 *
 ********************************************************************************/
bool fselGetFileByNoInDisk(uint8 *strFile, const uint16 Num)
{
    uint8 mode_bak, total = 0;
    bool result;
    if (Num == 0)
    {
        return FALSE;
    }
    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)
    {
        mode_bak = fselMode;
        fselMode = FSEL_ALL_REPEATALL; //调用fselGetNext会根据播放模式，必须先切换模式
        result = fselGetFileByNoInDiskC(strFile, Num);
        fselMode = mode_bak;
    }
#ifdef _ACTIVE_USERPL
    else if (fselPLOrCommonDir == FSEL_TYPE_USERPL)
    {
        GetFilebyItem(strFile, Num);
    }
#endif
    else
    {
        return FALSE;
    }
    return result;
}

/********************************************************************************
 * Description : 选择下一个文件的第一首歌
 *
 * Arguments  : None
 * Returns     : Number of current selected file.
 * Notes       : None
 *
 ********************************************************************************/
bool fselGetNextFolderFile(uint8 *filename)
{
    //uint8 strFile[12];
    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)
    {
        return fselGetNextFolder(filename);
    }
    else
    {
        return FALSE;
    }
}

/********************************************************************************
 * Description : 选择上一个文件的第一首歌
 *
 * Arguments  : None
 * Returns     : Number of current selected file.
 * Notes       : None
 *
 ********************************************************************************/
bool fselGetPrevFolderFile(uint8 *filename)
{
    //uint8 strFile[12];
    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)
    {
        return fselGetPrevFolder(filename);
    }
    else
    {
        return FALSE;
    }
}

/**************************************************
 返回值：获取当前选定的歌曲所在目录的序号，从1开始
 输出参数 strLName：获取当前所在目录名，strLName的第一个字节 0：目录名为ascii码  1：目录名为unicode码
 输入参数 LNanmeSize：要获取的长名Unicode字符数目
 ***************************************************/
uint16 fselGetDirNo(uint8 *strLName, uint8 LNameSize)
{
    uint32 CurClusNo, DirEntry;
    uint8 dir_buf[74];
    if (fselPLOrCommonDir == FSEL_TYPE_COMMONDIR)
    {
        if (fselDirLayer == 0)
        {
            *strLName = 0; //第一个字节0表示返回的目录名是ASCII编码
            memcpy(strLName + 1, "ROOT", 5);
        }
        else
        {
            FS_GetCurDir_layer(dir_buf);
            CurClusNo = FS_GetCurDir();//当前目录项在文件系统中的偏移
            DirEntry = FS_GetCurDirEntry();//文件项在当前目录项中的偏移

            FS_DirParNext();
            *(strLName + 1) = 0x20; //获取当前文件系统指针指向的目录名
            if (!FS_GetName(strLName + 1, (LNameSize - 3) / 2))
            {
                FS_GetName(strLName + 1, 0); //不存在长名则获取短名
                *strLName = 0; //只有短名，ascii编码
            }
            else
            {
                *strLName = 1; //存在长名，unicode编码
            }
            if (FALSE == FS_CD(":")) //避免处于最后的目录项时调用FS_SetCurDir出错
            {
                return FALSE;
            }
            FS_SetCurDir_layer(dir_buf);
            FS_SetCurDir(CurClusNo);
            FS_SetCurDirEntry(DirEntry);
        }
        return fselDirNoInDisk;
    }
    else
    {
        return FALSE;
    }
}

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


#pragma name(MW_EH_FSEL_PREV1)

extern uint8 fselMode; //Mode of select, Sequence, repeat one ...
extern uint8 fselPLOrCommonDir; //playlist or common operation
extern uint8 fselType; //Type of select,  Music or voice.
extern uint8 fselError; // store error id;
extern int8 fselDiskLetter;
extern int8 fselDirLayer; //dir  layer
extern uint16 fselFileNoInDisk;// 文件在全盘的序号
extern uint16 fselTotalInDisk;// 全盘文件总数
extern bool select_file_begin_flag;
extern uint16 fselDirNoInDisk;

extern uint16 GetFileTotalCur(void);
extern uint16 GetFileTotal(uint8 *strDir, bool bCheckValidDir);
extern uint8 GetExtSize_Prev(void);
extern bool SelectNext_8Layer(uint8 *strFile);

const int8 dirExtLocation[4] =
{ "*  " };

const int8 ExtMusicPrev1[][4] =
{ "MP3", "WMA", "WAV", "MP1", "MP2" };
const int8 ExtVoicePrev1[][4] =
{ "WAV" };
const int8 IgnoreExtPrev1[4] =
{ "*  " };
const int8 ExtAll11[][4] =
{ "*  " };

#pragma renamecode(MW_EH_FSEL_PREV1)

extern uint16 GetNumByLocation(file_location_t *location);

/********************************************************************************
 * Description : Set the selected location which is specified by location.
 *
 * Arguments  :
 *            location:     store drive name, path name, file name.
 * Returns     :
 *            TRUE:    Successful
 *            FALSE:    Fail, detail information in fselError
 * Notes       :  Should select path in the case of only file name is invalid
 *
 ********************************************************************************/
uint8 fselSetLocationC(file_location_t *location)
{
    char m_szTemp1[12];
    uint8 dir_buf[74];
    ClearWatchDog();

    /*加入驱动初始化代码，解决插卡拔出时调用接口死机的问题*/
    if (!DRV_ActiveDrive(location->disk, 1))
    {
        fselError = FSEL_ERR_DISK;
        return FALSE;
    }
    if(FALSE == FS_CD(":")) //避免处于最后的目录项时调用FS_SetCurDir出错
    {
        fselError = FSEL_ERR_SETDIR;
        return FALSE;
    }
    fselDiskLetter = location->disk;
    if(fselDiskLetter == 'U')
    {
        VMRead(dir_buf, VM_EXFAT_INFO_UH, 74);
    }
    else
    {
        VMRead(dir_buf, VM_EXFAT_INFO_CARD, 74);
    }
    FS_SetCurDir_layer(dir_buf);

    //已经设置好文件夹的路径了
    if (!FS_SetCurDir(location->ClusterNo))
    {
        fselError = FSEL_ERR_SETDIR;
        return FALSE;
    }
    if (FALSE == FS_SetCurDirEntry(location->DirEntry))
    {
        fselError = FSEL_ERR_SETDIR;
        return FALSE;
    }
    if (!FS_GetName(m_szTemp1, 0))
    {
        fselError = FSEL_ERR_SETFILE;
        return FALSE;
    }
    else
    {
        if (memcmp(m_szTemp1, location->filename, 11) != 0)
        {
            fselError = FSEL_ERR_SETFILE;
            return FALSE;
        }
    }
    fselDirNoInDisk = location->fselDirNoInDisk;
    fselFileNoInDisk = location->fselFileNoInDisk;
    if(fselFileNoInDisk == 0xffff)
    {
        GetNumByLocation(location);
        fselFileNoInDisk = fselFileNoInDisk + location->fselFileNo;
        FS_SetCurDirEntry(location->DirEntry);  //回到刚才的文件
    }

    select_file_begin_flag = FALSE;	//断点存在则不从第一首开始播
    return TRUE;
}

/********************************************************************************
 * Description : Get the selected location
 *
 * Arguments  :
 *            location:    the buffer to store location
 * Returns     :
 *            TRUE: Successful
 *            FALSE: Fail, detail information in fselError.
 * Notes       :  None
 *
 ********************************************************************************/

bool fselGetLocationC(file_location_t *location)
{
    ClearWatchDog();

    location->disk = fselDiskLetter;
    location->ClusterNo = FS_GetCurDir();//当前目录项在文件系统中的偏移
    location->DirEntry = FS_GetCurDirEntry();//文件项在当前目录项中的偏移

    if (!FS_GetName(location->filename, 0)) //获取8+3短名
    {
        return FALSE;
    }
    location->fselDirNoInDisk = fselDirNoInDisk;
    location->fselFileNoInDisk = fselFileNoInDisk;
    return TRUE;
}

bool fselGetLocationC_Switch(file_location_t *location)
{
    ClearWatchDog();

    location->disk = fselDiskLetter;
    location->ClusterNo = FS_GetCurDir();//当前目录项在文件系统中的偏移
    location->DirEntry = FS_GetCurDirEntry();//文件项在当前目录项中的偏移

    if (!FS_GetName(location->filename, 0)) //获取8+3短名
    {
        return FALSE;
    }
    location->fselDirNoInDisk = fselDirNoInDisk;
    location->fselFileNoInDisk = fselFileNoInDisk;
    return TRUE;
}
uint16 GetNumByLocation(file_location_t *location)
{
    uint8 fileNameTmp[12];

    fselFileNoInDisk = 0;
    if(FALSE == FS_CD(":"))
    {
        return FALSE;
    }

    while(FALSE != SelectNext_8Layer(fileNameTmp))
    {
        if(FS_GetCurDir() == location->ClusterNo)
        {
            break;
        }
        fselFileNoInDisk++;
    }

    return fselFileNoInDisk;

}


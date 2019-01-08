/*
********************************************************************************
*                       ACTOS
*                  file system include file
*
*                (c) Copyright 2002-2003, Actions Co,Ld.
*                        All Right Reserved
*
* File   : filesys.h
* By     : DONGZH
* Version: 1> v1.00     first version     03-11-3 13:54
********************************************************************************
*/
#ifndef _FILESYS_INCLUDE
#define _FILESYS_INCLUDE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//此处添加c语言和汇编语言都要使用的定义
#include "actapi.h"
#include "typeext.h"
#include "uddrv.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// file system type
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define FS_FAT12        0x00
#define FS_FAT16        0x01
#define FS_FAT32        0x02
#define FS_EXFAT        0x03
//FIXME: complete these defines

//input parameter
#define FS_DIR_FILE             0x80    //  表示（根据输入扩展名）从当前目录首DIR第一个文件
#define FS_DIR_DIR              0x81    //  表示（根据输入扩展名）从当前目录首DIR第一个目录
#define FS_DIR_FILE_CONT        0x00    //  表示（根据输入扩展名）DIR下一个文件
#define FS_DIR_DIR_CONT         0x01    //  表示（根据输入扩展名）DIR下一个文件
#define FS_DIR_DIR_FILE      0x82	    //   表示（根据输入扩展名)    获取当前目录下的所有文件(只获取扩展名中包含的文件)   
#define FS_DIR_ALL_FILE      0x83	   //   表示（根据输入扩展名)    获取当前目录下的所有文件(包括目录和扩展名中包含的文件)

//#define FS_MODE_RONLY         0   //  暂时没有使用
//#define FS_MODE_WONLY         1   //  暂时没有使用
//#define FS_MODE_RW            2   //  暂时没有使用

#define FS_OPEN_NORMAL          0x0   //  表示正常打开文件
#define FS_OPEN_NOSEEKBACK      0x1   //  表示表示打开文件时，设定文件读操作的参数，为1表示，不记录簇信息，不支持Seekback
#define FS_OPEN_LN_NORMAL       0x80  //  表示输入长文件名正常打开文件
#define FS_OPEN_LN_NOSEEKBACK   0x81  //  表示输入长文件名打开文件时，设定文件读操作的参数，为1表示，不记录簇信息，不支持Seekback

/* prepare file cluster link when open     2008-12-19 13:56  */
#define FS_OPEN_TIDYCLUS        0x2
#define FS_OPEN_LN_TIDYCLUS     0x82
#define FS_CD_ROOT              ":"   //  表示改变当前目录到根目录（当前FAT16支持）
#define FS_CD_PARENT            "\\"  //  表示改变当前目录到上一级父目录（当前FAT16不支持）
#define FS_CD_SON               "/"   //  表示改变当前目录到当前目录项指向的子目录（当前FAT16不支持）
#define FS_CD_BACK             "|" //表示改变当前的目录到上一级目录，并且定位到上一次的目录位置

#define FS_GET_SPACE_TOTAL      0x00  //  表示获取描述当前驱动器总空间的扇区数   
#define FS_GET_SPACE_FREE       0x01  //  表示获取描述当前驱动器剩余空间的扇区数

#define FS_BACKUP_RWINFO        0     //  备份当前的读或者写操作的状态    
#define FS_BACKUP_BUFFER        1     //  备份文件系统的TempBuf
#define FS_BACKUP_DIRINFO       2     //  备份当前Dir操作的状态  


#define FS_RESTORE_RWINFO       0     //  备份备份的读或者写操作的状态
#define FS_RESTORE_BUFFER       1     //  恢复备份的文件系统的TempBuf
#define FS_RESTORE_DIRINFO      2     //    备份备份的Dir操作的状态

#define FS_SEEK_FFROMSTART      0     //  从文件起始开始位置向前Seek到输入的相对偏移所指定的扇区
#define FS_SEEK_FFROMCUR        1     //  从文件当前读取位置向前Seek到输入的相对偏移所指定的扇区
#define FS_SEEK_BFROMCUR        2     //  从文件当前读取位置向后Seek到输入的相对偏移所指定的扇区


//ret
#define FS_INFO_RW_ERR          1   //   磁盘读写错误
#define FS_INFO_WRITE_PROTECT   2   //   磁盘写保护
#define FS_INFO_UNFORMATTED     3   //   磁盘未格式化
#define FS_INFO_OVEFLOW         4   //   文件操作超出文件边界，或目录操作超出目录边界
#define FS_INFO_NOT_EXIST       5   //   文件操作的目标文件，或目录操作的目录项
#define FS_INFO_SPACE_OUT       6   //   表示文件操作或目录操作没有磁盘空间，不能写数据或者扩展子目录
#define FS_INFO_DIR_OUT         7   //   文件操作根目录目录项满
#define FS_INFO_NOT_EMPTY       8   //   删除目录时返回，表示删除的目录为非空目录


#define ATTR_READ_ONLY          0x01      /* Indicates that writes to the file should fail. */
#define ATTR_HIDDEN             0x02      /* Indicates that normal directory listings should not show this file. */
#define ATTR_SYSTEM             0x04      /* Indicates that this is an operating system file. */
#define ATTR_ARCHIVE            0x20      /* This attribute supports backup utilities. */
#define FILESIZE_BORDERLINE     0x10      /* borderline of file size, it is multiple of 64K  */

#define FS_SDFFAPIEntry         0x2b00    /* for data exchange, SDCard fast driver entry  2008-9-28 14:29   */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//此处添加c独有的定义
#ifdef __C251__
//


//文件建立时间
typedef struct
{
    WORD year;
    uchar month;
    uchar day;
    uchar hour;
    uchar minute;
    uchar second;
} fs_crt_time_t;

//此处添加c独有的定义
//#ifdef __A251__

#define		 APINO_FS_INIT               0x00
#define		 APINO_FS_EXIT               0x01
#define		 APINO_FS_CD                 0x02
#define		 APINO_FS_GETCURDIR          0x03
#define		 APINO_FS_SETCURDIR          0x04
#define		 APINO_FS_GETCURDIRENTRY     0x05
#define		 APINO_FS_SETCURDIRENTRY     0x06
#define		 APINO_FS_DIR                0x07
#define		 APINO_FS_DIRPREV            0x08
#define		 APINO_FS_DIRLAST            0x09
#define		 APINO_FS_FSEEK              0x0a
#define		 APINO_FS_FREAD              0x0b
#define		 APINO_FS_FOPEN 	         0x0c
#define		 APINO_FS_FCLOSE             0x0d
#define		 APINO_FS_GETNAME            0x0e
#define		 APINO_FS_FOPENBYLN          0x0f
#define		 APINO_FS_GETFILELEN         0x10
#define		 APINO_FS_GETUNALIGNEDLEN    0x11
#define		 APINO_FS_FTELL              0x12
#define		 APINO_FS_GETINFO            0x13
#define		 APINO_FS_GETSPACE           0x14
#define		 APINO_FS_JUSTBEING          0x15
#define		 APINO_FS_GETCRTTIME         0x16
#define		 APINO_FS_DIRPARNEXT         0x17
#define		 APINO_FS_GETWRTTIME         0x18
#define		 APINO_FS_CDBYLN 	         0x19
#define      APINO_FS_FREMOVE			 0x1a
#define      APINO_FS_FWRITE			 0x1b
#define      APINO_FS_FCREATE			 0x1c
#define      APINO_FS_FCREATEBYLN        0x1d
#define      APINO_FS_FSETNAMEATTR       0x1e
#define      APINO_FS_MAKEDIRBYLN        0x1f
#define      APINO_FS_MAKEDIR            0x20
#define      APINO_FS_REMOVEDIR          0x21
#define      APINO_FS_MODUNALIGNEDLEN    0x22
#define      APINO_FS_FRENAME            0x23
#define      APINO_FS_BACKUP             0x24
#define      APINO_FS_RESTORE            0x25
#define      APINO_FS_GETCURDIR_LAYER    0x26
#define      APINO_FS_SETCURDIR_LAYER    0x27
#define      APINO_FS_SETMODE    		 0x28
#define      APINO_FS_FATTRALTER         0x29
#define      APINO_FS_DIRPARPREV		 0x2a
#define      APINO_FS_GETCHECKSUM		 0x2b
#define      APINO_FS_GetSecPerClus		 0x2c
//********************************
//nor_fs spec api num
#define      APINO_FS_NorErase		     0x2b


//--------------------------------------------------------------------------
//mini fs api num
#define		 APINO_MFS_INIT               0x00
#define		 APINO_MFS_EXIT               0x01
#define		 APINO_MFS_CD                 0x02
#define		 APINO_MFS_GETCURDIR          0x03
#define		 APINO_MFS_SETCURDIR          0x04
#define		 APINO_MFS_GETCURDIRENTRY     0x05
#define		 APINO_MFS_SETCURDIRENTRY     0x06
#define		 APINO_MFS_DIR                0x07
#define		 APINO_MFS_FREAD              0x08
#define		 APINO_MFS_FOPEN              0x09
#define		 APINO_MFS_FCLOSE             0x0a
#define		 APINO_MFS_GETNAME            0x0b
#define		 APINO_MFS_GETFILELEN 	      0x0c
#define		 APINO_MFS_GETUNALIGNEDLEN    0x0d
#define		 APINO_MFS_JUSTBEING          0x0e
#define		 APINO_MFS_SETMODE			  0x0f
#define		 APINO_MFS_GetSecPerClus	  0x10
/*  FS_Init()
 *  初始化当前驱动器文件系统参数，之后才可以开始文件系统功能调用
 */
bool   API_FS_Init(uint8 api_no);
#define FS_Init()  API_FS_Init(APINO_FS_INIT)
bool   API_MFS_Init(uint8 api_no);
#define MFS_Init()  API_MFS_Init(APINO_MFS_INIT)
/*
*********************************************************************************************************
*Function    ：initialize FS by drive letter         2008-9-19 14:45
*
*Description：used specially for data copying, after DRV_SActiveDrive() and before FS_Init().
*
*Arguments   : letter   ---- drive letter, such as C--Flash, H--Card, L--HideDisk.
*
*Rerurns    ：0            failed
*             1            successful
*
*Other        :
*********************************************************************************************************
*/
bool API_FS_DiskInit(uint8 api_no, uint8 letter);
#define FS_DiskInit(letter) API_FS_DiskInit(APINO_FS_DISKINIT,letter)
//-----------------------------------------------------------------------------------

/*  FS_Exit()
 *  结束并退出文件系统功能调用
 */
bool   API_FS_Exit(uint8 api_n);
#define FS_Exit() API_FS_Exit(APINO_FS_EXIT)
bool   API_MFS_Exit(uint8 api_n);
#define MFS_Exit() API_MFS_Exit(APINO_MFS_EXIT)
//-----------------------------------------------------------------------------------

/*  FS_CD()
 *描述:
    根据将用户输入将当前目录指向当前目录的子目录，父目录或直接返回根目录
    strDirName为用户输入8.3子目录名，(11个byte,大写,不足补20h)表示改变当前目录到该子目录
 *参数:
    uint8 *strName为8.3子目录名的buffer指针
    strName第一个字符为"\"即5ch时表示改变当前目录到上一级父目录
    strName第一个字符为":"即3ah时表示改变当前目录到根目录
    strName第一个字符为"/"即2fh时表示改变当前目录到当前目录项对应的子目录
 *返回:
    0,1
 */
bool   API_FS_CD(uint8 api_no, const uint8 *strName);
#define FS_CD(strName) API_FS_CD(APINO_FS_CD,strName)
bool   API_MFS_CD(uint8 api_no, const uint8 *strName);
#define MFS_CD(strName) API_MFS_CD(APINO_MFS_CD,strName)
//-----------------------------------------------------------------------------------

/*  FS_Dir()
 *描述:
    在当前目录下检索文件和子目录，通过输入参数选择检索子目录还是文件
    调用该函数，该函数从当前目录项指针开始取一个文件和子目录名返回
    a.调用FS_CD()或者FS_SetCurDir()打开一个目录然后再Dir，Dir操作的起始目录项指针设为00000000h
    b.调用FS_SetCurDirEntry()设定一个目录项，Dir操作的起始目录项指针设为FS_SetCurDirEntry()的输入值
 *参数:
    DirType用来输入控制DIR的对象,0表DIR文件,1表示DIR目录,当前版本的FAT16，DirType输0x80或0x81从目录首开始Dir
    uint8 *StrName用来输入存放输出短文件名的buffer指针
    uint8 *StrExtName用来输入扩展名信息,控制DIR操作，输入'*'表示dir所有类型文件
    uint8 ExtNameNum 表示StrExtName中扩展名的个数
 *返回:
    0,1
 */
bool   API_FS_Dir(uint8 api_no, uint8 *StrName, const uint8 *StrExtName, uint8 type, uint8 ExtNameNum);
#define FS_Dir(strName,StrExtName,type,ExtNameNum) API_FS_Dir(APINO_FS_DIR,strName,StrExtName,type,ExtNameNum)
bool   API_MFS_Dir(uint8 api_no, uint8 *StrName, const uint8 *StrExtName, uint8 type, uint8 ExtNameNum);
#define MFS_Dir(strName,StrExtName,type,ExtNameNum) API_MFS_Dir(APINO_MFS_DIR,strName,StrExtName,type,ExtNameNum)
//-----------------------------------------------------------------------------------

/*  FS_GetName()
 *描述:
    取长文件名,通过输入要求返回的Unicode字符数来决定取真名还是取短名，确定用户取真名的Unicode字符数目
    通过在文件名输出Buffer初始数据，来区别两种取文件名的方式
    一种是根据当前指向的目录项取对应的文件真名
    一种是根据文件存储用的短名在当前目录中先定位，然后再取文件真名
 *参数:
    uint8 CharNum为希望获取文件名包含的Unicode字符数，用户程序相应设定足够buffer空间
    1.) uint8 CharNum不为0表示取真名或长名
    2.) uint8 CharNum为0则表示从当前目录项直接获取文件8+3类型的短名，
    调用不需要做NameBuf的初始化
    文档中所有文件名和目录名均指大写的8+3字符数组格式,占用11个byte,不足补20h

    uint8*NameBuf为存放调用输出的文件真名（或长名）的buffer指针
    1.) 如果直接从当前目录项查找该目录项的真名，则将NameBuf的第一个字节初始化为20h即可
    2.) 如果需要根据短名查找Unicode真名，调用时需要在NameBuf初始化输入短名，短名可由DIR调用或者本调用获取
 *返回:
    取长名时返回实际函数输出的长名字符的个数（包括函数返回时添加的结束符0x0000）
 *说明:
    取长名时返回的字符数小于文件实际字符数时，程序在后面添加Unicode结束符，
    所以用户的设置CharNum为1时，buffer至少应该是4byte。2时则是6byte
 */
uint8   API_FS_GetName(uint8 api_no, uint8 *NameBuf, uint8 CharNum);
#define FS_GetName(NameBuf,CharNum) API_FS_GetName(APINO_FS_GETNAME,NameBuf,CharNum)
uint8   API_MFS_GetName(uint8 api_no, uint8 *NameBuf, uint8 CharNum);
#define MFS_GetName(NameBuf,CharNum) API_MFS_GetName(APINO_MFS_GETNAME,NameBuf,CharNum)
//-----------------------------------------------------------------------------------

/*  FS_GetCurDirEntry()
 *描述:
    取当前检索到的文件对应的目录项在当前目录中的偏移字节
    根据目录项相对于当前目录首的偏移，可以确定该目录项的位置，同时指向一个文件或者子目录
 *返回:
    long DirEntry
 */
long   API_FS_GetCurDirEntry(uint8 api_no);
#define FS_GetCurDirEntry() API_FS_GetCurDirEntry(APINO_FS_GETCURDIRENTRY)
long   API_MFS_GetCurDirEntry(uint8 api_no);
#define MFS_GetCurDirEntry() API_MFS_GetCurDirEntry(APINO_MFS_GETCURDIRENTRY)
//-----------------------------------------------------------------------------------

/*  FS_SetCurDirEntry()
 *描述:
    将目录项偏移作为参数输入，可以设定Dir操作的目录项,
    通过设定偏移设定目录项可以指向一个文件或者子目录,
    若指向文件就可以对文件直接擦作,
    若指向子目录，则可获取目录名然后CD进子目录
 *参数:
    long DirEntry为需设定的目录项相对目录的偏移
 *返回:
    0,1
 */
bool   API_FS_SetCurDirEntry(uint8 api_no, long DirEntry);
#define FS_SetCurDirEntry(DirEntry) API_FS_SetCurDirEntry(APINO_FS_SETCURDIRENTRY,DirEntry)
bool    API_MFS_SetCurDirEntry(uint8 api_no, long DirEntry);
#define MFS_SetCurDirEntry(DirEntry) API_MFS_SetCurDirEntry(APINO_MFS_SETCURDIRENTRY,DirEntry)
//-----------------------------------------------------------------------------------

/*  FS_GetCurDir()
 *描述:
    取当前目录的句柄，也就是目录起始的簇号
    通过文件所在目录地句柄和文件目录项句柄（在目录中的偏移）,可以确定一个文件的位置
 *参数:
 *返回:
    long DirStartCluster也就是目录起始的簇号
 */
long   API_FS_GetCurDir(uint8 api_no);
#define FS_GetCurDir() API_FS_GetCurDir(APINO_FS_GETCURDIR)
long   API_MFS_GetCurDir(uint8 api_no);
#define MFS_GetCurDir() API_MFS_GetCurDir(APINO_MFS_GETCURDIR)
//-----------------------------------------------------------------------------------

/*  FS_SetCurDir()
 *描述:
    根据用户程序输入的目录句柄设定当前目录
 *参数:
    long DirStartCluster也就是目录起始的簇号
 *返回:
    0,1
 */
bool   API_FS_SetCurDir(uint8 api_no, long DirStartCluster);
#define FS_SetCurDir(DirStartCluster) API_FS_SetCurDir(APINO_FS_SETCURDIR,DirStartCluster)
bool   API_MFS_SetCurDir(uint8 api_no, long DirStartCluster);
#define MFS_SetCurDir(DirStartCluster) API_MFS_SetCurDir(APINO_MFS_SETCURDIR,DirStartCluster)
//-----------------------------------------------------------------------------------
/*  FS_GetCurDir_layer(uint8 *dir_layer)
 *描述:
    获取当前文件系统的路径信息，然后存入于  dir_layer地址中。
 *参数:
    dir_layer：存入路径信息的指针地址
 *返回:
    0,1
 */
uint8  API_FS_GetCurDir_layer(uint8 api_no, uint8 *dir_layer);
#define FS_GetCurDir_layer(dir_layer) API_FS_GetCurDir_layer(APINO_FS_GETCURDIR_LAYER,dir_layer)
//-----------------------------------------------------------------------------------
/*  FS_SetCurDir_layer(uint8 *dir_layer)
 *描述:
    设置当前文件系统的路径信息。
 *参数:
    dir_layer：存入需要重新设置的路径信息的指针地址
 *返回:
    0,1
 */
uint8  API_FS_SetCurDir_layer(uint8 api_no, uint8 *dir_layer);
#define FS_SetCurDir_layer(dir_layer) API_FS_SetCurDir_layer(APINO_FS_SETCURDIR_LAYER,dir_layer)
//-----------------------------------------------------------------------------------
/*  FS_MakeDir()
 *描述:
    在当前目录下生成一个用户程序制定的目录名
    文档中所有文件名和目录名均指大写的8+3字符数组格式,占用11个byte,不足补20h
 *参数:
    const uint8 *strSubDirName,生成的目录的8.3名称
 *返回:
    0,1
 */
bool   API_FS_MakeDir(uint8 api_no, const uint8 *strSubDirName);
#define FS_MakeDir(strSubDirName) API_FS_MakeDir(APINO_FS_MAKEDIR,strSubDirName)
//-----------------------------------------------------------------------------------

/*  FS_RemoveDir()
 *描述:
    在当前目录下删除一个用户程序指定的目录名
 *参数:
    const uint8 *strSubDirName,需删除目录的8.3名称
 *返回:
    0,1
 */
bool   API_FS_RemoveDir(uint8 api_no, uint8 *strSubDirName);
#define FS_RemoveDir(strSubDirName) API_FS_RemoveDir(APINO_FS_REMOVEDIR,strSubDirName)
//-----------------------------------------------------------------------------------

/*  FS_FOpen()
 *描述:
    根据用户输入的文件名在当前目录中打开一个已存在的文件。
    1.打开文件的方式有两种，
    一种是当Dir检索到相应文件目录项时，可以选择直接打开文件，此时uint8* strFlieName输入0；
    一种就是可以获取该文件的8＋3短名，然后在当前目录下输入uint8* strFlieName打开文件
    也就是说打开相应文件通过路径有两条
        * 设定当前目录＋输入文件名调用
        * 设定当前目录＋设定当前目录项＋不输入文件名直接调用
    2.文档中所有文件名和目录名除特殊说明一般指大写的8+3字符数组格式，即短名格式,占用11个byte,不足补20h
    3.FS_FOpen()支持短名或者长名输入，*strFlieName指向的可以是短名或长名（特殊说明）

 *参数:
    uint8 *strFlieName为代开文件的文件名的输入指针
    uint8 OpenMode为0表示打开文件后读取时支持往后SEEK, 为1表示打开
    文件后读取时不支持往后SEEK
 *返回:
    handle fp 文件句柄
 */
handle API_FS_FOpen(uint8 api_no, const uint8 *strFlieName, uint8 Mode);
#define FS_FOpen(strFlieName,Mode) API_FS_FOpen(APINO_FS_FOPEN,strFlieName,Mode)
handle API_MFS_FOpen(uint8 api_no, const uint8 *strFlieName, uint8 Mode);
#define MFS_FOpen(strFlieName,Mode) API_MFS_FOpen(APINO_MFS_FOPEN,strFlieName,Mode)
//-----------------------------------------------------------------------------------

/*  FS_FOpenbyLN()
 *描述:
    根据用户输入的UNICODE文件名在当前目录中打开一个已存在的文件。

 *参数:
    UNstr 打开文件的文件名的输入指针
    Mode  0    打开文件后读取数据时支持往后SEEK
          1    表示打开文件后读取时不支持往后SEEK
    SNstr 输入指针用于返回该文件存储在目录中的短名

 *返回:
    handle fp 文件句柄
 */

handle API_FS_FOpenbyLN(uint8 api_no, uint8 *UNstr, uint8 Mode, uint8 *SNstr);
#define FS_FOpenbyLN(UNstr,Mode,SNstr) API_FS_FOpenbyLN(APINO_FS_FOPENBYLN,UNstr,Mode,SNstr)
//-----------------------------------------------------------------------------------

/*  FS_FCreate()
 *描述:
    根据用户输入的文件名创建一个文件目录项,用户可获得当前操作文件的句柄
    文档中所有文件名和目录名均指大写的8+3字符数组格式,占用11个byte,不足补20h
 *参数:
    uint8 *strFlieName为创建文件的文件名的输入指针
 *返回:
    handle fp 文件句柄
 */
handle API_FS_FCreate(uint8 api_no, const uint8 *strFileName);
#define FS_FCreate(strFileName) API_FS_FCreate(APINO_FS_FCREATE,strFileName)
//-----------------------------------------------------------------------------------

/*  FS_FClose()
 *描述
    关闭用户创建的文件,用户输入需要操作文件的句柄
 *参数
    handle fp 文件句柄
 *返回
    0,1
 */
bool   API_FS_FClose(uint8 api_no, handle hFile);
#define FS_FClose(hFile) API_FS_FClose(APINO_FS_FCLOSE,hFile)
bool   API_MFS_FClose(uint8 api_no, handle hFile);
#define MFS_FClose(hFile) API_MFS_FClose(APINO_MFS_FCLOSE,hFile)
//-----------------------------------------------------------------------------------

/*  FS_FRemove()
 *描述
    在当前目录下删除一个用户程序指定的目录名
    文档中所有文件名和目录名均指大写的8+3字符数组格式,占用11个byte,不足补20h
 *参数
    uint8 *strName删除目录的目录名指针输入
 *返回
    0,1
 */
bool   API_FS_FRemove(uint8 api_no, uint8 *strName);
#define FS_FRemove(strName) API_FS_FRemove(APINO_FS_FREMOVE,strName)
//-----------------------------------------------------------------------------------

/*  FS_GetFileLen()
 *描述
    取当前打开文件的文件的长度,以扇区为单位
 *参数
    handle fp 文件句柄
 *返回
    long    FileLength
 */
long   API_FS_GetFileLen(uint8 api_no, handle hFile);
#define FS_GetFileLen(hFile) API_FS_GetFileLen(APINO_FS_GETFILELEN,hFile)
long   API_MFS_GetFileLen(uint8 api_no, handle hFile);
#define MFS_GetFileLen(hFile) API_MFS_GetFileLen(APINO_MFS_GETFILELEN,hFile)
//-----------------------------------------------------------------------------------

/*  FS_GetUnalignedLen()
 *描述
 	获取当前打开文件最后一个非对齐扇区的字节数，如返回0代表此文件长度整扇区对齐
 *参数
    handle fp 文件句柄
 *返回
 	int UnalignedLength当前打开文件最后一个非对齐扇区的字节数，如返回0代表此文件长度整扇区对齐
 */
int    API_FS_GetUnalignedLen(uint8 api_no, handle hFile);
#define FS_GetUnalignedLen(hFile) API_FS_GetUnalignedLen(APINO_FS_GETUNALIGNEDLEN,hFile)
int    API_MFS_GetUnalignedLen(uint8 api_no, handle hFile);
#define MFS_GetUnalignedLen(hFile) API_MFS_GetUnalignedLen(APINO_MFS_GETUNALIGNEDLEN,hFile)
//-----------------------------------------------------------------------------------


/*  FS_GetCrtTime()
 *描述
    获取当前打开文件的创建时间
    时间输出格式：
            struct timebuf
                {
                    WORD year;
                    uchar month;
                    uchar day;
                    uchar hour;
                    uchar minute;
                    uchar second;
                }Crttimebuffer;
 *参数
    handle fp 文件句柄，时间输出buf指针
 *返回
    0，1
 */
bool     API_FS_GetCrtTime(uint8 api_no, fs_crt_time_t *Crttimebuffer, handle hFile);
#define FS_GetCrtTime(Crttimebuffer,hFile) API_FS_GetCrtTime(APINO_FS_GETCRTTIME,Crttimebuffer,hFile)
//-----------------------------------------------------------------------------------

bool     API_FS_GetWrtTime(fs_crt_time_t *Crttimebuffer, handle hFile);
#define FS_GetWrtTime(Crttimebuffer,hFile) API_FS_GetWrtTime(APINO_FS_GETWRTTIME,Crttimebuffer,hFile)
/*  FS_FSeek()
 *描述:
    根据数据在FAT中的组织形式，来查找定位已存在文件中的一个扇区。实现：
    a.根据相对文件首的偏移量，指向一个扇区，实现用户程序的顺序读和随机读
    b.实现从当前位置往前或往后偏移若干扇区，实现用户程序的顺序读和随机读
    c.根据相对文件首的偏移量，指向已存在文件的一个扇区，实现用户程序的对该扇区数据的修改
    配合Fread()支持用户程序顺序读数据，快进快退以及随机定位读数据，另外可以方便实现回写
    修改已经生成的文件
 *参数:
    long fileoffset对应SEEK偏移量
    uint8 seektype对应SEEK类型 0:从文件头开始往文件尾快进，1:从当前位置往文件尾快进
    						  2:从当前位置往文件头快退
    handle hFile对应文件操作句柄
 *返回:
    0,1
 */
bool   API_FS_FSeek(uint8 api_no, long fileoffset, uint8 seektype, handle hFile);
#define FS_FSeek(fileoffset,seektype,hFile) API_FS_FSeek(APINO_FS_FSEEK,fileoffset,seektype,hFile)
//-----------------------------------------------------------------------------------

/*  FS_FTell()
 *描述:
    取当前读写操作的指针，指针是指相对文件头的扇区偏移量。
    读数据时用户调用该函数记录AB点，配合FS_Seek和FS_FRead实现数据
    的AB读取.写数据时，支持用户程序修改已生成文件的一个扇区
 *参数:
    handle hFile对应文件操作句柄
 *返回:
    long fileoffset对应汇编调用中的bcde寄存器
 */
long   API_FS_FTell(uint8 api_no, handle hFile);
#define FS_FTell(hFile) API_FS_FTell(APINO_FS_FTELL,hFile)
//-----------------------------------------------------------------------------------

/*  FS_FRead()
 *描述
    从FS_FSeek()指向的扇区读取以扇区为单位的数据,也可以连续顺序读取.
 *参数
    uint8 *DataIObuffer 读操作数据输出buffer的指针
    uint8 DataSecNum操作的扇区数目
    handle hFile操作文件的句柄
 *返回
    0,1
 */
bool   API_FS_FRead(uint8 api_no, uint8 *DataIObuffer, uint8 DataSecNum, handle hFile);
#define FS_FRead(DataIObuffer,DataSecNum,hFile) API_FS_FRead(APINO_FS_FREAD,DataIObuffer,DataSecNum,hFile)
bool   API_MFS_FRead(uint8 api_no, uint8 *DataIObuffer, uint8 DataSecNum, handle hFile);
#define MFS_FRead(DataIObuffer,DataSecNum,hFile) API_MFS_FRead(APINO_MFS_FREAD,DataIObuffer,DataSecNum,hFile)
//-----------------------------------------------------------------------------------

/*  FS_Write()
 *描述
    用户程序调用该函数每次以扇区为单位将数据顺序写入已创建文件的簇链表示的物理空间中
 *参数
    uint8 *DataIObuffer 写操作数据输出buffer的指针
    uint8 DataSecNum操作的扇区数目
    handle hFile操作文件的句柄
 *返回
    0,1
 */
bool   API_FS_FWrite(uint8 api_no, const uint8 *DataIObuffer, uint8 DataSecNum, handle hFile);
#define FS_FWrite(DataIObuffer,DataSecNum,hFile) API_FS_FWrite(APINO_FS_FWRITE,DataIObuffer,DataSecNum,hFile)
//-----------------------------------------------------------------------------------

/*  FS_GetSpace()
 *描述
    获取u盘剩余空间，根据输入参数不同选择要求返回磁盘总的扇区数还是剩
    余扇区数
 *参数
    uint8 typechar 0表示调用将返回表示当前磁盘驱动器总空间的扇区数，
                   1表示返回当前磁盘驱动器剩余空间的扇区数
 *返回
    long diskspace
 */
long   API_FS_GetSpace(uint8 api_no, uint8 type);
#define FS_GetSpace(type) API_FS_GetSpace(APINO_FS_GETSPACE,type)
//-----------------------------------------------------------------------------------

/*  FS_GetInfo()
 *描述
    获取文件系统出错信息
 *参数
 *返回
    uint8 status：出错信息
        1   磁盘读写错误
        2   磁盘写保护
        3   磁盘未格式化
        4   文件操作超出文件边界,目录操作超出目录边界
        5   文件操作的目标文件,目录操作的目录项不存在
        6   表示文件操作时没有磁盘空间,不能写数据或者扩展目录
            表示目录操作时没有磁盘空间,不能扩展目录,新建子目录
        7   文件操作时根目录目录项满
        8   删除目录时返回,表示删除的目录非空
 */
uint8   API_FS_GetInfo(uint8 api_no);
#define FS_GetInfo() API_FS_GetInfo(APINO_FS_GETINFO)
//-----------------------------------------------------------------------------------

/*  FS_Backup()
 *描述
    备份当前文件操作的状态
 *参数
    uint8* pbuf为备份buffer的指针，buffer大小视备份内容而定
    uint8 type为0表示备份当前读或写状态信息，1表示备份文件系统内部数据buffer
    handle hFile
 *返回
    0,1
 */
bool   API_FS_Backup(uint8 api_no, const uint8 *pbuf, uint8 type, handle hFile);
#define FS_Backup(pbuf,type,hFile) API_FS_Backup(APINO_FS_BACKUP,pbuf,type,hFile)
//-----------------------------------------------------------------------------------

/*  FS_Restore
 *描述
    恢复所备份得文件操作状态
 *参数
    uint8 * pbuf为备份buffer的指针，buffer大小视备份内容而定
    uint8 type为0表示恢复当前读或写状态信息，1表示恢复文件系统内部数据buffer
    handle hFile
 *返回
    0,1
 */
bool   API_FS_Restore(uint8 api_no, uint8 *pbuf, uint8 type, handle hFile);
#define FS_Restore(pbuf,type,hFile) API_FS_Restore(APINO_FS_RESTORE,pbuf,type,hFile)
//-----------------------------------------------------------------------------------

/*  FS_MakeSN
 *描述
    根据输入的UNICODE以及当前磁盘的文件信息，转换出文件系统存入文件时选用的短名
 *参数
    uint8 *SFNstr为指向输出短名字符串的指针
    uint8 *UNstr为指向输入长名（unicode）字符串的指针
    uint8 encode为转换成短名时采用的本地语言的格式（GB,..）
 *返回
    0,1
 */
bool    API_FS_MakeSN(uint8 api_no, uint8 *UNstr, uint8 *SFNstr, uint8 encode);
#define FS_MakeSN(UNstr,SFNstr,encode) API_FS_MakeSN(APINO_FS_MAKESN,UNstr,SFNstr,encode)
//-----------------------------------------------------------------------------------



/*  FS_CDbyLN
 *描述
    根据输入的UNICODE长目录名，进入子目录
 *参数
    uint8 *UNstr为指向输入长目录名（unicode）字符串的指针
    uint8 *SFNstr为指向输出短名字符串的指针
 *返回
    0,1
 */
bool   API_FS_CDbyLN(uint8 api_no, uint8 *UNstr, uint8 *SNstr);
#define FS_CDbyLN(UNstr,SFNstr) API_FS_CDbyLN(APINO_FS_CDBYLN,UNstr,SFNstr)
//-----------------------------------------------------------------------------------


/*  FS_MakeDirbyLN
 *描述
    根据输入的UNICODE长目录名，创建子目录
 *参数
    uint8 *UNstr为指向输入长目录名（unicode）字符串的指针
    uint8 *SFNstr为指向输出短名字符串的指针
    uint8 encode为转换成短名时采用的本地语言的格式（GB,..）
 *返回
    0,1
 */
bool   API_FS_MakeDirbyLN(uint8 api_no, uint8 *UNstr, uint8 *SNstr, uint8 encode);
#define FS_MakeDirbyLN(UNstr,SFNstr,encode) API_FS_MakeDirbyLN(APINO_FS_MAKEDIRBYLN,UNstr,SFNstr,encode)
//-----------------------------------------------------------------------------------


/*  FS_FCreatebyLN
 *描述
    根据输入的UNICODE长文件名，创建文件
 *参数
    uint8 *UNstr为指向输入长目录名（unicode）字符串的指针
    uint8 *SFNstr为指向输出短名字符串的指针
    uint8 encode为转换成短名时采用的本地语言的格式（GB,..）
 *返回
    0,1
 */

handle API_FS_FCreatebyLN(uint8 api_no, uint8 *UNstr, uint8 *SNstr, uint8 encode);
#define FS_FCreatebyLN(UNstr,SFNstr,encode) API_FS_FCreatebyLN(APINO_FS_FCREATEBYLN,UNstr,SFNstr,encode)
//-----------------------------------------------------------------------------------


/*  FS_JustBeing
 *描述

 *参数
  InputName 存放文件或目录名的字符串的指针
            当用户检索文件或子目录是否存在时，初始化输入名称到该指针指向的字符串。
            初始化名称时，根据type对应的操作类型，输入短名或者长名
  OutName    当文件或目录存在，并且该用户输入的OutName非空时，程序向用
            户输出文件或目录的短名


  JustType  Type的高四位表示一大类操作，低四位表示小类操作的细分
            0x10    dir当前目录下是否存在StrName（短名）命名的文件
            0x11    dir当前目录下是否存在StrName（短名）命名的子目录
            0x12    dir当前目录下是否存在StrName（短名）命名的文件或子目录
            0x20    dir当前目录下是否存在StrName（长名）命名的文件
            0x21    dir当前目录下是否存在StrName（长名）命名的子目录
            0x22    dir当前目录下是否存在StrName（短名）命名的文件或子目录
 *返回
  fail 0,succese,1
  函数返回Success表示，磁盘中存在以输入的名称命名的文件或目录，返回fail表示不存在
 */
bool  API_FS_JustBeing (uint8 api_no, uint8 *InputName, uint8 *OutName, uint8 JustType);
#define FS_JustBeing(InputName, OutName,JustType) API_FS_JustBeing(APINO_FS_JUSTBEING,InputName, OutName,JustType)
bool  API_MFS_JustBeing (uint8 api_no, uint8 *InputName, uint8 *OutName, uint8 JustType);
#define MFS_JustBeing(InputName, OutName,JustType) API_MFS_JustBeing(APINO_MFS_JUSTBEING,InputName, OutName,JustType)
//-----------------------------------------------------------------------------------

/*
*********************************************************************************************************
*Function	：删除文件的某一部分信息	2007-1-22 18:46
*
*Description：该函数用于删除一个文件的某一部分（中间或者结尾，但不可以全部）信息
*
*Arguments	: StartSec	将被删除的开始扇区位置（相对于文件首的扇区偏移）
*			  EndSec	将被删除的结束扇区位置（相对于文件首的扇区偏移）
*			  hFile		当前文件操作块的句柄
*
*Rerurns	：0			failed
*			  1			successful
*
*Other		:
*********************************************************************************************************
*/
bool FS_FRemovePart (long StartSec, long EndSec, handle hFile);
/*
*********************************************************************************************************
*Function	：添加文件的某部分到另一个文件	2007-1-25 20:18
*
*Description：该函数用于添加文件的某部分（中间或者结尾，但不可以全部）到另一个文件
*
*Arguments	: InsertSec	文件1的插入点（相对于文件首的扇区偏移）
*			  StartSec	文件2的插入部分的起始扇区位置（相对于文件首的扇区偏移）
*			  EndSec	文件2的插入部分的结束扇区位置（相对于文件首的扇区偏移）
*			  hFile1	将被插入和修改的文件的句柄
*			  hFile2	要插入并将被删除（根据需要可能不立即删除，建议立即删除）的文件的句柄
*
*Rerurns	：0			failed
*			  1			successful
*
*Other		:
*********************************************************************************************************
*/
bool FS_FAddFile (long InsertSec, long StartSec, long EndSec, handle hFile1, handle hFile2);

/*
*********************************************************************************************************
*Function	：拷贝FAT1扇区到FAT2	2007-3-26 17:10
*
*Description：该函数用于拷贝FAT1扇区到FAT2
*
*Arguments	:
*
*Rerurns	：0			failed
*			  1			successful
*
*Other		:
*********************************************************************************************************
*/
bool FS_CopyFATSec (void);
/*
*********************************************************************************************************
*Function	：alter file attribute        2007-7-5 11:12
*
*Description：used just after FS_FOPen() or FS_FCreate()
*
*Arguments	: 	hFile-----file handle
*			mode ---- ATTR_READ_ONLY (0x01), indicates that writes to the file should fail.
*                       ATTR_HIDDEN (0x02), indicates that normal directory listings should not show this file.
*                       ATTR_SYSTEM (0x04), indicates that this is an operating system file.
*                       ATTR_ARCHIVE (0x20), supports backup utilities.
*
*
*Rerurns	：0			failed
*			  1			successful
*
*Other		:
*********************************************************************************************************
*/
bool API_FS_FAttrAlter (uint8 api_no, handle hFile, uint8 mode);
#define FS_FAttrAlter(hFile,mode) API_FS_FAttrAlter(APINO_FS_FATTRALTER,hFile,mode)
/*
*********************************************************************************************************
*Function	：rename a file        2007-9-17 16:07
*
*Description：Change the name of a file according to the new name from application.
*
*Arguments	: NewName ---- address of new short name (8 filename and 3 ext).
*
*
*Rerurns	：0			failed
*			  1			successful
*
*Other		: used just after FS_FOPen() or FS_FCreate()
*********************************************************************************************************
*/
bool API_FS_FRename (uint8 api_no, uint8 *NewName);
#define FS_FRename(NewName) API_FS_FRename(APINO_FS_FRENAME,NewName)
//-----------------------------------------------------------------------------------

/*  FS_ModUnalignedLen()
 *描述
    生成文件时，修改表示文件精确长度的文件字节数。
 *参数
    UnalignedLen    文件结尾扇区的字节数（1~512）
    hFile   当前写文件操作块的句柄

 *返回
  fail 0,succese,1
 */
bool  API_FS_ModUnalignedLen (uint8 api_no, int UnalignedLen , handle hFile );
#define FS_ModUnalignedLen(UnalignedLen,hFile) API_FS_ModUnalignedLen(APINO_FS_MODUNALIGNEDLEN,UnalignedLen,hFile)
//-----------------------------------------------------------------------------------


bool  FS_FFRead (word fileoffset, uint8 *databuf, handle hFile );
//-----------------------------------------------------------------------------------

/*  FS_GetFileStartClusNum()
 *描述
    取得文件的首簇号，需要在FS_FOPen和FS_FOpenbyLN函数后调用，输入的文件句柄是上述函数返回的文件句柄
 *参数
    hFile  文件句柄
    type   暂时未使用

 *返回
  fail 0,succese,1
 */
long  FS_GetFileStartClusNum(handle hFile, uint8 type);

/*
*********************************************************************************************************
*Function	：修改文件名属性
*
*Description：将文件名或扩展名设置为小写显示。
*
*Arguments	: NameAttr-0x00 文件名和扩展名为大写
*			          -0x08 文件名为小写,扩展名为大写
*					  -0x10 文件名为大写，扩展名为小写
*				      -0x18 文件名和扩展名都是小写
*
*
*Rerurns	：0			failed
*			  1			successful
*
*Other		: 只能在 FS_FOPen() 或者 FS_FCreate()后使用。
*********************************************************************************************************
*/
bool API_FS_FSetNameAttr (uint8 api_no, uint8 NameAttr);
#define FS_FSetNameAttr(NameAttr) API_FS_FSetNameAttr(APINO_FS_FSETNAMEATTR,NameAttr)
//-----------------------------------------------------------------------------------
/*  FS_DirLast()
 *描述
    定位到当前目录的最后。
 *参数
	void

 *返回
  fail 0,succese,1
 */
bool  API_FS_DirLast(uint8 api_no);
#define FS_DirLast() API_FS_DirLast(APINO_FS_DIRLAST)
//-----------------------------------------------------------------------------------
/*  FS_DirParNext()
 *描述
    将目录指针指向上一级目录中指向当前目录的目录项的下一个目录项，若当前指针指向的
    是根目录，那么这个函数将返回错误。
 *参数
	void
 *返回
  fail 0,succese,1
 */
bool  API_FS_DirParNext(uint8 api_no);
#define FS_DirParNext() API_FS_DirParNext(APINO_FS_DIRPARNEXT)
//-----------------------------------------------------------------------------------
/*  FS_DirParPrev()
 *描述
    将目录指针指向上一级目录中指向当前目录的目录项的上一个目录项，若当前指针指向的
    是根目录，那么这个函数将返回错误。
 *参数
	void
 *返回
  fail 0,succese,1
 */
bool  API_FS_DirParPrev(uint8 api_no);
#define FS_DirParPrev() API_FS_DirParPrev(APINO_FS_DIRPARPREV)
//-----------------------------------------------------------------------------------
/*  FS_DirPrev()
 *描述:
    在当前目录下检索文件和子目录，通过输入参数选择检索子目录还是文件
    调用该函数，该函数从当前目录项指针开始取一个文件和子目录名返回
    a.调用FS_DirLast后再调用FS_DirPrev将获取最后一个有效的目录项。
    b.调用FS_SetCurDirEntry()设定一个目录项，DirPrev将返回该目录项的前一个有效目录项。
    c.在普通情况下DirPrev返回的是当前目录项的前一个有效目录项。
 *参数:
    Type用来输入控制DIR的对象,0表DIR文件,1表示DIR目录。
    uint8 *StrName用来输入存放输出短文件名的buffer指针
    uint8 *StrExtName用来输入扩展名信息,控制DIR操作，输入'*'表示dir所有类型文件
    uint8	ExtNameNum 表示StrExtName中扩展名的个数
 *返回:
    0,1
 */
bool  API_FS_DirPrev(uint8 api_no, uint8 *StrName, const uint8 *StrExtName, uint8 Type, uint8	ExtNameNum);
#define FS_DirPrev(StrName,StrExtName,Type,ExtNameNum) API_FS_DirPrev(APINO_FS_DIRPREV,StrName,StrExtName,Type,ExtNameNum)

//-----------------------------------------------------------------------------------
/*  FS_GetCheckSum()
 *描述
    获取文件名校验和，只有在调用fs_open以后才有效，同时只对exfat有效，对于fat32返回0
 *参数
	void
 *返回

 */
//uint16 FS_GetCheckSum(void);

uint16 API_FS_GetCheckSum(uint8 api_no);
#define FS_GetCheckSum() API_FS_GetCheckSum(APINO_FS_GETCHECKSUM)

//-----------------------------------------------------------------------------------
/*  FS_SetMode()
 *描述
    调序uhost读取方式
 *参数
	void
 *返回
  fail 0,succese,1
 */
//bool FS_CreateVolume(void);

void API_FS_SetMode(uint8 cmd, uint8 mode);
#define FS_SetMode(mode) API_FS_SetMode(APINO_FS_SETMODE,mode)
void API_MFS_SetMode(uint8 cmd, uint8 mode);
#define MFS_SetMode(mode) API_MFS_SetMode(APINO_MFS_SETMODE,mode)

uint16 API_FS_GetSecPerClus(uint8 cmd);
#define	FS_GetSecPerClus() API_FS_GetSecPerClus(APINO_FS_GetSecPerClus)
uint16 API_MFS_GetSecPerClus(uint8 cmd);
#define	MFS_GetSecPerClus() API_MFS_GetSecPerClus(APINO_MFS_GetSecPerClus)



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*  FS_NorErase()
 *描述
    调文件系统获取nor介质的擦除空间，只在装载nor_fs时，接口才有效
 *参数
	sec_count 擦除的扇区个数
 *返回
  fail 0,succese,1
 */

bool API_FS_NorErase(uint8 cmd, uint16 sec_count);
#define FS_NorErase(sec_count) API_FS_NorErase(APINO_FS_NorErase,sec_count)


#endif /*__C251__ */

//#endif /*__A251__ */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif  /* _FILESYS_INCLUDE */


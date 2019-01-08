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
#ifndef _MFS_INCLUDE
#define _MFS_INCLUDE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//此处添加c语言和汇编语言都要使用的定义
#include "actapi.h"
#include "typeext.h"
#include "uddrv.h"

/*
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// file system type
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define FS_FAT12        0x00
#define FS_FAT16        0x01
#define FS_FAT32        0x02

//FIXME: complete these defines

//input parameter
#define FS_DIR_FILE             0x80    //  表示（根据输入扩展名）从当前目录首DIR第一个文件
#define FS_DIR_DIR              0x81    //  表示（根据输入扩展名）从当前目录首DIR第一个目录
#define FS_DIR_FILE_CONT        0x00    //  表示（根据输入扩展名）DIR下一个文件
#define FS_DIR_DIR_CONT         0x01    //  表示（根据输入扩展名）DIR下一个文件

//#define FS_MODE_RONLY         0   //  暂时没有使用
//#define FS_MODE_WONLY         1   //  暂时没有使用
//#define FS_MODE_RW            2   //  暂时没有使用

#define FS_OPEN_NORMAL          0x0   //  表示正常打开文件
#define FS_OPEN_NOSEEKBACK      0x1   //  表示表示打开文件时，设定文件读操作的参数，为1表示，不记录簇信息，不支持Seekback
#define FS_OPEN_LN_NORMAL       0x80  //  表示输入长文件名正常打开文件
#define FS_OPEN_LN_NOSEEKBACK   0x81  //  表示输入长文件名打开文件时，设定文件读操作的参数，为1表示，不记录簇信息，不支持Seekback

#define FS_CD_ROOT              ":"   //  表示改变当前目录到根目录（当前FAT16支持）
#define FS_CD_PARENT            "\\"  //  表示改变当前目录到上一级父目录（当前FAT16不支持）
#define FS_CD_SON               "/"   //  表示改变当前目录到当前目录项指向的子目录（当前FAT16不支持）

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

*/
#define FS_SDFFAPIEntry         0x2b00    /* for data exchange, SDCard fast driver entry  2008-9-28 14:29   */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//此处添加c独有的定义
#ifdef __C251__
//

/*
//文件建立时间
typedef struct
{
    WORD year;
    uchar month;
    uchar day;
    uchar hour;
    uchar minute;
    uchar second;
}fs_crt_time_t;

*/

//-----------------------------------------------------------------------------------

/*  MFS_Init()
 *  初始化当前驱动器文件系统参数，之后才可以开始文件系统功能调用
 */
bool   MFS_Init(void);

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
bool MFS_DiskInit(char letter);

//-----------------------------------------------------------------------------------

/*  MFS_Exit()
 *  结束并退出文件系统功能调用
 */
bool MFS_Exit(void);

//-----------------------------------------------------------------------------------

/*  MFS_CD()
 *描述:
    根据将用户输入将当前目录指向当前目录的子目录，父目录或直接返回根目录
    strDirName为用户输入8.3子目录名，(11个byte,大写,不足补20h)表示改变当前目录到该子目录
 *参数:
    char *strName为8.3子目录名的buffer指针
    strName第一个字符为"\"即5ch时表示改变当前目录到上一级父目录
    strName第一个字符为":"即3ah时表示改变当前目录到根目录
    strName第一个字符为"/"即2fh时表示改变当前目录到当前目录项对应的子目录
 *返回:
    0,1
 */
bool   MFS_CD(const char *strName);

//-----------------------------------------------------------------------------------

/*  MFS_Dir()
 *描述:
    在当前目录下检索文件和子目录，通过输入参数选择检索子目录还是文件
    调用该函数，该函数从当前目录项指针开始取一个文件和子目录名返回
    a.调用FS_CD()或者FS_SetCurDir()打开一个目录然后再Dir，Dir操作的起始目录项指针设为00000000h
    b.调用FS_SetCurDirEntry()设定一个目录项，Dir操作的起始目录项指针设为FS_SetCurDirEntry()的输入值
 *参数:
    DirType用来输入控制DIR的对象,0表DIR文件,1表示DIR目录,当前版本的FAT16，DirType输0x80或0x81从目录首开始Dir
    char *StrName用来输入存放输出短文件名的buffer指针
    char *StrExtName用来输入扩展名信息,控制DIR操作，输入'*'表示dir所有类型文件
    char ExtNameNum 表示StrExtName中扩展名的个数
 *返回:
    0,1
 */
bool   MFS_Dir(char *StrName, const char *StrExtName, char type, char ExtNameNum);




//-----------------------------------------------------------------------------------

/*  MFS_GetCurDirEntry()
 *描述:
    取当前检索到的文件对应的目录项在当前目录中的偏移字节
    根据目录项相对于当前目录首的偏移，可以确定该目录项的位置，同时指向一个文件或者子目录
 *返回:
    long DirEntry
 */
long   MFS_GetCurDirEntry(void);


//-----------------------------------------------------------------------------------

/*  MFS_SetCurDirEntry()
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
bool   MFS_SetCurDirEntry(long DirEntry);


//-----------------------------------------------------------------------------------

/*  MFS_GetCurDir()
 *描述:
    取当前目录的句柄，也就是目录起始的簇号
    通过文件所在目录地句柄和文件目录项句柄（在目录中的偏移）,可以确定一个文件的位置
 *参数:
 *返回:
    long DirStartCluster也就是目录起始的簇号
 */
long   MFS_GetCurDir(void);


//-----------------------------------------------------------------------------------

/*  MFS_SetCurDir()
 *描述:
    根据用户程序输入的目录句柄设定当前目录
 *参数:
    long DirStartCluster也就是目录起始的簇号
 *返回:
    0,1
 */
bool   MFS_SetCurDir(long DirStartCluster);



//-----------------------------------------------------------------------------------

/*  MFS_FOpen()
 *描述:
    根据用户输入的文件名在当前目录中打开一个已存在的文件。
    1.打开文件的方式有两种，
    一种是当Dir检索到相应文件目录项时，可以选择直接打开文件，此时char* strFlieName输入0；
    一种就是可以获取该文件的8＋3短名，然后在当前目录下输入char* strFlieName打开文件
    也就是说打开相应文件通过路径有两条
        * 设定当前目录＋输入文件名调用
        * 设定当前目录＋设定当前目录项＋不输入文件名直接调用
    2.文档中所有文件名和目录名除特殊说明一般指大写的8+3字符数组格式，即短名格式,占用11个byte,不足补20h
    3.FS_FOpen()支持短名或者长名输入，*strFlieName指向的可以是短名或长名（特殊说明）

 *参数:
    char *strFlieName为代开文件的文件名的输入指针
    char OpenMode为0表示打开文件后读取时支持往后SEEK, 为1表示打开
    文件后读取时不支持往后SEEK
 *返回:
    handle fp 文件句柄
 */
handle MFS_FOpen(const char *strFlieName, char Mode);

//-------------------------------------------------------------------------------------
/*  FS_FRead()
 *描述
    从FS_FSeek()指向的扇区读取以扇区为单位的数据,也可以连续顺序读取.
 *参数
    char *DataIObuffer 读操作数据输出buffer的指针
    char DataSecNum操作的扇区数目
    handle hFile操作文件的句柄
 *返回
    0,1
 */
bool   MFS_FRead(char *DataIObuffer, char DataSecNum, handle hFile);

//-------------------------------------------------------------------------------------

/*  MFS_FClose()
 *描述
    关闭用户创建的文件,用户输入需要操作文件的句柄
 *参数
    handle fp 文件句柄
 *返回
    0,1
 */
bool   MFS_FClose(handle hFile);


//-----------------------------------------------------------------------------------

/*  MFS_GetFileLen()
 *描述
    取当前打开文件的文件的长度,以扇区为单位
 *参数
    handle fp 文件句柄
 *返回
    long    FileLength
 */
long   MFS_GetFileLen(handle hFile);


//-----------------------------------------------------------------------------------

/*  MFS_GetUnalignedLen()
 *描述
    获取当前打开文件最后一个扇区数据的字节数
 *参数
    handle fp 文件句柄
 *返回
    int UnalignedLength当前打开文件最后一个扇区数据的字节数
 */
int    MFS_GetUnalignedLen(handle hFile);


//-----------------------------------------------------------------------------------

/*  MFS_GetInfo()
 *描述
    获取文件系统出错信息
 *参数
 *返回
    char status：出错信息
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
char   MFS_GetInfo(void);

//-----------------------------------------------------------------------------------


/*  MFS_JustBeing
 *描述

 *参数
  InputName 存放文件或目录名的字符串的指针
            当用户检索文件或子目录是否存在时，初始化输入名称到该指针指向的字符串。
            初始化名称时，根据type对应的操作类型，输入短名或者长名
  OutName    当文件或目录存在，并且该用户输入的OutName非空时，程序向用
            户输出文件或目录的短名


  JustType  Type的高四位表示一大类操作，低四位表示小类操作的细分
            0x10    dir当前目录下是否存在StrName（短名）命名的文件
            0x11    dir当前目录下是否存在StrName（短名）命名的文件或子目录
            0x12    dir当前目录下是否存在StrName（短名）命名的文件或子目录
            0x20    dir当前目录下是否存在StrName（长名）命名的文件
            0x21    dir当前目录下是否存在StrName（长名）命名的子目录
            0x22    dir当前目录下是否存在StrName（短名）命名的文件或子目录
 *返回
  fail 0,succese,1
  函数返回Success表示，磁盘中存在以输入的名称命名的文件或目录，返回fail表示不存在
 */
bool  MFS_JustBeing (char *InputName, char *OutName, char JustType);


/*
*********************************************************************************************************
*Function    ：2008-9-24 18:02
*
*Description ：.
*
*Arguments   :
*描述:
    取长文件名,通过输入要求返回的UnIcdeo字符数来决定取真名还是取短名，确定用户取真名的UnIcdeo字符数目
    通过在文件名输出Buffer初始数据，来区别两种取文件名的方式
    一种是根据当前指向的目录项取对应的文件真名
    一种是根据文件存储用的短名在当前目录中先定位，然后在取文件真名
 *参数:
    char CharNum为希望获取文件名包含的Unicoe字符数，用户程序相应设定足够buffer空间
    1.) char CharNum不为0表示取真名或长名
    2.) char CharNum为0则表示从当前目录项直接获取文件8+3类型的短名，
    调用不需要做NameBuf的初始化
    文档中所有文件名和目录名均指大写的8+3字符数组格式,占用11个byte,不足补20h

    char*NameBuf为存放调用输出的文件真名（或长名）的buffer指针
    1.) 如果直接从当前目录项查找该目录项的真名，则将NameBuf的第一个字节初始化为20h即可
    2.) 如果需要根据短名查找Unicode真名，调用时需要在NameBuf初始化输入短名，短名可由DIR调用或者本调用获取

    NameAttr ---- address of attribute of filename, capital or small letter.

 *返回:
    取长名时返回实际函数输出的长名字符的个数（包括函数返回时添加的结束符0x0000）
 *说明:
    取长名时返回的字符数小于文件实际字符数时，程序在后面添加Unicode结束符，
    所以用户的设置CharNum为1时，buffer至少应该是4byte。2时则是6byte
*
*Rerurns    ：0            failed
*             1            successful
*
*Other        :
*********************************************************************************************************
*/
bool MFS_GetName(char *NameBuf, char CharNum);

//-----------------------------------------------------------------------------------

/*  MFS_GetFAttr()
 *描述:
	获取文件的属性
 *参数:

 *返回:
    文件属性
 */
char   MFS_GetFAttr();

#endif /*__IAR_SYSTEMS_ICC */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//此处添加c独有的定义
#ifdef __A251__

//常驻内存的函数
#define API_MFS_GetInfo                     0x00ff
#define API_MFS_GetFileLen                  0x01ff
#define	API_MFS_GetUnalignedLen				0x02ff
#define API_MFS_FOpen                       0x03ff
#define API_MFS_FRead                      	0x04ff
#define API_MFS_FClose                      0x05ff

#define BI_RC_GetInfo                       0x00
#define BI_RC_GetFileLen                    0x01
#define	BI_RC_GetUnalignedLen				0x02
#define BI_RC_FOpen                       	0x03
#define BI_RC_FRead                       	0x04
#define BI_RC_FClose                       	0x05


//第0个bank
#define API_MFS_Init                        0x0000
#define API_MFS_Exit                        0x0100
#define API_MFS_DiskInit                    0x0200
#define API_MFS_CD                    		0x0300
#define BI_B0_Init                          0x00
#define BI_B0_Exit                          0x01
#define BI_B0_DiskInit                      0x02
#define BI_B0_CD                      		0x03

//第1个bank
#define API_MFS_Dir                         0x0001
#define API_MFS_GetCurDir                   0x0101
#define API_MFS_GetCurDirEntry             	0x0201
#define API_MFS_SetCurDir                   0x0301
#define API_MFS_SetCurDirEntry             	0x0401
#define API_MFS_GetFAttr	             	0x0501
#define BI_B1_Dir                          	0x00
#define BI_B1_GetCurDir                     0x01
#define BI_B1_GetCurDirEntry                0x02
#define BI_B1_SetCurDir                		0x03
#define BI_B1_SetCurDirEntry                0x04
#define BI_B1_GetFAttr                		0x05

//第2个bank
#define API_MFS_JustBeing                   0x0002
#define API_MFS_GetName                     0x0102
#define BI_B2_JustBeing                   	0x00
#define BI_B2_GetName                     	0x01

/*
* 此处使用宏是为了在bank里使用时
*/
mMFS_Init    macro

mov		wr8, #API_MFS_Init
jmp     RSTFSAPI
endm

mMFS_DiskInit    macro

mov		wr8, #API_MFS_DiskInit
jmp     RSTFSAPI
endm

mMFS_Exit    macro
mov		wr8, #API_MFS_Exit
jmp     RSTFSAPI
endm

mMFS_CD      macro
mov		wr8, #API_MFS_CD
jmp     RSTFSAPI
endm


mMFS_Dir     macro
mov		wr8, #API_MFS_Dir
jmp     RSTFSAPI
endm

mMFS_JustBeing     macro
mov		wr8, #API_MFS_JustBeing
jmp     RSTFSAPI
endm


mMFS_SetCurDirEntry  macro
mov		wr8, #API_MFS_SetCurDirEntry
jmp     RSTFSAPI
endm

mMFS_GetFAttr  macro
mov		wr8, #API_MFS_GetFAttr
jmp     RSTFSAPI
endm

mMFS_GetCurDir   macro
mov		wr8, API_MFS_GetCurDir
jmp     RSTFSAPI
endm

mMFS_SetCurDir   macro
mov		wr8, API_MFS_SetCurDir
jmp     RSTFSAPI
endm


mMFS_FOpen       macro
mov		wr8, API_MFS_FOpen
jmp     RSTFSAPI
endm

mMFS_FClose      macro
mov		wr8, #API_MFS_FClose
jmp		RSTFSAPI
endm

mMFS_GetName         macro
mov		wr8, #API_MFS_GetName
jmp     RSTFSAPI
endm

mMFS_GetFileLen         macro
mov		wr8, #API_MFS_GetFileLen
jmp     RSTFSAPI
endm

mMFS_GetUnalignedLen         macro
mov		wr8, #API_MFS_GetUnalignedLen
jmp     RSTFSAPI
endm

mMFS_GetCurDirEntry         macro
mov		wr8, #API_MFS_GetCurDirEntry
jmp     RSTFSAPI
endm

mMFS_GetInfo     macro
mov     wr8, #API_MFS_GetInfo
jmp     RSTFSAPI
endm


#endif /*__IAR_SYSTEMS_ASM */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif  /* _FILESYS_INCLUDE */


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
//�˴����c���Ժͻ�����Զ�Ҫʹ�õĶ���
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
#define FS_DIR_FILE             0x80    //  ��ʾ������������չ�����ӵ�ǰĿ¼��DIR��һ���ļ�
#define FS_DIR_DIR              0x81    //  ��ʾ������������չ�����ӵ�ǰĿ¼��DIR��һ��Ŀ¼
#define FS_DIR_FILE_CONT        0x00    //  ��ʾ������������չ����DIR��һ���ļ�
#define FS_DIR_DIR_CONT         0x01    //  ��ʾ������������չ����DIR��һ���ļ�
#define FS_DIR_DIR_FILE      0x82	    //   ��ʾ������������չ��)    ��ȡ��ǰĿ¼�µ������ļ�(ֻ��ȡ��չ���а������ļ�)   
#define FS_DIR_ALL_FILE      0x83	   //   ��ʾ������������չ��)    ��ȡ��ǰĿ¼�µ������ļ�(����Ŀ¼����չ���а������ļ�)

//#define FS_MODE_RONLY         0   //  ��ʱû��ʹ��
//#define FS_MODE_WONLY         1   //  ��ʱû��ʹ��
//#define FS_MODE_RW            2   //  ��ʱû��ʹ��

#define FS_OPEN_NORMAL          0x0   //  ��ʾ�������ļ�
#define FS_OPEN_NOSEEKBACK      0x1   //  ��ʾ��ʾ���ļ�ʱ���趨�ļ��������Ĳ�����Ϊ1��ʾ������¼����Ϣ����֧��Seekback
#define FS_OPEN_LN_NORMAL       0x80  //  ��ʾ���볤�ļ����������ļ�
#define FS_OPEN_LN_NOSEEKBACK   0x81  //  ��ʾ���볤�ļ������ļ�ʱ���趨�ļ��������Ĳ�����Ϊ1��ʾ������¼����Ϣ����֧��Seekback

/* prepare file cluster link when open     2008-12-19 13:56  */
#define FS_OPEN_TIDYCLUS        0x2
#define FS_OPEN_LN_TIDYCLUS     0x82
#define FS_CD_ROOT              ":"   //  ��ʾ�ı䵱ǰĿ¼����Ŀ¼����ǰFAT16֧�֣�
#define FS_CD_PARENT            "\\"  //  ��ʾ�ı䵱ǰĿ¼����һ����Ŀ¼����ǰFAT16��֧�֣�
#define FS_CD_SON               "/"   //  ��ʾ�ı䵱ǰĿ¼����ǰĿ¼��ָ�����Ŀ¼����ǰFAT16��֧�֣�
#define FS_CD_BACK             "|" //��ʾ�ı䵱ǰ��Ŀ¼����һ��Ŀ¼�����Ҷ�λ����һ�ε�Ŀ¼λ��

#define FS_GET_SPACE_TOTAL      0x00  //  ��ʾ��ȡ������ǰ�������ܿռ��������   
#define FS_GET_SPACE_FREE       0x01  //  ��ʾ��ȡ������ǰ������ʣ��ռ��������

#define FS_BACKUP_RWINFO        0     //  ���ݵ�ǰ�Ķ�����д������״̬    
#define FS_BACKUP_BUFFER        1     //  �����ļ�ϵͳ��TempBuf
#define FS_BACKUP_DIRINFO       2     //  ���ݵ�ǰDir������״̬  


#define FS_RESTORE_RWINFO       0     //  ���ݱ��ݵĶ�����д������״̬
#define FS_RESTORE_BUFFER       1     //  �ָ����ݵ��ļ�ϵͳ��TempBuf
#define FS_RESTORE_DIRINFO      2     //    ���ݱ��ݵ�Dir������״̬

#define FS_SEEK_FFROMSTART      0     //  ���ļ���ʼ��ʼλ����ǰSeek����������ƫ����ָ��������
#define FS_SEEK_FFROMCUR        1     //  ���ļ���ǰ��ȡλ����ǰSeek����������ƫ����ָ��������
#define FS_SEEK_BFROMCUR        2     //  ���ļ���ǰ��ȡλ�����Seek����������ƫ����ָ��������


//ret
#define FS_INFO_RW_ERR          1   //   ���̶�д����
#define FS_INFO_WRITE_PROTECT   2   //   ����д����
#define FS_INFO_UNFORMATTED     3   //   ����δ��ʽ��
#define FS_INFO_OVEFLOW         4   //   �ļ����������ļ��߽磬��Ŀ¼��������Ŀ¼�߽�
#define FS_INFO_NOT_EXIST       5   //   �ļ�������Ŀ���ļ�����Ŀ¼������Ŀ¼��
#define FS_INFO_SPACE_OUT       6   //   ��ʾ�ļ�������Ŀ¼����û�д��̿ռ䣬����д���ݻ�����չ��Ŀ¼
#define FS_INFO_DIR_OUT         7   //   �ļ�������Ŀ¼Ŀ¼����
#define FS_INFO_NOT_EMPTY       8   //   ɾ��Ŀ¼ʱ���أ���ʾɾ����Ŀ¼Ϊ�ǿ�Ŀ¼


#define ATTR_READ_ONLY          0x01      /* Indicates that writes to the file should fail. */
#define ATTR_HIDDEN             0x02      /* Indicates that normal directory listings should not show this file. */
#define ATTR_SYSTEM             0x04      /* Indicates that this is an operating system file. */
#define ATTR_ARCHIVE            0x20      /* This attribute supports backup utilities. */
#define FILESIZE_BORDERLINE     0x10      /* borderline of file size, it is multiple of 64K  */

#define FS_SDFFAPIEntry         0x2b00    /* for data exchange, SDCard fast driver entry  2008-9-28 14:29   */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//�˴����c���еĶ���
#ifdef __C251__
//


//�ļ�����ʱ��
typedef struct
{
    WORD year;
    uchar month;
    uchar day;
    uchar hour;
    uchar minute;
    uchar second;
} fs_crt_time_t;

//�˴����c���еĶ���
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
 *  ��ʼ����ǰ�������ļ�ϵͳ������֮��ſ��Կ�ʼ�ļ�ϵͳ���ܵ���
 */
bool   API_FS_Init(uint8 api_no);
#define FS_Init()  API_FS_Init(APINO_FS_INIT)
bool   API_MFS_Init(uint8 api_no);
#define MFS_Init()  API_MFS_Init(APINO_MFS_INIT)
/*
*********************************************************************************************************
*Function    ��initialize FS by drive letter         2008-9-19 14:45
*
*Description��used specially for data copying, after DRV_SActiveDrive() and before FS_Init().
*
*Arguments   : letter   ---- drive letter, such as C--Flash, H--Card, L--HideDisk.
*
*Rerurns    ��0            failed
*             1            successful
*
*Other        :
*********************************************************************************************************
*/
bool API_FS_DiskInit(uint8 api_no, uint8 letter);
#define FS_DiskInit(letter) API_FS_DiskInit(APINO_FS_DISKINIT,letter)
//-----------------------------------------------------------------------------------

/*  FS_Exit()
 *  �������˳��ļ�ϵͳ���ܵ���
 */
bool   API_FS_Exit(uint8 api_n);
#define FS_Exit() API_FS_Exit(APINO_FS_EXIT)
bool   API_MFS_Exit(uint8 api_n);
#define MFS_Exit() API_MFS_Exit(APINO_MFS_EXIT)
//-----------------------------------------------------------------------------------

/*  FS_CD()
 *����:
    ���ݽ��û����뽫��ǰĿ¼ָ��ǰĿ¼����Ŀ¼����Ŀ¼��ֱ�ӷ��ظ�Ŀ¼
    strDirNameΪ�û�����8.3��Ŀ¼����(11��byte,��д,���㲹20h)��ʾ�ı䵱ǰĿ¼������Ŀ¼
 *����:
    uint8 *strNameΪ8.3��Ŀ¼����bufferָ��
    strName��һ���ַ�Ϊ"\"��5chʱ��ʾ�ı䵱ǰĿ¼����һ����Ŀ¼
    strName��һ���ַ�Ϊ":"��3ahʱ��ʾ�ı䵱ǰĿ¼����Ŀ¼
    strName��һ���ַ�Ϊ"/"��2fhʱ��ʾ�ı䵱ǰĿ¼����ǰĿ¼���Ӧ����Ŀ¼
 *����:
    0,1
 */
bool   API_FS_CD(uint8 api_no, const uint8 *strName);
#define FS_CD(strName) API_FS_CD(APINO_FS_CD,strName)
bool   API_MFS_CD(uint8 api_no, const uint8 *strName);
#define MFS_CD(strName) API_MFS_CD(APINO_MFS_CD,strName)
//-----------------------------------------------------------------------------------

/*  FS_Dir()
 *����:
    �ڵ�ǰĿ¼�¼����ļ�����Ŀ¼��ͨ���������ѡ�������Ŀ¼�����ļ�
    ���øú������ú����ӵ�ǰĿ¼��ָ�뿪ʼȡһ���ļ�����Ŀ¼������
    a.����FS_CD()����FS_SetCurDir()��һ��Ŀ¼Ȼ����Dir��Dir��������ʼĿ¼��ָ����Ϊ00000000h
    b.����FS_SetCurDirEntry()�趨һ��Ŀ¼�Dir��������ʼĿ¼��ָ����ΪFS_SetCurDirEntry()������ֵ
 *����:
    DirType�����������DIR�Ķ���,0��DIR�ļ�,1��ʾDIRĿ¼,��ǰ�汾��FAT16��DirType��0x80��0x81��Ŀ¼�׿�ʼDir
    uint8 *StrName����������������ļ�����bufferָ��
    uint8 *StrExtName����������չ����Ϣ,����DIR����������'*'��ʾdir���������ļ�
    uint8 ExtNameNum ��ʾStrExtName����չ���ĸ���
 *����:
    0,1
 */
bool   API_FS_Dir(uint8 api_no, uint8 *StrName, const uint8 *StrExtName, uint8 type, uint8 ExtNameNum);
#define FS_Dir(strName,StrExtName,type,ExtNameNum) API_FS_Dir(APINO_FS_DIR,strName,StrExtName,type,ExtNameNum)
bool   API_MFS_Dir(uint8 api_no, uint8 *StrName, const uint8 *StrExtName, uint8 type, uint8 ExtNameNum);
#define MFS_Dir(strName,StrExtName,type,ExtNameNum) API_MFS_Dir(APINO_MFS_DIR,strName,StrExtName,type,ExtNameNum)
//-----------------------------------------------------------------------------------

/*  FS_GetName()
 *����:
    ȡ���ļ���,ͨ������Ҫ�󷵻ص�Unicode�ַ���������ȡ��������ȡ������ȷ���û�ȡ������Unicode�ַ���Ŀ
    ͨ�����ļ������Buffer��ʼ���ݣ�����������ȡ�ļ����ķ�ʽ
    һ���Ǹ��ݵ�ǰָ���Ŀ¼��ȡ��Ӧ���ļ�����
    һ���Ǹ����ļ��洢�õĶ����ڵ�ǰĿ¼���ȶ�λ��Ȼ����ȡ�ļ�����
 *����:
    uint8 CharNumΪϣ����ȡ�ļ���������Unicode�ַ������û�������Ӧ�趨�㹻buffer�ռ�
    1.) uint8 CharNum��Ϊ0��ʾȡ��������
    2.) uint8 CharNumΪ0���ʾ�ӵ�ǰĿ¼��ֱ�ӻ�ȡ�ļ�8+3���͵Ķ�����
    ���ò���Ҫ��NameBuf�ĳ�ʼ��
    �ĵ��������ļ�����Ŀ¼����ָ��д��8+3�ַ������ʽ,ռ��11��byte,���㲹20h

    uint8*NameBufΪ��ŵ���������ļ���������������bufferָ��
    1.) ���ֱ�Ӵӵ�ǰĿ¼����Ҹ�Ŀ¼�����������NameBuf�ĵ�һ���ֽڳ�ʼ��Ϊ20h����
    2.) �����Ҫ���ݶ�������Unicode����������ʱ��Ҫ��NameBuf��ʼ�������������������DIR���û��߱����û�ȡ
 *����:
    ȡ����ʱ����ʵ�ʺ�������ĳ����ַ��ĸ�����������������ʱ��ӵĽ�����0x0000��
 *˵��:
    ȡ����ʱ���ص��ַ���С���ļ�ʵ���ַ���ʱ�������ں������Unicode��������
    �����û�������CharNumΪ1ʱ��buffer����Ӧ����4byte��2ʱ����6byte
 */
uint8   API_FS_GetName(uint8 api_no, uint8 *NameBuf, uint8 CharNum);
#define FS_GetName(NameBuf,CharNum) API_FS_GetName(APINO_FS_GETNAME,NameBuf,CharNum)
uint8   API_MFS_GetName(uint8 api_no, uint8 *NameBuf, uint8 CharNum);
#define MFS_GetName(NameBuf,CharNum) API_MFS_GetName(APINO_MFS_GETNAME,NameBuf,CharNum)
//-----------------------------------------------------------------------------------

/*  FS_GetCurDirEntry()
 *����:
    ȡ��ǰ���������ļ���Ӧ��Ŀ¼���ڵ�ǰĿ¼�е�ƫ���ֽ�
    ����Ŀ¼������ڵ�ǰĿ¼�׵�ƫ�ƣ�����ȷ����Ŀ¼���λ�ã�ͬʱָ��һ���ļ�������Ŀ¼
 *����:
    long DirEntry
 */
long   API_FS_GetCurDirEntry(uint8 api_no);
#define FS_GetCurDirEntry() API_FS_GetCurDirEntry(APINO_FS_GETCURDIRENTRY)
long   API_MFS_GetCurDirEntry(uint8 api_no);
#define MFS_GetCurDirEntry() API_MFS_GetCurDirEntry(APINO_MFS_GETCURDIRENTRY)
//-----------------------------------------------------------------------------------

/*  FS_SetCurDirEntry()
 *����:
    ��Ŀ¼��ƫ����Ϊ�������룬�����趨Dir������Ŀ¼��,
    ͨ���趨ƫ���趨Ŀ¼�����ָ��һ���ļ�������Ŀ¼,
    ��ָ���ļ��Ϳ��Զ��ļ�ֱ�Ӳ���,
    ��ָ����Ŀ¼����ɻ�ȡĿ¼��Ȼ��CD����Ŀ¼
 *����:
    long DirEntryΪ���趨��Ŀ¼�����Ŀ¼��ƫ��
 *����:
    0,1
 */
bool   API_FS_SetCurDirEntry(uint8 api_no, long DirEntry);
#define FS_SetCurDirEntry(DirEntry) API_FS_SetCurDirEntry(APINO_FS_SETCURDIRENTRY,DirEntry)
bool    API_MFS_SetCurDirEntry(uint8 api_no, long DirEntry);
#define MFS_SetCurDirEntry(DirEntry) API_MFS_SetCurDirEntry(APINO_MFS_SETCURDIRENTRY,DirEntry)
//-----------------------------------------------------------------------------------

/*  FS_GetCurDir()
 *����:
    ȡ��ǰĿ¼�ľ����Ҳ����Ŀ¼��ʼ�Ĵغ�
    ͨ���ļ�����Ŀ¼�ؾ�����ļ�Ŀ¼��������Ŀ¼�е�ƫ�ƣ�,����ȷ��һ���ļ���λ��
 *����:
 *����:
    long DirStartClusterҲ����Ŀ¼��ʼ�Ĵغ�
 */
long   API_FS_GetCurDir(uint8 api_no);
#define FS_GetCurDir() API_FS_GetCurDir(APINO_FS_GETCURDIR)
long   API_MFS_GetCurDir(uint8 api_no);
#define MFS_GetCurDir() API_MFS_GetCurDir(APINO_MFS_GETCURDIR)
//-----------------------------------------------------------------------------------

/*  FS_SetCurDir()
 *����:
    �����û����������Ŀ¼����趨��ǰĿ¼
 *����:
    long DirStartClusterҲ����Ŀ¼��ʼ�Ĵغ�
 *����:
    0,1
 */
bool   API_FS_SetCurDir(uint8 api_no, long DirStartCluster);
#define FS_SetCurDir(DirStartCluster) API_FS_SetCurDir(APINO_FS_SETCURDIR,DirStartCluster)
bool   API_MFS_SetCurDir(uint8 api_no, long DirStartCluster);
#define MFS_SetCurDir(DirStartCluster) API_MFS_SetCurDir(APINO_MFS_SETCURDIR,DirStartCluster)
//-----------------------------------------------------------------------------------
/*  FS_GetCurDir_layer(uint8 *dir_layer)
 *����:
    ��ȡ��ǰ�ļ�ϵͳ��·����Ϣ��Ȼ�������  dir_layer��ַ�С�
 *����:
    dir_layer������·����Ϣ��ָ���ַ
 *����:
    0,1
 */
uint8  API_FS_GetCurDir_layer(uint8 api_no, uint8 *dir_layer);
#define FS_GetCurDir_layer(dir_layer) API_FS_GetCurDir_layer(APINO_FS_GETCURDIR_LAYER,dir_layer)
//-----------------------------------------------------------------------------------
/*  FS_SetCurDir_layer(uint8 *dir_layer)
 *����:
    ���õ�ǰ�ļ�ϵͳ��·����Ϣ��
 *����:
    dir_layer��������Ҫ�������õ�·����Ϣ��ָ���ַ
 *����:
    0,1
 */
uint8  API_FS_SetCurDir_layer(uint8 api_no, uint8 *dir_layer);
#define FS_SetCurDir_layer(dir_layer) API_FS_SetCurDir_layer(APINO_FS_SETCURDIR_LAYER,dir_layer)
//-----------------------------------------------------------------------------------
/*  FS_MakeDir()
 *����:
    �ڵ�ǰĿ¼������һ���û������ƶ���Ŀ¼��
    �ĵ��������ļ�����Ŀ¼����ָ��д��8+3�ַ������ʽ,ռ��11��byte,���㲹20h
 *����:
    const uint8 *strSubDirName,���ɵ�Ŀ¼��8.3����
 *����:
    0,1
 */
bool   API_FS_MakeDir(uint8 api_no, const uint8 *strSubDirName);
#define FS_MakeDir(strSubDirName) API_FS_MakeDir(APINO_FS_MAKEDIR,strSubDirName)
//-----------------------------------------------------------------------------------

/*  FS_RemoveDir()
 *����:
    �ڵ�ǰĿ¼��ɾ��һ���û�����ָ����Ŀ¼��
 *����:
    const uint8 *strSubDirName,��ɾ��Ŀ¼��8.3����
 *����:
    0,1
 */
bool   API_FS_RemoveDir(uint8 api_no, uint8 *strSubDirName);
#define FS_RemoveDir(strSubDirName) API_FS_RemoveDir(APINO_FS_REMOVEDIR,strSubDirName)
//-----------------------------------------------------------------------------------

/*  FS_FOpen()
 *����:
    �����û�������ļ����ڵ�ǰĿ¼�д�һ���Ѵ��ڵ��ļ���
    1.���ļ��ķ�ʽ�����֣�
    һ���ǵ�Dir��������Ӧ�ļ�Ŀ¼��ʱ������ѡ��ֱ�Ӵ��ļ�����ʱuint8* strFlieName����0��
    һ�־��ǿ��Ի�ȡ���ļ���8��3������Ȼ���ڵ�ǰĿ¼������uint8* strFlieName���ļ�
    Ҳ����˵����Ӧ�ļ�ͨ��·��������
        * �趨��ǰĿ¼�������ļ�������
        * �趨��ǰĿ¼���趨��ǰĿ¼��������ļ���ֱ�ӵ���
    2.�ĵ��������ļ�����Ŀ¼��������˵��һ��ָ��д��8+3�ַ������ʽ����������ʽ,ռ��11��byte,���㲹20h
    3.FS_FOpen()֧�ֶ������߳������룬*strFlieNameָ��Ŀ����Ƕ�������������˵����

 *����:
    uint8 *strFlieNameΪ�����ļ����ļ���������ָ��
    uint8 OpenModeΪ0��ʾ���ļ����ȡʱ֧������SEEK, Ϊ1��ʾ��
    �ļ����ȡʱ��֧������SEEK
 *����:
    handle fp �ļ����
 */
handle API_FS_FOpen(uint8 api_no, const uint8 *strFlieName, uint8 Mode);
#define FS_FOpen(strFlieName,Mode) API_FS_FOpen(APINO_FS_FOPEN,strFlieName,Mode)
handle API_MFS_FOpen(uint8 api_no, const uint8 *strFlieName, uint8 Mode);
#define MFS_FOpen(strFlieName,Mode) API_MFS_FOpen(APINO_MFS_FOPEN,strFlieName,Mode)
//-----------------------------------------------------------------------------------

/*  FS_FOpenbyLN()
 *����:
    �����û������UNICODE�ļ����ڵ�ǰĿ¼�д�һ���Ѵ��ڵ��ļ���

 *����:
    UNstr ���ļ����ļ���������ָ��
    Mode  0    ���ļ����ȡ����ʱ֧������SEEK
          1    ��ʾ���ļ����ȡʱ��֧������SEEK
    SNstr ����ָ�����ڷ��ظ��ļ��洢��Ŀ¼�еĶ���

 *����:
    handle fp �ļ����
 */

handle API_FS_FOpenbyLN(uint8 api_no, uint8 *UNstr, uint8 Mode, uint8 *SNstr);
#define FS_FOpenbyLN(UNstr,Mode,SNstr) API_FS_FOpenbyLN(APINO_FS_FOPENBYLN,UNstr,Mode,SNstr)
//-----------------------------------------------------------------------------------

/*  FS_FCreate()
 *����:
    �����û�������ļ�������һ���ļ�Ŀ¼��,�û��ɻ�õ�ǰ�����ļ��ľ��
    �ĵ��������ļ�����Ŀ¼����ָ��д��8+3�ַ������ʽ,ռ��11��byte,���㲹20h
 *����:
    uint8 *strFlieNameΪ�����ļ����ļ���������ָ��
 *����:
    handle fp �ļ����
 */
handle API_FS_FCreate(uint8 api_no, const uint8 *strFileName);
#define FS_FCreate(strFileName) API_FS_FCreate(APINO_FS_FCREATE,strFileName)
//-----------------------------------------------------------------------------------

/*  FS_FClose()
 *����
    �ر��û��������ļ�,�û�������Ҫ�����ļ��ľ��
 *����
    handle fp �ļ����
 *����
    0,1
 */
bool   API_FS_FClose(uint8 api_no, handle hFile);
#define FS_FClose(hFile) API_FS_FClose(APINO_FS_FCLOSE,hFile)
bool   API_MFS_FClose(uint8 api_no, handle hFile);
#define MFS_FClose(hFile) API_MFS_FClose(APINO_MFS_FCLOSE,hFile)
//-----------------------------------------------------------------------------------

/*  FS_FRemove()
 *����
    �ڵ�ǰĿ¼��ɾ��һ���û�����ָ����Ŀ¼��
    �ĵ��������ļ�����Ŀ¼����ָ��д��8+3�ַ������ʽ,ռ��11��byte,���㲹20h
 *����
    uint8 *strNameɾ��Ŀ¼��Ŀ¼��ָ������
 *����
    0,1
 */
bool   API_FS_FRemove(uint8 api_no, uint8 *strName);
#define FS_FRemove(strName) API_FS_FRemove(APINO_FS_FREMOVE,strName)
//-----------------------------------------------------------------------------------

/*  FS_GetFileLen()
 *����
    ȡ��ǰ���ļ����ļ��ĳ���,������Ϊ��λ
 *����
    handle fp �ļ����
 *����
    long    FileLength
 */
long   API_FS_GetFileLen(uint8 api_no, handle hFile);
#define FS_GetFileLen(hFile) API_FS_GetFileLen(APINO_FS_GETFILELEN,hFile)
long   API_MFS_GetFileLen(uint8 api_no, handle hFile);
#define MFS_GetFileLen(hFile) API_MFS_GetFileLen(APINO_MFS_GETFILELEN,hFile)
//-----------------------------------------------------------------------------------

/*  FS_GetUnalignedLen()
 *����
 	��ȡ��ǰ���ļ����һ���Ƕ����������ֽ������緵��0������ļ���������������
 *����
    handle fp �ļ����
 *����
 	int UnalignedLength��ǰ���ļ����һ���Ƕ����������ֽ������緵��0������ļ���������������
 */
int    API_FS_GetUnalignedLen(uint8 api_no, handle hFile);
#define FS_GetUnalignedLen(hFile) API_FS_GetUnalignedLen(APINO_FS_GETUNALIGNEDLEN,hFile)
int    API_MFS_GetUnalignedLen(uint8 api_no, handle hFile);
#define MFS_GetUnalignedLen(hFile) API_MFS_GetUnalignedLen(APINO_MFS_GETUNALIGNEDLEN,hFile)
//-----------------------------------------------------------------------------------


/*  FS_GetCrtTime()
 *����
    ��ȡ��ǰ���ļ��Ĵ���ʱ��
    ʱ�������ʽ��
            struct timebuf
                {
                    WORD year;
                    uchar month;
                    uchar day;
                    uchar hour;
                    uchar minute;
                    uchar second;
                }Crttimebuffer;
 *����
    handle fp �ļ������ʱ�����bufָ��
 *����
    0��1
 */
bool     API_FS_GetCrtTime(uint8 api_no, fs_crt_time_t *Crttimebuffer, handle hFile);
#define FS_GetCrtTime(Crttimebuffer,hFile) API_FS_GetCrtTime(APINO_FS_GETCRTTIME,Crttimebuffer,hFile)
//-----------------------------------------------------------------------------------

bool     API_FS_GetWrtTime(fs_crt_time_t *Crttimebuffer, handle hFile);
#define FS_GetWrtTime(Crttimebuffer,hFile) API_FS_GetWrtTime(APINO_FS_GETWRTTIME,Crttimebuffer,hFile)
/*  FS_FSeek()
 *����:
    ����������FAT�е���֯��ʽ�������Ҷ�λ�Ѵ����ļ��е�һ��������ʵ�֣�
    a.��������ļ��׵�ƫ������ָ��һ��������ʵ���û������˳����������
    b.ʵ�ִӵ�ǰλ����ǰ������ƫ������������ʵ���û������˳����������
    c.��������ļ��׵�ƫ������ָ���Ѵ����ļ���һ��������ʵ���û�����ĶԸ��������ݵ��޸�
    ���Fread()֧���û�����˳������ݣ���������Լ������λ�����ݣ�������Է���ʵ�ֻ�д
    �޸��Ѿ����ɵ��ļ�
 *����:
    long fileoffset��ӦSEEKƫ����
    uint8 seektype��ӦSEEK���� 0:���ļ�ͷ��ʼ���ļ�β�����1:�ӵ�ǰλ�����ļ�β���
    						  2:�ӵ�ǰλ�����ļ�ͷ����
    handle hFile��Ӧ�ļ��������
 *����:
    0,1
 */
bool   API_FS_FSeek(uint8 api_no, long fileoffset, uint8 seektype, handle hFile);
#define FS_FSeek(fileoffset,seektype,hFile) API_FS_FSeek(APINO_FS_FSEEK,fileoffset,seektype,hFile)
//-----------------------------------------------------------------------------------

/*  FS_FTell()
 *����:
    ȡ��ǰ��д������ָ�룬ָ����ָ����ļ�ͷ������ƫ������
    ������ʱ�û����øú�����¼AB�㣬���FS_Seek��FS_FReadʵ������
    ��AB��ȡ.д����ʱ��֧���û������޸��������ļ���һ������
 *����:
    handle hFile��Ӧ�ļ��������
 *����:
    long fileoffset��Ӧ�������е�bcde�Ĵ���
 */
long   API_FS_FTell(uint8 api_no, handle hFile);
#define FS_FTell(hFile) API_FS_FTell(APINO_FS_FTELL,hFile)
//-----------------------------------------------------------------------------------

/*  FS_FRead()
 *����
    ��FS_FSeek()ָ���������ȡ������Ϊ��λ������,Ҳ��������˳���ȡ.
 *����
    uint8 *DataIObuffer �������������buffer��ָ��
    uint8 DataSecNum������������Ŀ
    handle hFile�����ļ��ľ��
 *����
    0,1
 */
bool   API_FS_FRead(uint8 api_no, uint8 *DataIObuffer, uint8 DataSecNum, handle hFile);
#define FS_FRead(DataIObuffer,DataSecNum,hFile) API_FS_FRead(APINO_FS_FREAD,DataIObuffer,DataSecNum,hFile)
bool   API_MFS_FRead(uint8 api_no, uint8 *DataIObuffer, uint8 DataSecNum, handle hFile);
#define MFS_FRead(DataIObuffer,DataSecNum,hFile) API_MFS_FRead(APINO_MFS_FREAD,DataIObuffer,DataSecNum,hFile)
//-----------------------------------------------------------------------------------

/*  FS_Write()
 *����
    �û�������øú���ÿ��������Ϊ��λ������˳��д���Ѵ����ļ��Ĵ�����ʾ������ռ���
 *����
    uint8 *DataIObuffer д�����������buffer��ָ��
    uint8 DataSecNum������������Ŀ
    handle hFile�����ļ��ľ��
 *����
    0,1
 */
bool   API_FS_FWrite(uint8 api_no, const uint8 *DataIObuffer, uint8 DataSecNum, handle hFile);
#define FS_FWrite(DataIObuffer,DataSecNum,hFile) API_FS_FWrite(APINO_FS_FWRITE,DataIObuffer,DataSecNum,hFile)
//-----------------------------------------------------------------------------------

/*  FS_GetSpace()
 *����
    ��ȡu��ʣ��ռ䣬�������������ͬѡ��Ҫ�󷵻ش����ܵ�����������ʣ
    ��������
 *����
    uint8 typechar 0��ʾ���ý����ر�ʾ��ǰ�����������ܿռ����������
                   1��ʾ���ص�ǰ����������ʣ��ռ��������
 *����
    long diskspace
 */
long   API_FS_GetSpace(uint8 api_no, uint8 type);
#define FS_GetSpace(type) API_FS_GetSpace(APINO_FS_GETSPACE,type)
//-----------------------------------------------------------------------------------

/*  FS_GetInfo()
 *����
    ��ȡ�ļ�ϵͳ������Ϣ
 *����
 *����
    uint8 status��������Ϣ
        1   ���̶�д����
        2   ����д����
        3   ����δ��ʽ��
        4   �ļ����������ļ��߽�,Ŀ¼��������Ŀ¼�߽�
        5   �ļ�������Ŀ���ļ�,Ŀ¼������Ŀ¼�����
        6   ��ʾ�ļ�����ʱû�д��̿ռ�,����д���ݻ�����չĿ¼
            ��ʾĿ¼����ʱû�д��̿ռ�,������չĿ¼,�½���Ŀ¼
        7   �ļ�����ʱ��Ŀ¼Ŀ¼����
        8   ɾ��Ŀ¼ʱ����,��ʾɾ����Ŀ¼�ǿ�
 */
uint8   API_FS_GetInfo(uint8 api_no);
#define FS_GetInfo() API_FS_GetInfo(APINO_FS_GETINFO)
//-----------------------------------------------------------------------------------

/*  FS_Backup()
 *����
    ���ݵ�ǰ�ļ�������״̬
 *����
    uint8* pbufΪ����buffer��ָ�룬buffer��С�ӱ������ݶ���
    uint8 typeΪ0��ʾ���ݵ�ǰ����д״̬��Ϣ��1��ʾ�����ļ�ϵͳ�ڲ�����buffer
    handle hFile
 *����
    0,1
 */
bool   API_FS_Backup(uint8 api_no, const uint8 *pbuf, uint8 type, handle hFile);
#define FS_Backup(pbuf,type,hFile) API_FS_Backup(APINO_FS_BACKUP,pbuf,type,hFile)
//-----------------------------------------------------------------------------------

/*  FS_Restore
 *����
    �ָ������ݵ��ļ�����״̬
 *����
    uint8 * pbufΪ����buffer��ָ�룬buffer��С�ӱ������ݶ���
    uint8 typeΪ0��ʾ�ָ���ǰ����д״̬��Ϣ��1��ʾ�ָ��ļ�ϵͳ�ڲ�����buffer
    handle hFile
 *����
    0,1
 */
bool   API_FS_Restore(uint8 api_no, uint8 *pbuf, uint8 type, handle hFile);
#define FS_Restore(pbuf,type,hFile) API_FS_Restore(APINO_FS_RESTORE,pbuf,type,hFile)
//-----------------------------------------------------------------------------------

/*  FS_MakeSN
 *����
    ���������UNICODE�Լ���ǰ���̵��ļ���Ϣ��ת�����ļ�ϵͳ�����ļ�ʱѡ�õĶ���
 *����
    uint8 *SFNstrΪָ����������ַ�����ָ��
    uint8 *UNstrΪָ�����볤����unicode���ַ�����ָ��
    uint8 encodeΪת���ɶ���ʱ���õı������Եĸ�ʽ��GB,..��
 *����
    0,1
 */
bool    API_FS_MakeSN(uint8 api_no, uint8 *UNstr, uint8 *SFNstr, uint8 encode);
#define FS_MakeSN(UNstr,SFNstr,encode) API_FS_MakeSN(APINO_FS_MAKESN,UNstr,SFNstr,encode)
//-----------------------------------------------------------------------------------



/*  FS_CDbyLN
 *����
    ���������UNICODE��Ŀ¼����������Ŀ¼
 *����
    uint8 *UNstrΪָ�����볤Ŀ¼����unicode���ַ�����ָ��
    uint8 *SFNstrΪָ����������ַ�����ָ��
 *����
    0,1
 */
bool   API_FS_CDbyLN(uint8 api_no, uint8 *UNstr, uint8 *SNstr);
#define FS_CDbyLN(UNstr,SFNstr) API_FS_CDbyLN(APINO_FS_CDBYLN,UNstr,SFNstr)
//-----------------------------------------------------------------------------------


/*  FS_MakeDirbyLN
 *����
    ���������UNICODE��Ŀ¼����������Ŀ¼
 *����
    uint8 *UNstrΪָ�����볤Ŀ¼����unicode���ַ�����ָ��
    uint8 *SFNstrΪָ����������ַ�����ָ��
    uint8 encodeΪת���ɶ���ʱ���õı������Եĸ�ʽ��GB,..��
 *����
    0,1
 */
bool   API_FS_MakeDirbyLN(uint8 api_no, uint8 *UNstr, uint8 *SNstr, uint8 encode);
#define FS_MakeDirbyLN(UNstr,SFNstr,encode) API_FS_MakeDirbyLN(APINO_FS_MAKEDIRBYLN,UNstr,SFNstr,encode)
//-----------------------------------------------------------------------------------


/*  FS_FCreatebyLN
 *����
    ���������UNICODE���ļ����������ļ�
 *����
    uint8 *UNstrΪָ�����볤Ŀ¼����unicode���ַ�����ָ��
    uint8 *SFNstrΪָ����������ַ�����ָ��
    uint8 encodeΪת���ɶ���ʱ���õı������Եĸ�ʽ��GB,..��
 *����
    0,1
 */

handle API_FS_FCreatebyLN(uint8 api_no, uint8 *UNstr, uint8 *SNstr, uint8 encode);
#define FS_FCreatebyLN(UNstr,SFNstr,encode) API_FS_FCreatebyLN(APINO_FS_FCREATEBYLN,UNstr,SFNstr,encode)
//-----------------------------------------------------------------------------------


/*  FS_JustBeing
 *����

 *����
  InputName ����ļ���Ŀ¼�����ַ�����ָ��
            ���û������ļ�����Ŀ¼�Ƿ����ʱ����ʼ���������Ƶ���ָ��ָ����ַ�����
            ��ʼ������ʱ������type��Ӧ�Ĳ������ͣ�����������߳���
  OutName    ���ļ���Ŀ¼���ڣ����Ҹ��û������OutName�ǿ�ʱ����������
            ������ļ���Ŀ¼�Ķ���


  JustType  Type�ĸ���λ��ʾһ�������������λ��ʾС�������ϸ��
            0x10    dir��ǰĿ¼���Ƿ����StrName���������������ļ�
            0x11    dir��ǰĿ¼���Ƿ����StrName����������������Ŀ¼
            0x12    dir��ǰĿ¼���Ƿ����StrName���������������ļ�����Ŀ¼
            0x20    dir��ǰĿ¼���Ƿ����StrName���������������ļ�
            0x21    dir��ǰĿ¼���Ƿ����StrName����������������Ŀ¼
            0x22    dir��ǰĿ¼���Ƿ����StrName���������������ļ�����Ŀ¼
 *����
  fail 0,succese,1
  ��������Success��ʾ�������д���������������������ļ���Ŀ¼������fail��ʾ������
 */
bool  API_FS_JustBeing (uint8 api_no, uint8 *InputName, uint8 *OutName, uint8 JustType);
#define FS_JustBeing(InputName, OutName,JustType) API_FS_JustBeing(APINO_FS_JUSTBEING,InputName, OutName,JustType)
bool  API_MFS_JustBeing (uint8 api_no, uint8 *InputName, uint8 *OutName, uint8 JustType);
#define MFS_JustBeing(InputName, OutName,JustType) API_MFS_JustBeing(APINO_MFS_JUSTBEING,InputName, OutName,JustType)
//-----------------------------------------------------------------------------------

/*
*********************************************************************************************************
*Function	��ɾ���ļ���ĳһ������Ϣ	2007-1-22 18:46
*
*Description���ú�������ɾ��һ���ļ���ĳһ���֣��м���߽�β����������ȫ������Ϣ
*
*Arguments	: StartSec	����ɾ���Ŀ�ʼ����λ�ã�������ļ��׵�����ƫ�ƣ�
*			  EndSec	����ɾ���Ľ�������λ�ã�������ļ��׵�����ƫ�ƣ�
*			  hFile		��ǰ�ļ�������ľ��
*
*Rerurns	��0			failed
*			  1			successful
*
*Other		:
*********************************************************************************************************
*/
bool FS_FRemovePart (long StartSec, long EndSec, handle hFile);
/*
*********************************************************************************************************
*Function	������ļ���ĳ���ֵ���һ���ļ�	2007-1-25 20:18
*
*Description���ú�����������ļ���ĳ���֣��м���߽�β����������ȫ��������һ���ļ�
*
*Arguments	: InsertSec	�ļ�1�Ĳ���㣨������ļ��׵�����ƫ�ƣ�
*			  StartSec	�ļ�2�Ĳ��벿�ֵ���ʼ����λ�ã�������ļ��׵�����ƫ�ƣ�
*			  EndSec	�ļ�2�Ĳ��벿�ֵĽ�������λ�ã�������ļ��׵�����ƫ�ƣ�
*			  hFile1	����������޸ĵ��ļ��ľ��
*			  hFile2	Ҫ���벢����ɾ����������Ҫ���ܲ�����ɾ������������ɾ�������ļ��ľ��
*
*Rerurns	��0			failed
*			  1			successful
*
*Other		:
*********************************************************************************************************
*/
bool FS_FAddFile (long InsertSec, long StartSec, long EndSec, handle hFile1, handle hFile2);

/*
*********************************************************************************************************
*Function	������FAT1������FAT2	2007-3-26 17:10
*
*Description���ú������ڿ���FAT1������FAT2
*
*Arguments	:
*
*Rerurns	��0			failed
*			  1			successful
*
*Other		:
*********************************************************************************************************
*/
bool FS_CopyFATSec (void);
/*
*********************************************************************************************************
*Function	��alter file attribute        2007-7-5 11:12
*
*Description��used just after FS_FOPen() or FS_FCreate()
*
*Arguments	: 	hFile-----file handle
*			mode ---- ATTR_READ_ONLY (0x01), indicates that writes to the file should fail.
*                       ATTR_HIDDEN (0x02), indicates that normal directory listings should not show this file.
*                       ATTR_SYSTEM (0x04), indicates that this is an operating system file.
*                       ATTR_ARCHIVE (0x20), supports backup utilities.
*
*
*Rerurns	��0			failed
*			  1			successful
*
*Other		:
*********************************************************************************************************
*/
bool API_FS_FAttrAlter (uint8 api_no, handle hFile, uint8 mode);
#define FS_FAttrAlter(hFile,mode) API_FS_FAttrAlter(APINO_FS_FATTRALTER,hFile,mode)
/*
*********************************************************************************************************
*Function	��rename a file        2007-9-17 16:07
*
*Description��Change the name of a file according to the new name from application.
*
*Arguments	: NewName ---- address of new short name (8 filename and 3 ext).
*
*
*Rerurns	��0			failed
*			  1			successful
*
*Other		: used just after FS_FOPen() or FS_FCreate()
*********************************************************************************************************
*/
bool API_FS_FRename (uint8 api_no, uint8 *NewName);
#define FS_FRename(NewName) API_FS_FRename(APINO_FS_FRENAME,NewName)
//-----------------------------------------------------------------------------------

/*  FS_ModUnalignedLen()
 *����
    �����ļ�ʱ���޸ı�ʾ�ļ���ȷ���ȵ��ļ��ֽ�����
 *����
    UnalignedLen    �ļ���β�������ֽ�����1~512��
    hFile   ��ǰд�ļ�������ľ��

 *����
  fail 0,succese,1
 */
bool  API_FS_ModUnalignedLen (uint8 api_no, int UnalignedLen , handle hFile );
#define FS_ModUnalignedLen(UnalignedLen,hFile) API_FS_ModUnalignedLen(APINO_FS_MODUNALIGNEDLEN,UnalignedLen,hFile)
//-----------------------------------------------------------------------------------


bool  FS_FFRead (word fileoffset, uint8 *databuf, handle hFile );
//-----------------------------------------------------------------------------------

/*  FS_GetFileStartClusNum()
 *����
    ȡ���ļ����״غţ���Ҫ��FS_FOPen��FS_FOpenbyLN��������ã�������ļ�����������������ص��ļ����
 *����
    hFile  �ļ����
    type   ��ʱδʹ��

 *����
  fail 0,succese,1
 */
long  FS_GetFileStartClusNum(handle hFile, uint8 type);

/*
*********************************************************************************************************
*Function	���޸��ļ�������
*
*Description�����ļ�������չ������ΪСд��ʾ��
*
*Arguments	: NameAttr-0x00 �ļ�������չ��Ϊ��д
*			          -0x08 �ļ���ΪСд,��չ��Ϊ��д
*					  -0x10 �ļ���Ϊ��д����չ��ΪСд
*				      -0x18 �ļ�������չ������Сд
*
*
*Rerurns	��0			failed
*			  1			successful
*
*Other		: ֻ���� FS_FOPen() ���� FS_FCreate()��ʹ�á�
*********************************************************************************************************
*/
bool API_FS_FSetNameAttr (uint8 api_no, uint8 NameAttr);
#define FS_FSetNameAttr(NameAttr) API_FS_FSetNameAttr(APINO_FS_FSETNAMEATTR,NameAttr)
//-----------------------------------------------------------------------------------
/*  FS_DirLast()
 *����
    ��λ����ǰĿ¼�����
 *����
	void

 *����
  fail 0,succese,1
 */
bool  API_FS_DirLast(uint8 api_no);
#define FS_DirLast() API_FS_DirLast(APINO_FS_DIRLAST)
//-----------------------------------------------------------------------------------
/*  FS_DirParNext()
 *����
    ��Ŀ¼ָ��ָ����һ��Ŀ¼��ָ��ǰĿ¼��Ŀ¼�����һ��Ŀ¼�����ǰָ��ָ���
    �Ǹ�Ŀ¼����ô������������ش���
 *����
	void
 *����
  fail 0,succese,1
 */
bool  API_FS_DirParNext(uint8 api_no);
#define FS_DirParNext() API_FS_DirParNext(APINO_FS_DIRPARNEXT)
//-----------------------------------------------------------------------------------
/*  FS_DirParPrev()
 *����
    ��Ŀ¼ָ��ָ����һ��Ŀ¼��ָ��ǰĿ¼��Ŀ¼�����һ��Ŀ¼�����ǰָ��ָ���
    �Ǹ�Ŀ¼����ô������������ش���
 *����
	void
 *����
  fail 0,succese,1
 */
bool  API_FS_DirParPrev(uint8 api_no);
#define FS_DirParPrev() API_FS_DirParPrev(APINO_FS_DIRPARPREV)
//-----------------------------------------------------------------------------------
/*  FS_DirPrev()
 *����:
    �ڵ�ǰĿ¼�¼����ļ�����Ŀ¼��ͨ���������ѡ�������Ŀ¼�����ļ�
    ���øú������ú����ӵ�ǰĿ¼��ָ�뿪ʼȡһ���ļ�����Ŀ¼������
    a.����FS_DirLast���ٵ���FS_DirPrev����ȡ���һ����Ч��Ŀ¼�
    b.����FS_SetCurDirEntry()�趨һ��Ŀ¼�DirPrev�����ظ�Ŀ¼���ǰһ����ЧĿ¼�
    c.����ͨ�����DirPrev���ص��ǵ�ǰĿ¼���ǰһ����ЧĿ¼�
 *����:
    Type�����������DIR�Ķ���,0��DIR�ļ�,1��ʾDIRĿ¼��
    uint8 *StrName����������������ļ�����bufferָ��
    uint8 *StrExtName����������չ����Ϣ,����DIR����������'*'��ʾdir���������ļ�
    uint8	ExtNameNum ��ʾStrExtName����չ���ĸ���
 *����:
    0,1
 */
bool  API_FS_DirPrev(uint8 api_no, uint8 *StrName, const uint8 *StrExtName, uint8 Type, uint8	ExtNameNum);
#define FS_DirPrev(StrName,StrExtName,Type,ExtNameNum) API_FS_DirPrev(APINO_FS_DIRPREV,StrName,StrExtName,Type,ExtNameNum)

//-----------------------------------------------------------------------------------
/*  FS_GetCheckSum()
 *����
    ��ȡ�ļ���У��ͣ�ֻ���ڵ���fs_open�Ժ����Ч��ͬʱֻ��exfat��Ч������fat32����0
 *����
	void
 *����

 */
//uint16 FS_GetCheckSum(void);

uint16 API_FS_GetCheckSum(uint8 api_no);
#define FS_GetCheckSum() API_FS_GetCheckSum(APINO_FS_GETCHECKSUM)

//-----------------------------------------------------------------------------------
/*  FS_SetMode()
 *����
    ����uhost��ȡ��ʽ
 *����
	void
 *����
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
 *����
    ���ļ�ϵͳ��ȡnor���ʵĲ����ռ䣬ֻ��װ��nor_fsʱ���ӿڲ���Ч
 *����
	sec_count ��������������
 *����
  fail 0,succese,1
 */

bool API_FS_NorErase(uint8 cmd, uint16 sec_count);
#define FS_NorErase(sec_count) API_FS_NorErase(APINO_FS_NorErase,sec_count)


#endif /*__C251__ */

//#endif /*__A251__ */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif  /* _FILESYS_INCLUDE */


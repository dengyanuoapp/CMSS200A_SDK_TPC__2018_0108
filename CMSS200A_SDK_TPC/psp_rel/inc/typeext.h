/****************************************************************
NAME:     TypeExt.h
ABSTRACT: Define extended type
Copyright (c) 2003 by Actions, All Rights Reserved.


*****************************************************************/
#ifndef _TYPEEXT_H
#define _TYPEEXT_H


#ifdef PC
#define __C251__
#define xdata
#include "simulatorpc.h"
#endif

#ifdef __C251__
#define OLD_TYPEEXT

// typedef unsigned char   bool;              // bool 变量
#define  bool unsigned char

typedef unsigned char   uint8;            // 无符号8位整型变量 //
typedef signed char     int8;             // 有符号8位整型变量 //

#ifndef PC
typedef unsigned int    uint16;           // 无符号16位整型变量 //
typedef signed int      int16;            // 有符号16位整型变量 //
#else
typedef unsigned short    uint16;           // 无符号16位整型变量 //
typedef signed short      int16;            // 有符号16位整型变量 //
#endif //endof ifndef PC

typedef unsigned int    IntAddr;		 // 一般用保存变量地址，由于在不同系统下，该地址宽度有差异，
// 所以特定于该变量类型，避免代码处过多的宏处理

typedef unsigned long   uint32;           // 无符号32位整型变量 //
typedef signed long     int32;            // 有符号32位整型变量 //


#ifdef OLD_TYPEEXT
#define  uchar unsigned char
#ifndef __DEC
// #define  uchar unsigned char
#define  BYTE unsigned char
#ifndef PC //10-4-11 防止与windows.h冲突
#define  byte unsigned char
typedef unsigned char   BOOL;              // BOOL 变量
#define  UCHAR unsigned char
#else
typedef  unsigned char byte;
#define BOOL unsigned char              // BOOL 变量
typedef  unsigned char UCHAR;
#endif
#endif	/* #ifndef __DEC */
#define  WORD unsigned short
#define  word unsigned short
#define  DWORD unsigned long
#define  dword unsigned long
#define  HANDLE unsigned int
#define  handle unsigned int
#define  HANDLE32 unsigned long
#define  handle32 unsigned long
#define  ulong unsigned long
#define  HANDLE32 unsigned long
#endif //end of ifdef OLD_TYPEEXT
#endif //end of __C251__



#define  TRUE   1
#define  FALSE  0

#define  FOREVER 1
#define  NEVER 0

#define  uint unsigned short

#ifndef NULL
#define  NULL 0
#endif

#endif  /* _TYPEEXT_H*/

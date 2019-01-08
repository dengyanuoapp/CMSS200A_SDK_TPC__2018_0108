/*
********************************************************************************
*                       ACTOS
*                     fwcfg head file
*
*                (c) Copyright 2002-2010, Actions Co,Ld.
*                        All Right Reserved
*
* File   : cfg.h
* By     : Mars
* Version: v1.00     first version     2010-03-29 11:35
* Modify :
********************************************************************************
*/
#ifndef CFG_H
#define CFG_H
#include "ap_common.h"
//#include "key.h"
//#include "lang_id.h"
#include "typeext.h"




const paraset_head_t ParaHead;
const key_cfg_t KeyCfg;
const ap_cfg_head_t APCfg;
const lang_infor_t LanguageCfg;
const file_ex_t FileCfg;
const value_t  ValueSet[];
const string_t StringSet[];

const func_cfg_t FuncCfg;
#endif
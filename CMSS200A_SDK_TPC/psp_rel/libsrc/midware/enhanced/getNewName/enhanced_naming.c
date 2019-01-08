/*
 *******************************************************************************
 *                ACTOS AP
 *        enhanced module impletementation
 *
 *        (c) Copyright, Actions Co,Ld.
 *             All Right Reserved
 *
 *******************************************************************************
 */
#pragma name(MW_EH_NAME)
#pragma renamecode(MW_EH_NAME)

#include <actos.h>
#include <enhanced.h>
#include <stringH.h>

#include "filesys.h"
//#include <debug.h>


/*
 ********************************************************************************
 * Description : 数字转换成字符串
 *
 * Arguments   : i, 数字
 *               buf, 输出字符串指针
 *
 * Returns     : 成功, 输出字符串指针
 *
 * Notes       : 数字只能是 01~99, 没有0结尾
 *
 ********************************************************************************
 */
char *my_itoa(int i, uint8 *buf)
{
    buf[0] = ((uint8)((i / 100) % 10)) + '0';
    buf[1] = ((uint8)((i / 10) % 10)) + '0';
    buf[2] = ((uint8)(i % 10)) + '0';
    return buf;
}
/*
 ********************************************************************************
 * Description : 根据序号生成新的可用文件名
 *
 * Arguments   : location->filename, 原来的文件名
 *               location->filename, 输出新的文件名
 *
 * Returns     :  新文件的序号, 01~99
 *        如果01~99 文件存在, 返回0
 *
 * Notes       :文件名字格式 : xxx000 WAV, xxx000 ACT, ***000 JPG...
 *        如果输入的文件不存在,则直接返回
 检查目录满最长时间15s
 顺序产生下一个目录名最长时间1s.
 *
 * TODO:        不要使用 atoi(), 节省128byte  (ok!)
 ********************************************************************************
 */
#ifdef PC
int sGenNewName(file_location_t *location)
#else
int GenNewName(file_location_t *location)
#endif
//int sGenNewName(file_location_t *location)
{
    int num;
    int loops = 0;
    uint8 new_name[12];
    handle f;

    //ASSERT(location->filename[3] >= '0' && location->filename[3] <= '9');
    //ASSERT(location->filename[4] >= '0' && location->filename[4] <= '9');
    //ASSERT(location->filename[5] >= '0' && location->filename[5] <= '9');

    //复制到new_name
    memcpy(new_name, location->filename, 12);

    // 检查文件夹是否含有001这个文件,如没有RETURN 1
    new_name[3] = '0';
    new_name[4] = '0';
    new_name[5] = '1';
    // 如果文件不存在
    f = FS_FOpen(new_name, 0);
    if (f == NULL)
    {
        memcpy(location->filename, new_name, 11);
        location->filename[11] = 0;
        // 删除文件长度为0的文件.
        //文件名已经存在，但文件长度为0时open也失败。
        FS_FRemove(location->filename);
        return 1;
    }
    else
    {
        FS_FClose(f);
    }

    // 按指定的值,继续查找.
    memcpy(new_name, location->filename, 12);
    num = (new_name[3] - '0') * 100 + (new_name[4] - '0') * 10 + (new_name[5] - '0');
    //    if(num <= 0) num = 1;
    //    if(num > 200) num = 1;
    //	my_itoa(num, &new_name[3]);
    if ((num <= 0) || (num > 200))
    {
        num = 1;
        my_itoa(num, &new_name[3]);
    }

    //如果文件存在,数字加1
    do
    {
        // 如果文件不存在
        ClearWatchDog();
        f = FS_FOpen(new_name, 0);
        if (f == NULL)
        {
            memcpy(location->filename, new_name, 11);
            location->filename[11] = 0;
            FS_FRemove(location->filename);
            return num;
        }
        else
        {
            FS_FClose(f);
            num++;
            if (num > 200)
            {
                num = 1;
            }
            my_itoa(num, &new_name[3]);
        }
        loops++;
    }
    while (loops < 200);

    return 0;
}

#include <actos.h>
#include <enhanced.h>
#include <stringH.h>
#include <sysdef.h>
#include <rand.h>

#pragma name(MO_EH_USERPL)
uint8 User_ItemTotal;
uint8 User_ItemCur;
uint8 User_FirstOffset;
uint8 User_LastOffset;
uint8 User_fileoffset;
uint16 vm1_addr;

extern char MW_User_buffer[512];
extern uint8  fselError;    // store error id;
extern uint8  fselMode;    //Mode of select, Sequence, repeat one ...
extern uint16 fselRandCount;
extern uint8 fselSetLocationC(file_location_t *location);
extern uint16 shuffle_next_file(void);
extern uint16 shuffle_prev_file(void);
extern bool init_shuffle(uint16 num_total);

#pragma renamecode(MO_EH_USERPL)
BOOL GetFilebyItem(uint8 *filename, uint16 ItemNum);

static BOOL Read_PL(uint8 offset)
{
    VMRead(MW_User_buffer, vm1_addr + offset / ONE_SEC_FILE * 0x200, 0x200);
    return TRUE;
}


bool fselGetLocationPL(file_location_t  *location)
{
    //User_fileoffset = 0;
    location->PL_Cur = User_ItemCur;
    if(Read_PL(User_fileoffset))
    {
        if(MW_User_buffer[(User_fileoffset % ONE_SEC_FILE)*USER_PLAYLIST_SIZE] == 1)
        {
            memcpy(location, MW_User_buffer + (User_fileoffset % ONE_SEC_FILE)*USER_PLAYLIST_SIZE + 1, 38);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}
uint8 fselSetLocationPL(file_location_t  *location)
{
    uint8 *dir_buf;
    //    handle fp_list;
    if(FS_CD(":") == FALSE)
    {
        return FALSE;
    }
    dir_buf = (uint8 *)(((uint32)location) + ITEM_LOCATION_SIZE + (EXFAT_INFO_OFFSET - LAST_OFFSET));
    FS_SetCurDir_layer(dir_buf);
    //if(location->path[0] != ':')
    {
        if (!FS_SetCurDir(location->ClusterNo))
        {
            fselError = FSEL_ERR_SETDIR;                             //目录不存在
            return FALSE;
        }
        if (FALSE == FS_SetCurDirEntry(location->DirEntry))
        {
            fselError = FSEL_ERR_SETDIR;
            return FALSE;
        }

    }
    return TRUE;

}
bool UserPL_next(char *filename)
{
    uint8 temp;
    uint16 temp1;
    file_location_t  *tempt;
    int result = FALSE;
    switch(fselMode)
    {
    case FSEL_ALL_REPEATONE:  //current file
        if(Read_PL(User_fileoffset))
        {
            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            tempt = (file_location_t *)(MW_User_buffer + temp1 + 1);
            if(fselSetLocationPL(tempt))
            {
                memcpy(filename, tempt->filename, 12);
                result = TRUE;
            }
        }
        break;

    case FSEL_ALL_RANDOM:  //random file in current class
        temp1 = shuffle_next_file();
        if(temp1 == 0)
        {
            init_shuffle(User_ItemTotal);
        }
        else
        {
            result = GetFilebyItem(filename, (uint8)temp1);
        }
        break;

    case FSEL_DIR_SEQUENCE:
    case FSEL_DIR_INTRO:
    case FSEL_ALL_SEQUENCE:     //all file sequence select the next file                  if(User_ItemCur< User_ItemTotal)
        if(User_ItemCur < User_ItemTotal)
        {
            User_ItemCur++;
        }
        else
        {
            result = FALSE;
            break;
        }
        if(Read_PL(User_fileoffset))
        {
            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            //memcpy(&temp,temp+MW_User_buffer+NEXT_OFFSET,1);
            temp = *(temp1 + MW_User_buffer + NEXT_OFFSET);
            if((temp / ONE_SEC_FILE) != (User_fileoffset / ONE_SEC_FILE))
            {
                Read_PL(temp);
            }
            User_fileoffset = temp;
            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            tempt = (file_location_t *)(MW_User_buffer + temp1 + 1);
            if(fselSetLocationPL(tempt))
            {
                memcpy(filename, tempt->filename, 12);
                result = TRUE;
            }

        }
        break;




    case FSEL_ALL_REPEATALL:
    case FSEL_DIR_REPEATALL:  // in the same class get the file
        if(User_ItemCur < User_ItemTotal)
        {
            User_ItemCur++;
            if(!Read_PL(User_fileoffset))
            {
                break;
            }
            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            memcpy(&temp, temp1 + MW_User_buffer + NEXT_OFFSET, 1);
            if((temp / ONE_SEC_FILE) != (User_fileoffset / ONE_SEC_FILE))
            {
                Read_PL(temp);
            }
        }
        else
        {
            User_ItemCur = 1;
            temp = User_FirstOffset;
            Read_PL(temp);
        }
        User_fileoffset = temp;
        temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
        tempt = (file_location_t *)(MW_User_buffer + temp1 + 1);
        if(fselSetLocationPL(tempt))
        {
            memcpy(filename, tempt->filename, 12);
            result = TRUE;
        }


        break;


    default:
        return FALSE;
    }
    return result;

}


bool UserPL_prev(char *filename)
{
    uint8 temp;
    uint16 temp1;
    file_location_t  *tempt;
    int result = FALSE;
    switch(fselMode)
    {
    case FSEL_ALL_REPEATONE:  //current file
        if(Read_PL(User_fileoffset))
        {
            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            tempt = (file_location_t *)(MW_User_buffer + temp1 + 1);
            if(fselSetLocationPL(tempt))
            {
                memcpy(filename, tempt->filename, 12);
                result = TRUE;
            }
        }

        break;

    case FSEL_ALL_RANDOM:  //random file in current class
        temp1 = shuffle_prev_file();
        if(temp1 == 0)
        {
            init_shuffle(User_ItemTotal);
        }
        else
        {
            result = GetFilebyItem(filename, (uint8)temp1);
        }
        break;

    case FSEL_DIR_SEQUENCE:
    case FSEL_DIR_INTRO:
    case FSEL_ALL_SEQUENCE:     //all file sequence select the next file                  if(User_ItemCur< User_ItemTotal)
        if(User_ItemCur > 1)
        {
            User_ItemCur--;
        }

        else
        {
            result = FALSE;
            break;
        }
        if(Read_PL(User_fileoffset))
        {
            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            memcpy(&temp, temp1 + MW_User_buffer + LAST_OFFSET, 1);
            if((temp / ONE_SEC_FILE) != (User_fileoffset / ONE_SEC_FILE))
            {
                Read_PL(temp);
            }
            User_fileoffset = temp;
            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            tempt = (file_location_t *)(MW_User_buffer + temp1 + 1);
            if(fselSetLocationPL(tempt))
            {
                memcpy(filename, tempt->filename, 12);
                result = TRUE;
            }
        }
        break;

    case FSEL_ALL_REPEATALL:
    case FSEL_DIR_REPEATALL:  // in the same class get the file
        if(User_ItemCur > 1)
        {
            User_ItemCur--;
            if(!Read_PL(User_fileoffset))
            {
                break;
            }

            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            memcpy(&temp, temp1 + MW_User_buffer + LAST_OFFSET, 1);
            if((temp / ONE_SEC_FILE) != (User_fileoffset / ONE_SEC_FILE))
            {
                Read_PL(temp);
            }
        }
        else
        {
            User_ItemCur = User_ItemTotal;
            temp = User_LastOffset;
            Read_PL(temp);
        }
        User_fileoffset = temp;
        temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
        tempt = (file_location_t *)(MW_User_buffer + temp1 + 1);
        if(fselSetLocationPL(tempt))
        {
            memcpy(filename, tempt->filename, 12);
            result = TRUE;
        }
        break;

    default:
        return FALSE;
    }
    return result ;
}

bool GetFilebyItem(uint8 *filename, uint16 ItemNum)
{
    uint8 temp;
    uint16 temp1;
    uint8 num;
    file_location_t  *tempt;
    if(ItemNum == 0xffff || ItemNum == 0)
    {
        User_ItemCur = 1;
        User_fileoffset = User_FirstOffset;
    }
    else
    {
        if(ItemNum > User_ItemTotal)
        {
            return FALSE;
        }

        num = 1;
        User_fileoffset = User_FirstOffset;
        Read_PL(User_fileoffset);
        while(num != ItemNum)
        {
            temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
            memcpy(&temp, temp1 + MW_User_buffer + NEXT_OFFSET, 1);
            if((temp / ONE_SEC_FILE) != (User_fileoffset / ONE_SEC_FILE))
            {
                Read_PL(temp);
            }
            User_fileoffset = temp;
            num++;
        }
    }
    if(Read_PL(User_fileoffset))
    {

        temp1 = (User_fileoffset % ONE_SEC_FILE) * USER_PLAYLIST_SIZE;
        tempt = (file_location_t *)(MW_User_buffer + temp1 + 1);
        if(fselSetLocationPL(tempt))
        {
            User_ItemCur = ItemNum;
            memcpy(filename, tempt->filename, 12);
            return TRUE;
        }

    }
    User_ItemCur = ItemNum;
    return TRUE;
}

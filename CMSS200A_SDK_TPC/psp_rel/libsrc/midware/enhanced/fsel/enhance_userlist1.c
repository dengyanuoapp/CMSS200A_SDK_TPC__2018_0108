
#include "actos.h"
#include "enhanced.h"
#include "string.h"

#pragma name(MO_EH_USERPL1)
extern uint8 User_ItemTotal;
extern uint8 User_ItemCur;
extern uint8 User_FirstOffset;
extern uint8 User_LastOffset;
extern uint8 User_fileoffset;
extern uint8 fselError;    // store error id;
extern uint16 vm1_addr;


extern uint8  fselError;    // store error id;
extern uint8  fselMode;    //Mode of select, Sequence, repeat one ...
extern uint16 fselRandCount;
extern uint16 Random1(void);
extern bool fselGetLocationPL(file_location_t  *location);
extern uint8 fselSetLocationPL(file_location_t  *location);
extern bool GetFilebyItem(uint8 *filename, uint16 ItemNum);
extern bool init_shuffle(uint16 num_total);

char MW_User_buffer[512];
const uint8 far vfilename1[7] = "USERPL";

#pragma renamecode(MO_EH_USERPL1)


BOOL fselUserPLEnt(file_location_t  *location)
{
    favi_head_t  head;

    VMRead(MW_User_buffer, VM_FAVI_HEAD, 0x80);
    if(location->disk == 'H')
    {
        memcpy(&head, MW_User_buffer + 0x40, sizeof(favi_head_t));
    }
    else
    {
        memcpy(&head, MW_User_buffer, sizeof(favi_head_t));
    }

    if(memcmp(head.magic, vfilename1, 6) == 0)
    {
        //magic 正确
        User_fileoffset = head.first_offset;
        User_LastOffset = head.last_offset;
        User_ItemTotal = head.total_item;
        User_ItemCur = 0;
        User_FirstOffset = User_fileoffset;
        if(User_ItemTotal == 0)
        {
            return FALSE;
        }
#ifdef _ACTIVE_SHUFFLE
        init_shuffle(User_ItemTotal);
#endif
        if(location->disk == 'H')
        {
            vm1_addr = VM_FAVI_CARD;
        }
        else
        {
            vm1_addr = VM_FAVI_HOST;
        }
        return TRUE;
    }
    return FALSE;
}

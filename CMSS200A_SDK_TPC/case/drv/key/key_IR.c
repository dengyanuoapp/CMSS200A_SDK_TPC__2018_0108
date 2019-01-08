#include "drv_key.h"

#pragma name(KY_IR)

extern uint8 oldkey;
extern uint8 IR_KEY;
extern uint8 tmp_count;

#define key_260c

#ifdef key_260c
#define    MAX_KEY_NUM    36
uint8 const IrcKeyValueTab_0x00bf[MAX_KEY_NUM] =
{
    0x0d, /*0x1c, 0x1b, 0x4d, 0x4f, 0x12,0x1f,*/ 0x56,
    0x13, 0x58, 0x4e, 0x59, 0x5a, 0x57, 0x5e, /*0x52, */
    /* 0x53, 0x54, 0x55,*/0x14, 0x16, 0x5c, 0x19, 0x15,
    0x18, 0x0e, 0x17, /*0x1e, */0x44, 0x11, 0x4a, 0x43,
    0x10, 0x4b, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0c, 0x00, 0x0b/*, 0x0f, 0x1a, */
    /*0x48, 0x1d, 0x49, 0x4c , 0x5d*/
};
uint8 const IrcKeyMsgTab_0x00bf[MAX_KEY_NUM] =
{
    Msg_KeyPower, /*Msg_KeyNull,Msg_KeyNull, Msg_KeyNull, Msg_KeyNull, Msg_KeyNull, Msg_KeyNull,*/Msg_KeyChanSub,
    Msg_KeyUSBTest, Msg_KeyFolderSub, Msg_KeyPlayPause, Msg_KeyFolderAdd, Msg_KeyMicIn, Msg_KeyChanAdd, Msg_KeyREC, /*Msg_KeyPa0_1VolAdd,*/
    /*Msg_KeyPa0_1VolSub, Msg_KeyPaBassAdd, Msg_KeyPaBassSub, */Msg_KeyClock, Msg_KeyLast, Msg_KeyVol, Msg_KeyVolSub, Msg_KeyMenu,
    Msg_KeyVolAdd, Msg_KeyMute, Msg_KeyNext, /*Msg_KeyMute, */Msg_KeyVolSub, Msg_KeyCaraOk, Msg_KeyLast, Msg_KeyVolAdd,
    Msg_KeyEQ, Msg_KeyNext, Msg_KeyNum1, Msg_KeyNum2, Msg_KeyNum3, Msg_KeyNum4,  Msg_KeyNum5, Msg_KeyNum6,
    Msg_KeyNum7, Msg_KeyNum8, Msg_KeyNum9, Msg_KeyLoop, Msg_KeyNum0, Msg_Keyautosleep /*, Msg_KeyBassAdd, Msg_KeyMidAdd,*/
    /*Msg_KeyTrebAdd, Msg_KeyBassSub, Msg_KeyMidSub, Msg_KeyTrebSub ,Msg_KeyNull*/
};
#else
#define    MAX_KEY_NUM    21
uint8 const IrcKeyValueTab_0x00bf[MAX_KEY_NUM] =
{
    0x00, 0x01, 0x02, 0x04, 0x05, 0x06, 0x08, 0x09, 0x0a, 0x0c, 0x0d, 0x0e, 0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x18,
    0x19, 0x1a
};
uint8 const IrcKeyMsgTab_0x00bf[MAX_KEY_NUM] =
{
    Msg_KeyClock, Msg_KeyMenu, Msg_KeyPlayPause, Msg_KeyLoop, Msg_KeyFolderSub, Msg_KeyFolderAdd, Msg_KeyEQ,
    Msg_KeyVolSub, Msg_KeyVolAdd, Msg_KeyNum0, Msg_KeyLast, Msg_KeyNext, Msg_KeyNum1, Msg_KeyNum2, Msg_KeyNum3,
    Msg_KeyNum4, Msg_KeyNum5, Msg_KeyNum6, Msg_KeyNum7, Msg_KeyNum8, Msg_KeyNum9
};
#endif
#if 0
uint8 const IrcKeyValueTab_0x00ff[MAX_KEY_NUM] =
{
    0x45, 0x46, 0x47, 0x44, 0x40, 0x43, 0x07, 0x15, 0x09, 0x16, 0x19, 0x0d, 0x0c, 0x18, 0x5e, 0x08, 0x1c, 0x5a, 0x42,
    0x52, 0x4a
};
uint8 const IrcKeyMsgTab_0x00ff[MAX_KEY_NUM] =
{
    Msg_KeyPower, Msg_KeyMenu, Msg_KeyMute, Msg_KeyPlayPause, Msg_KeyLast, Msg_KeyNext, Msg_KeyEQ, Msg_KeyVolSub,
    Msg_KeyVolAdd, Msg_KeyNum0, Msg_KeyLoop, Msg_KeyClock, Msg_KeyNum1, Msg_KeyNum2, Msg_KeyNum3, Msg_KeyNum4,
    Msg_KeyNum5, Msg_KeyNum6, Msg_KeyNum7, Msg_KeyNum8, Msg_KeyNum9
};

uint8 const IrcKeyValueTab_0x807f[MAX_KEY_NUM] =
{
    0x12, 0x1a, 0x1e, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x1b, 0x1f, 0x0c, 0x0d, 0x0e, 0x00,
    0x0f, 0x19
};
uint8 const IrcKeyMsgTab_0x807f[MAX_KEY_NUM] =
{
    Msg_KeyPower, Msg_KeyMenu, Msg_KeyMute, Msg_KeyPlayPause, Msg_KeyLast, Msg_KeyNext, Msg_KeyEQ, Msg_KeyVolSub,
    Msg_KeyVolAdd, Msg_KeyNum0, Msg_KeyFolderSub, Msg_KeyFolderAdd, Msg_KeyNum1, Msg_KeyNum2, Msg_KeyNum3,
    Msg_KeyNum4, Msg_KeyNum5, Msg_KeyNum6, Msg_KeyNum7, Msg_KeyNum8, Msg_KeyNum9
};
#endif
void IR_Interrupt(void)
{
    uint8 i;
    uint8 key_data;
    uint8 sfr_bak = SFR_BANK;
    uint8 *keyval, *keymsg;
    uint8 tmp_ir_sta;
    SFR_BANK = BANK_IR;
    tmp_ir_sta = IR_STA;
    IR_STA |= 0x08; //clear irq pending
    key_data = IR_KDC; //read key data value
    //接受到错误的按键码
    if ((tmp_ir_sta & 0x02) != 0)
    {
        IR_STA |= 0x0B;//clear error msg,clear irq pending
        SFR_BANK = sfr_bak;
        return;
    }
    //接受到重复码时，直接用旧码值
    if ((tmp_ir_sta & 0x01) != 0)
    {
        //        IR_STA |= 0x09;          //clear repeat flag,clear irq pending
        tmp_count = 0;
        IR_KEY = 1;
        PutSysMsg(oldkey);
        SFR_BANK = sfr_bak;
        return;
    }
#if 0
    if (IR_HUC == 0xff)
    {
        keyval = (uint8 *) IrcKeyValueTab_0x00ff;
        keymsg = (uint8 *) IrcKeyMsgTab_0x00ff;
    }
    else if (IR_HUC == 0x7f)
    {
        keyval = (uint8 *) IrcKeyValueTab_0x807f;
        keymsg = (uint8 *) IrcKeyMsgTab_0x807f;
    }
    else
    {
        keyval = (uint8 *) IrcKeyValueTab_0x00bf;
        keymsg = (uint8 *) IrcKeyMsgTab_0x00bf;
        //        max_keynum = 36;
    }
#else
    keyval = (uint8 *) IrcKeyValueTab_0x00bf;
    keymsg = (uint8 *) IrcKeyMsgTab_0x00bf;
#endif
    for (i = 0; i < MAX_KEY_NUM; i++)
    {
        if (keyval[i] == key_data)
        {
            oldkey = keymsg[i];
            IR_KEY = 1;
            PutSysMsg(oldkey);
            break;
        }
    }
}

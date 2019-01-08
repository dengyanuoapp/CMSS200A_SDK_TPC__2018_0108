#ifndef __USOUND_EXTERN_H__
#define __USOUND_EXTERN_H__

#define USB_IDLE         0x00
#define PLAY_RUNNING     0x01
#define RECORD_RUNNING   0x02
#define USOUND_PLUG_OUT  0x04

#define  USB_IS_CONNECT    0x00
#define  USB_CONNECT_RESET 0x01
#define  USB_DISCONNECT    0x02


extern uint8 usb_state;
extern uint8 g_PAOpenFlag;

extern void usound_exit(void);
extern uint8 usound_init(void);
extern void record_test_init(void);
extern uint8 is_usb_connect(void);
extern void usound_key_msg(uint8 key_value);

#endif

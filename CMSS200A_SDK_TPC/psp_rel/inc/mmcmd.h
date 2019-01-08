/*
*******************************************************************************
*                ACTOS AP
*        multi-media command head file
*
*        (c) Copyright Actions Co,Ld.
*
*******************************************************************************
*/
#ifndef _MMCMD_H
#define _MMCMD_H

/* 功能操作类*/
#define     MC_OPEN             0x01
#define     MC_CLOSE            0x02
#define     MC_LOAD             0x03
#define     MC_PLAY             0x04
#define     MC_PAUSE            0x05
#define     MC_STOP             0x06
#define     MC_STEP             0x07
#define     MC_RECORD           0x08
#define     MC_STEPGRID         0x09
#define     MC_SEARCH           0x0a
#define     MC_SAVESTATION      0x0b
#define      MC_MUTE                0x0c
#define     MC_FADEINOUT        0x0d
#define        MC_FADEIN            0x0e
#define        MC_FADEOUT            0x0f
#define     MC_GETINF            0x10

#define     MC_GETFREQ        0x11
#define     MC_SETFREQ        0x12
#define     MC_CALTIME          0x13

/* 信息设定类*/
#define     MC_SETVOLUME        0x40
#define     MC_SETA_B           0x41
#define     MC_SETUSEREQ        0x42
#define     MC_SETEQ            0x43
#define     MC_BACKUP           0x44
#define     MC_RESTORE          0x45
#define     MC_SETSPEED         0x46
#define     MC_SETSTEP          0x47
#define     MC_SETCALLBACK      0x48
#define     MC_SETGAIN          0x49
#define     MC_SETAVRFLAG           0x50
#define     MC_SETAUDIOFMT      0x51
#define     MC_SETSOURCE        0x52
#define     MC_SETMODE          0x53
#define     MC_SETNOISE            0x54
#define     MC_DELONE            0x55
#define        MC_DELALL            0X56
#define     MC_RECODEMODE    0x57
#define        MC_SETVOX    0x58
#define	    MC_SETPOWERCOUNT	0x59	//added 2005-7-18 9:44

#define        MC_SETAVRLEVEL    0x60	//avr func add by mzh 2007.1.18
#define        MC_SETAVRDELAY    0x61	//avr func add by mzh 2007.1.18

#define       MC_SETGAIN_DSP     0x62 //add by ljp 2008-3-24 11:05

#define MC_SET_DIGITAL_VOLUME 	0x63
#define MC_SET_CHANNEL 0x64
#define       MC_SETSOFTVOLUMEMAX  0x6a
#define MC_SETGAIN_ADC 0x6c
#define MC_SET_SONG_TYPE 0x6d
#define MC_SET_RECORD_DELAY 0x6e
#define MC_AUTO_AB 0x6f

#define	    MC_SET_TTS			0xdd	//add for TTS

/* 信息读取类*/
#define     MC_GETTIME          0x80
#define     MC_GETSAPLE         0x81
#define     MC_GETAUDIOFMT      0x82
#define     MC_GETSTATUS        0x83
#define     MC_GETTABLE         0x84
#define	    MC_GETPOWERVALUE	0x85	//added 2005-7-18 9:44

#define MC_GET_VIDEO_REGION	0x86
#define MC_GET_VIDEO_STATUS	0x87

#endif  /* _MMCMD_H*/

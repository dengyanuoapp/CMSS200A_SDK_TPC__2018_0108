#ifndef _USBDISK_H
#define _USBDISK_H

#ifdef __C251__

#define     USBTESTABUFFER  0x5e0  //0x5E0-0x5FF(共32字节)

typedef struct
{
    int8 LeftChannel80hz;		//-12db,-11db,………….12db;
    int8 LeftChannel200hz;  	//-12db,-11db,………….12db;
    int8 LeftChannel488hz;		//-12db,-11db,………….12db;
    int8 LeftChannel980hz;		//-12db,-11db,………….12db;
    int8 LeftChannel4khz;		//-12db,-11db,………….12db;
    int8 LeftChannel8khz;		//-12db,-11db,………….12db;
    int8 LeftChannel16khz;		//-12db,-11db,………….12db;
    int8 RightChannel80hz;		//-12db,-11db,………….12db;
    int8 RightChannel200hz;  	//-12db,-11db,………….12db;
    int8 RightChannel488hz;		//-12db,-11db,………….12db;
    int8 RightChannel980hz;	    //-12db,-11db,………….12db;
    int8 RightChannel4khz;		//-12db,-11db,………….12db;
    int8 RightChannel8khz;		//-12db,-11db,………….12db;
    int8 RightChannel16khz;	    //-12db,-11db,………….12db;
} utest_eq_info_t;

typedef struct
{
    uint8 TSHDEn;	 			//0, 1
    uint8 InputMode; 			//0, 1
    uint8 OutputMode;			//0, 1
    uint8 LimiterEn;			//0, 1
    uint8 SpeakerSize;			//0,1,2,3,4,5,6,7
    uint8 TruBassEn;			//0, 1
    uint8 DefinitionLEn;		//0, 1
    uint8 DefinitionREn;		//0, 1
    uint8 FocusLEn;				//0, 1
    uint8 FocusREn;				//0, 1
    uint8 HighBitRate;			//0, 1
    uint8 SRS3DHeadphone;		// 0, 1
    uint8 SRS3Dmode;			//0,1,2,3
    uint8 SRS3DEn;				//0, 1
    uint8 SRSTruBassProcessMode;//0, 1
    uint8 SurroundEn;			//0, 1
    uint8 InputGain;			//0,1,2,3,4,5,6,7,8,9,10
    uint8 OutputGain;			//0,1,2,3,4,5,6,7,8,9,10
    uint8 BypassGain;			//0,1,2,3,4,5,6,7,8,9,10
    uint8 SurroundLevel;		//0,1,2,3,4,5,6,7,8,9,10
    uint8 LimiterControl;		//0,1,2,3,4,5,6,7,8,9,10
    uint8 SpaceControl;			//0,1,2,3,4,5,6,7,8,9,10
    uint8 CenterControl;		//0,1,2,3,4,5,6,7,8,9,10
    uint8 TruBassControl;		//0,1,2,3,4,5,6,7,8,9,10
    uint8 DefinitionControl;	//0,1,2,3,4,5,6,7,8,9,10
    uint8 FOCUSControl;			//0,1,2,3,4,5,6,7,8,9,10
    uint8 DialogClarityEn;		//0,1
    uint8 DialogClarityChannelFOCUSControl;	//0,1,2,3,4,5,6,7,8,9,10
} utest_srs_info_t;

typedef struct
{
    uint8 cmd;                  //EQSET=0x02
    uint8 status;
    utest_eq_info_t eqinfo;
} utest_eq_set_t;

typedef struct
{
    uint8 cmd;                  //EQSAVE=0x03
    uint8 status;               //0: OK 1:Not Ready 2:Fail 3:Not Support
    uint8 EQMode;               //1:normal 2:Rock 3:Pop 4:Classic 5:Soft 6:Jazz 7:Dbb 8:自定义
    utest_eq_info_t eqinfo;
} utest_eq_save_t;

typedef struct
{
    uint8 cmd;                  //SRSSET=0x04
    uint8 status;
    utest_srs_info_t srsinfo;
} utest_srs_set_t;

typedef struct
{
    uint8 cmd;                  //SRSSAVE=0x05
    uint8 status;               //0: OK 1:Not Ready 2:Fail 3:Not Support
    uint8 SRSMode;              //1:music 2:movie 3:game 4:voice 5:自定义
    utest_srs_info_t srsinfo;
} utest_srs_save_t;

typedef struct
{
    uint8 cmd;                  //VOLUMESET=0x06
    uint8 status;
    uint8 MuteEn;               //0:mute enalbe 1:mute disable
    uint8 VolumeLevel;          //0,1,...10;
} utest_volume_set_t;

#define     EQSET           0x02
#define     EQSAVE          0x03
#define     SRSSET          0x04
#define     SRSSAVE         0x05
#define     VOLUMESET       0x06

#define     UTESTOK              0x00
#define     UTESTNOTREADY        0x01
#define	    UTESTFAIL            0x02
#define     UTESTNOTSUPPORT      0x03

bool UTestRun( );
bool UTestExit(void);

#endif /*__C251__*/
#endif /*_USBDISK_H*/





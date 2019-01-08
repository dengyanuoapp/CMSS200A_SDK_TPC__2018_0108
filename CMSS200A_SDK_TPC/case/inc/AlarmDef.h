#ifndef  ALARMDEF_H
#define  ALARMDEF_H

typedef struct
{
    int year;
    int month;
    int day;
} Alarm_Date;



typedef enum
{
    ALARMNONE, ALARAMType, FMType
} TALARMTYPE;

typedef enum
{
    Music_NON = 0, Music_MP1, Music_MP2, Music_MP3, Music_WMA,
    Music_WMV,
    Music_WAV,
    Music_TypeMax
} Music_type_a;


typedef struct
{
    uint16 magic;
    BOOL AlarmEnable;
    date_t alarmDate;
    time_t alarmTime;
    file_location_t location; //响应闹钟时所播放音乐的路径
    uint8 volume; //闹钟音乐播放时，音量的大小
    uint8 UdiskDriveNO;
} alarm_vars_t;

#endif

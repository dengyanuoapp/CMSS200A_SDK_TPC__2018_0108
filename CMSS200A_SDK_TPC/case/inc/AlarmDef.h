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
    file_location_t location; //��Ӧ����ʱ���������ֵ�·��
    uint8 volume; //�������ֲ���ʱ�������Ĵ�С
    uint8 UdiskDriveNO;
} alarm_vars_t;

#endif

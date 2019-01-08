#ifndef TTS_LIB_H
#define TTS_LIB_H

//TTS索引表头数据结构,占据32byte,reserved 24byte
typedef struct
{
    int wordcnt;        //TTS字库汉字的个数                 //完整库6768
    int bytes1;         //每个汉字信息所占字节数            // 8
    int multi_cnt;      //多音字其他发音个数                //当前15个
    int bytes2;         //占用字节数                        //8
    int citiaocnt;      //扩展词条的个数                    //31
    int bytes3;         //每个扩展词条信息所占字节数        // 6
} tts_header_t;

typedef struct
{
    long start_offset;  //
    int16 str_length;    //
    int16 multi_offset;  //      多音字链表偏移，0XFFFF代表没有多音字
} tts_pos_t;

typedef struct
{
    char multitone_tag; //
    char qu;    //
    char wei;   //      多音字链表偏移，0XFFFF代表没有多音字
} tts_multitone_t;

#define     TTS_MULTI_CON       0x01        //多音
#define     TTS_MULTI_PREV      0x02        //上文
#define     TTS_MULTI_NEXT      0x09        //下文
#define     TTS_MULTI_END       0x0F        //结束

extern SD_FILE *m_de_TTS_Fp;
extern SD_FILE *info_TTS_Fp;
//extern Open_param_t *m_deTTSOpenparam;
extern play_status_t  TTSplay_status;
extern BYTE *m_deTTSDataBuffer;//[512];//解码时用的BUFFER

extern uint32  m_deTTSOldDspIntAddrSav;
extern uint32  m_deTTSOldDmaIntAddrSav;
extern uint8  m_deTTScTimer_Sav;
extern uint8  m_deTTSInitFlag;
extern unsigned char  prev_next_flag;
extern WORD  deTTSSecCount;
extern WORD  deTTSSecOffset;

extern unsigned char *str_buff;
extern unsigned char  str_ptr;
extern unsigned char  tts_lib_flag;
extern tts_header_t  tts_info_head;

extern const char tts_lib[];
extern const char tts_info[];

/* Basal_de_tts_b.c */
bool TTSLoadCommand(BYTE music_type, void *param);
bool TTSStopCommand(void *param);
bool TTSPlayCommand(void *param);

/* Basal_de_tts_RD.c */
uint8 m_deTTSRead_Data(uchar readFileDirection, DWORD position);
void fix_head_data(tts_header_t *tts_info_head);
void fix_pos_data(tts_pos_t *pos);

/* basal_de_tts_a.c */
bool pSdTtsSendCommand(BYTE cmd, BYTE music_type, void *param);

#endif

/*
 *******************************************************************************
 *                ACTOS AP
 *        FM tunner basal module implementation
 *
 *        (c) Copyright Actions Co,Ld.
 *
 *******************************************************************************
 */
#include "ap_radio.h"

#pragma name(MOD_BS_FMRADIO)

uint16 m_FMStationTab[MAX_STATION_COUNT]; //��̨Ƶ�ʶ��ձ�(ע:Ƶ�ʷ�Χ87M-108M ��KΪ����).

static uint8 cmd_fm_mid_findstation(void *param1, void *param2);

/*********************************************************
 *input:  buf---ptr point to buffer which is wait to deal
 *          curfreq----current freq which will be inserted to buffer
 *          totalnum---current total freq num in freq list
 *output: no
 *Description:  insert current freq into freq list
 *note:  freq list from small to large
 **********************************************************
 */
bool SequenceFreqlist(uint16 *buf, uint16 curfreq, uint8 totalnum)
{
    uint8 i, cur_pos;
    uint16 *dealbuf;
    int8 j;

    dealbuf = buf;
    cur_pos = 0;

    for (i = 0; i < MAX_STATION_COUNT; i++)
    {
        //dealbuf �������Ǵ�С��������

        if (curfreq < (*dealbuf)) //С�뵱ǰָ��ģ���ѵ�ǰ�Ժ�������ƣ������뵽��ǰ
        {
            cur_pos = i;
            if (cur_pos == (MAX_STATION_COUNT - 1)) //��������һ�ֱ���滻
            {
                *dealbuf = curfreq;
                g_FMStatus.station = i + 1;
                break;
            }

            for (j = (MAX_STATION_COUNT - 2); j >= (int8) cur_pos; j--) //��С�������򣬲����м䣬��Ҫ�Ѻ��������������һλ
            {
                *(buf + j + 1) = *(buf + j);
            }
            *dealbuf = curfreq; //�������м�
            g_FMStatus.station = i + 1;

            break;
        }
        else if (curfreq == (*dealbuf))
        {
            return FALSE; //���ظ��������±�
        }
        else
        {
            if ((*dealbuf == 0) && (i < totalnum)) //��⵽����ˣ�ֱ�Ӳ��뵽��β
            {
                *dealbuf = curfreq;
                g_FMStatus.station = i + 1;
                break;
            }
            dealbuf++;
        }
    }
    return TRUE;
}

/*
 ********************************************************************************
 *             void FM_VolumeSet(uint8 Vol)
 *
 * Description : ����PA����
 *
 * Arguments   :	  �����õ�����ֵ
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
void FM_VolumeSet(uint8 Vol)
{
    set_dac_volume(Vol);
}

//��װfm����, ����ģ���ʼ��
uint8 cmd_fm_open(void *param1, void *param2)
{
    //�β�ȷ��OK
    //param1 ��������ʱû�õ��ģ��������ݽ�ȥ
    uint8 mode = (uint8) param1;
    param2 = param2;

    BankSwitch((uint32) FMdrv_mute); //����פ�����л�����
    return FMdrv_Init(mode);
}

//ģ�����standby, ��ж��fm����
uint8 cmd_fm_close(void *param1, void *param2)
{
    //�β�ȷ��OK
    param1 = param1;
    param2 = param2;

    BankSwitch((uint32) FMdrv_mute); //����פ�����л�����
    return FMdrv_Standby();
}

//FM Ӳ���������
uint8 cmd_fm_hardware(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint8 onoff = (uint8) param1; //onoff:  1--��FMʱ    0--�ر�FMʱ
    uint8 PA_mode = (uint8) param2; //PA_mode:   1-- Ҫ����PA    0--������PA

    BankSwitch((uint32) FMdrv_mute); //����פ�����л�����
    return FMdrv_hardware(onoff, PA_mode);
}

//���õ���Ƶ�㲥��
uint8 cmd_fm_setfreq(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint8 result;
    uint16 Freq = (uint16) param1;
    param2 = param2;

    result = FMdrv_setfreq((void *) Freq);

    return result;
}

//FM������������
uint8 cmd_fm_mute(void *param1, void *param2)
{
    //�β�ȷ��OK
    FM_MUTE_e mode = (uint8) param1;
    param2 = param2;
    return FMdrv_mute(mode);
}

//���ò���
uint8 cmd_fm_setband(void *param1, void *param2)
{
    //�β�ȷ��OK
    radio_band_e band = (uint8) param1;
    param2 = param2;
    return FMdrv_setband(band);
}

//������̨����
uint8 cmd_fm_setth(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint8 level = (uint8) param1;
    param2 = param2;

    BankSwitch((uint32) FMdrv_mute); //����פ�����л�����
    return FMdrv_setthrod(level);
}

//���������̨
uint8 cmd_fm_search(void *param1, void *param2)
{
    //�β�ȷ��OK
    fm_status_t *sptr = (fm_status_t *) param1;
    uint8 SeekDirect = (*(uint8 *) param2);

    return FMdrv_search(sptr->freq, SeekDirect);
}

//����seek (Ӳ����̨)
uint8 cmd_fm_seek(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint16 start = (uint16) param1;
    uint16 stop = (uint16) param2;
    uint8 step = 1;
    return FMdrv_seek(start, stop, step);
}

//��ȡseek״̬
uint8 cmd_fm_getseekflag(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint8 *flag = param1;
    param2 = param2;
    return FMdrv_getseekflag(flag);
}

//�˳�seek
uint8 cmd_fm_breakseek(void *param1, void *param2)
{
    //�β�ȷ��OK
    param1 = param1;
    param2 = param2;
    return FMdrv_breakseek(); //�˳�Ӳ����̨
}

//��ȡ��ǰƵ��ֵ
uint8 cmd_fm_getfreq(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint8 result;
    uint16 *pfreg = param1;
    param2 = param2;
    result = FMdrv_getfreq(pfreg);
    //���ݵ�̨Ƶ���ҵ���̨��
    cmd_fm_mid_findstation(NULL, NULL);
    return result;
}

//��ȡ��ǰ��̨�ź�ǿ��
uint8 cmd_fm_getintensity(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint8 *pvalue = param1;
    param2 = param2;
    return FMdrv_getintensity(pvalue);
}

//��ȡ��ǰ��̨״̬
uint8 cmd_fm_getstatus(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint8 result;
    fm_status_t *pBuf = param1; //FMStatus_Tab_t �ṹ������Ҳ����
    param2 = param2;
    result = FMdrv_getstatus(pBuf);
    //���ݵ�̨Ƶ���ҵ���̨��
    cmd_fm_mid_findstation(NULL, NULL);
    return result;
}

//���ŵ�̨
uint8 cmd_fm_mid_play(void *param1, void *param2)
{
    //�β�ȷ��OK
    uint16 Tempfreq;

    param1 = param1;
    param2 = param2;

    //FMcmd(FM_MUTE, SetMUTE,NULL);   //����
    fmSendCommand(FM_MUTE, SetMUTE, NULL);

    {
        Tempfreq = g_FMStatus.freq; //(g_FMStatus.freq << 6) + 0x0010;
    }

    FMdrv_setfreq(Tempfreq);

    //Ӧ����ʱһ��
    ap_sleep(1);

    FMdrv_mute(releaseMUTE); //�������

    return 0;
}

//���浱ǰ��̨
uint8 cmd_fm_mid_save(void *param1, void *param2)
{
    //���ɴ������:MC_BACKUP ����

    param2 = param2;

#ifndef PC
    memcpy(((FMRADIO_param_t *) param1)->param1, m_FMStationTab, sizeof(m_FMStationTab));
    memcpy(((FMRADIO_param_t *) param1)->param2, &g_FMStatus, sizeof(fm_status_t));
#else
    memcpy((void *)param1, m_FMStationTab, sizeof(m_FMStationTab));
    memcpy((void *)(param1 + 2), &g_FMStatus, sizeof(fm_status_t));
#endif
    return 0;
}

//���浱ǰ��̨��Ӧ�ĵ�̨����š�
uint8 cmd_fm_mid_savestation(void *param1, void *param2)
{
    //uint8 i,k;
    uint8 result = FALSE;
    param1 = param1;
    param2 = param2;

    if (g_FMStatus.station_count >= MAX_STATION_COUNT)
    {
        return FALSE;
    }

    //�ѵ�̨Ƶ�ʱ��浽��̨������Ѿ����ڣ��᷵��ʧ��
    g_FMStatus.station_count++;
    if (SequenceFreqlist(m_FMStationTab, g_FMStatus.freq, g_FMStatus.station_count) == FALSE)
    {
        g_FMStatus.station_count--; //���ʧ��
        return FALSE;
    }

    return TRUE;

}

//���ҵ�ǰ��̨��Ӧ�ĵ�̨����š�
//������ g_FMStatus.station ��
uint8 cmd_fm_mid_findstation(void *param1, void *param2)
{
    uint8 i = g_FMStatus.station_count;
    param1 = param1;
    param2 = param2;
    while (i--)
    {
        if (g_FMStatus.freq == m_FMStationTab[i])
        {
            g_FMStatus.station = (i + 1);
            return 0;
        }
    }
    g_FMStatus.station = 0;
    return 0;
}

//���ݵ�̨����ż���Ƶ�ʡ�
uint8 cmd_fm_mid_loadfreq(void *param1, void *param2)
{
    uint8 i = g_FMStatus.station_count;
    param1 = param1;
    param2 = param2;
    if (g_FMStatus.station_count != 0)
    {
        g_FMStatus.freq = m_FMStationTab[g_FMStatus.station - 1];
    }
    return 0;
}

//��ԭ��ǰ��̨
uint8 cmd_fm_mid_restore(void *param1, void *param2)
{
    param2 = param2;

#ifndef PC
    memcpy(m_FMStationTab, ((FMRADIO_param_t *) param1)->param1, sizeof(m_FMStationTab));
    memcpy(&g_FMStatus, ((FMRADIO_param_t *) param1)->param2, sizeof(fm_status_t));
#else
    memcpy(m_FMStationTab, (void *)param1, sizeof(m_FMStationTab));
    memcpy(&g_FMStatus, (void *)(param1 + 2), sizeof(fm_status_t));
#endif
    return 0;
}

//�л���һ����̨
uint8 cmd_fm_mid_turn(void *param1, void *param2)
{
    uint8 i;

    //param1 Ϊ UP �� DOWN
    param2 = param2;

    if (g_FMStatus.station_count >= 1) //�е�̨
    {
        if (g_FMStatus.station) //��̨�Ѿ�������
        {
            if (param1 == UP)
            {
                g_FMStatus.station++;
                if (g_FMStatus.station > g_FMStatus.station_count)
                {
                    g_FMStatus.station = 1;
                }
            }
            else if (param1 == DOWN)
            {
                g_FMStatus.station--;
                if (g_FMStatus.station == 0)
                {
                    g_FMStatus.station = g_FMStatus.station_count;
                }
            }
        }
        else
        {
            //��̨��û���棬Ӧ���������̨���ĸ�̨��̨֮��
            i = g_FMStatus.station_count;
            while (i)
            {
                //��̨��С��������ģ����ҵ�С�ڴ�Ƶ���ĵ�̨
                if (g_FMStatus.freq > m_FMStationTab[--i])
                {
                    i++;
                    break;
                }
            }
            //i == 0 ʱ��Ƶ�ʱ��κ�һ����̨����
            //i == g_FMStatus.station_count ,����κ�һ����̨����
            //�����̨�� i �� i+ 1 ֮��
            if (param1 == UP)
            {
                if (i == g_FMStatus.station_count)
                {
                    g_FMStatus.station = 1;
                }
                else
                {
                    g_FMStatus.station = i + 1;
                }
            }
            else if (param1 == DOWN)
            {
                if (i == 0)
                {
                    g_FMStatus.station = g_FMStatus.station_count;
                }
                else
                {
                    g_FMStatus.station = i;
                }
            }

        }
        g_FMStatus.freq = m_FMStationTab[g_FMStatus.station - 1];

        FMdrv_setfreq((void *) g_FMStatus.freq);

        return TRUE;
    }

    return FALSE;
}

//ɾ����ǰ��̨
//��g_FMStatus.station Ϊ��׼��ɾ��
//���g_FMStatus.station Ϊû���棬���� g_FMStatus.freq Ϊ��׼
uint8 cmd_fm_mid_delete(void *param1, void *param2)
{
    //���ɴ������:MC_DELONE ����

    uint8 k;

    param1 = param1;
    param2 = param2;

    if (g_FMStatus.station == 0)
    {
        cmd_fm_mid_findstation(NULL, NULL); //�����̨û���棬ȷ���Ƿ����û����
        if (g_FMStatus.station == 0)
        {
            return FALSE;
        }
    }
    //m_FMStationTab[g_FMStatus.station-1]=0;
    //for (k = g_FMStatus.station; k < (g_FMStatus.station_count + 1); k++)
    for (k = g_FMStatus.station; k < (g_FMStatus.station_count); k++)
    {
        m_FMStationTab[k - 1] = m_FMStationTab[k]; //�м���Ҫɾ�������Ժ������Ҫǰ��һλ
    }
    g_FMStatus.station_count--;
    m_FMStationTab[g_FMStatus.station_count] = 0;
    g_FMStatus.station = 0;

    return TRUE;
}

//ɾ��ȫ����̨
uint8 cmd_fm_mid_delall(void *param1, void *param2)
{
    //���ɴ������:MC_DELALL ����
    uint8 i;

    param1 = param1;
    param2 = param2;

    for (i = 0; i < MAX_STATION_COUNT; i++)
    {
        m_FMStationTab[i] = 0;
    }
    g_FMStatus.station = 0;
    g_FMStatus.station_count = 0;

    return 0;
}

//ȷ����ѡ���еĵ�һ�����е�̨
uint8 cmd_fm_mid_selcityinit(void *param1, void *param2)
{
    param1 = param1;
    param2 = param2;

    return 0;
}

//����ѡ�����������л����е�̨
uint8 cmd_fm_mid_cityswitch(void *param1, void *param2)
{
    param1 = param1;
    param2 = param2;
    return 0;
}

//��������
uint8 cmd_fm_mid_setvolume(void *param1, void *param2)
{
    uint8 Vol = (uint8) param1;
    param2 = param2;

    FM_VolumeSet(Vol);
    return 0;
}

//��̨Ƶ��΢������100KHZΪ����
uint8 cmd_fm_mid_stepgrid(void *param1, void *param2)
{
    uint16 Tempfreq;

    uint8 dir = (uint8) param1;

    param2 = param2;

    switch (dir)
    {
    case UP:
        if (((g_FMStatus.freq + 1) > USFREQ_MAX) && (g_radio_config.band_mode == Bank_US_Europe))
        {
            g_FMStatus.freq = USFREQ_MIN;
        }
        else if (((g_FMStatus.freq + 1) > JPFREQ_MAX) && (g_radio_config.band_mode == Bank_Japan))
        {
            g_FMStatus.freq = JPFREQ_MIN;
        }
        else
        {
            g_FMStatus.freq += 1;
        }
        break;
    case DOWN:
        if ((g_FMStatus.freq == USFREQ_MIN) && (g_radio_config.band_mode == Bank_US_Europe))
        {
            g_FMStatus.freq = USFREQ_MAX;
        }
        else if ((g_FMStatus.freq == JPFREQ_MIN) && (g_radio_config.band_mode == Bank_Japan))
        {
            g_FMStatus.freq = JPFREQ_MAX;
        }
        else
        {
            g_FMStatus.freq -= 1;
        }
        break;
    default:
        break;
    }

    Tempfreq = g_FMStatus.freq; //(g_FMStatus.freq << 6) + 0x0010;

    FMdrv_setfreq(Tempfreq);
    return TRUE;
}

const pfunc_cmd far fm_cmd_pfunc[CMD_COUNT] =
{
    /* FM �м�������, �����漰������FM�����ӿ� */
    cmd_fm_open, //��װfm����, ����ģ���ʼ��
    cmd_fm_close, //ģ�����standby, ��ж��fm����
    cmd_fm_hardware, //FM Ӳ���������
    cmd_fm_setfreq, //���õ���Ƶ�㲥��
    cmd_fm_mute, //FM������������
    cmd_fm_setband, //���ò���
    cmd_fm_setth, //������̨����
    cmd_fm_search, //���������̨
    cmd_fm_seek, //����seek (Ӳ����̨)
    cmd_fm_getseekflag, //��ȡseek״̬
    cmd_fm_breakseek, //�˳�seek

    cmd_fm_getfreq, //��ȡ��ǰƵ��ֵ
    cmd_fm_getintensity, //��ȡ��ǰ��̨�ź�ǿ��
    cmd_fm_getstatus, //��ȡ��ǰ��̨״̬

    //�м��߼���:��FM�����޹�
    cmd_fm_mid_play,
    cmd_fm_mid_save, //���浱ǰ��̨
    cmd_fm_mid_savestation,
    cmd_fm_mid_findstation, //���ҵ�ǰ��̨��Ӧ�ĵ�̨����š�
    cmd_fm_mid_loadfreq, //���ݵ�̨����ż���Ƶ�ʡ�
    cmd_fm_mid_restore, //��ԭ��ǰ��̨
    cmd_fm_mid_turn, //��̨
    cmd_fm_mid_delete, //ɾ����ǰ��̨
    cmd_fm_mid_delall, //ɾ��ȫ����̨
    cmd_fm_mid_selcityinit, //ȷ����ѡ���еĵ�һ�����е�̨
    cmd_fm_mid_cityswitch, //����ѡ�����������л����е�̨

    cmd_fm_mid_setvolume, //��������
    cmd_fm_mid_stepgrid
};

/*
 ********************************************************************************
 *             uint8 FMcmd(uint8 cmd, uint16 param1, void *param2)
 *
 * Description : FM�����
 *
 * Arguments   :
 *
 * Returns     :
 *
 * Notes       :
 *
 ********************************************************************************
 */
uint8 fmSendCommand(radio_cmd_e cmd, void *param1, void *param2)
{
    uint8 ret = 0;
    if (cmd < CMD_COUNT)
    {
        ret = (fm_cmd_pfunc[(uint16) cmd]) (param1, param2);
    }

    return ret;
}

